#include "pch.h"
#include "MyGame.h"
#include "GameCamera.h"
#include "GameContext.h"
#include "Collision.h"
#include "Input.h"
#include "DebugCamera.h"
#include "InfinityGridFloor.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MyGame::MyGame()
{
}

MyGame::~MyGame()
{
}

void MyGame::Initialize(GameContext& context)
{
	// グリッド床作成
	m_pGridFloor = std::make_unique<InfinityGridFloor>(context, 1.0f);
	// デバッグカメラ作成
	m_pDebugCamera = std::make_unique<DebugCamera>();

	// オブジェクトの要素を順に処理
	m_objectA = std::make_unique<CollisionObject<Collisions::Capsule>>();
	m_objectA->Initialize(context);
	m_objectA->m_objectPos = Vector3(2, 0, 0);
	m_objectA->m_objectColor = Colors::Yellow;
	m_objectA->m_objectVel = Vector3::Zero;
	m_objectA->m_objectSize = Vector3::One;
	m_objectA->m_objectRot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, XMConvertToRadians(45));
	m_objectA->m_objectWeight = 1;

	// オブジェクトの要素を順に処理
	for (int iz = 0; iz <= 0; iz++) for (int ix = 0; ix <= 0; ix++)
	{
		auto obj = std::make_unique<CollisionObject<Collisions::Capsule>>();
		obj->Initialize(context);
		obj->m_objectPos = Vector3(float(ix), 0, float(iz));
		obj->m_objectColor = Colors::Blue;
		obj->m_objectVel = Vector3::Zero;
		obj->m_objectSize = Vector3::One;
		obj->m_objectWeight = 1;
		m_objectB.emplace_back(std::move(obj));
	}

	m_font = std::make_unique<SpriteFont>(context.GetDR().GetD3DDevice(), L"logofont.spritefont");
	m_batch = std::make_unique<SpriteBatch>(context.GetDR().GetD3DDeviceContext());
}

namespace
{
	// 線形補間
	float Lerp(float a, float b, float x)
	{
		return a + (b - a) * x;
	}

	std::pair<Vector3, Vector3> Decompose(const Vector3 & normalVec, const Vector3 & inVec)
	{
		// 位置の差のベクトルに対して垂直なベクトルと水平な方向に速度ベクトルを分解
		auto normalVecNorm = normalVec;
		auto inVecNorm = inVec;
		normalVecNorm.Normalize();
		inVecNorm.Normalize();

		// 先生のArrow.cpp
		auto rotAngle = std::acos(normalVecNorm.Dot(inVecNorm));
		auto rotAxis = normalVecNorm.Cross(inVecNorm);
		if (rotAxis.Length() == 0.0f) rotAxis = Vector3(0.0f, 0.0f, 1.0f);

		// 90度回転して垂直なベクトル軸
		auto verticalNorm = Vector3::Transform(normalVecNorm, Quaternion::CreateFromAxisAngle(rotAxis, XMConvertToRadians(90)));
		auto inVecLength = inVec.Length();
		// sin cosで長さを加味
		auto vecLateral = verticalNorm * inVecLength * std::sin(rotAngle);
		auto vecDirection = normalVecNorm * inVecLength * std::cos(rotAngle);

		return std::make_pair(vecDirection, vecLateral);
	}
}

void MyGame::Update(GameContext & context)
{
	m_pDebugCamera->update();
	Input::Update();

	auto key = Keyboard::Get().GetState();
	{
		auto input = Vector3::Zero;
		if (key.A || key.Left)
			input.x -= 1;
		if (key.D || key.Right)
			input.x += 1;
		if (key.W || key.Up)
			input.z -= 1;
		if (key.S || key.Down)
			input.z += 1;
		if (key.Q || key.LeftShift)
			input.y -= 1;
		if (key.E || key.Space)
			input.y += 1;
		if (key.Z)
			m_objectA->m_objectSize /= 1.1f;
		if (key.C)
			m_objectA->m_objectSize *= 1.1f;

		Vector3 s_, t_;
		Quaternion rotation;
		context.GetCamera().view.Decompose(s_, rotation, t_);
		rotation.Inverse(rotation);

		m_objectA->m_objectAcc = Vector3::Transform(input, rotation) * .01f;
	}

	static auto lastMouse = Vector3::Zero;
	if (Input::GetMouseButtonDown(Input::Buttons::MouseRight))
	{
		lastMouse = Input::GetMousePosition();
	}
	else if (Input::GetMouseButton(Input::Buttons::MouseRight))
	{
		auto mouse = Input::GetMousePosition();
		auto delta = (mouse - lastMouse) * .001f;
		m_objectA->m_objectRot *= Quaternion::CreateFromYawPitchRoll(delta.x, delta.y, 0);
	}

	// オブジェクトの更新
	m_objectA->Update(context);
	for (auto& obj : m_objectB)
		obj->Update(context);

	for (auto& obj : m_objectB)
	{
		if (Collisions::IsHit(*m_objectA->m_objectCollider, *obj->m_objectCollider))
		{
			auto norm = Collisions::GetHitNormal(*m_objectA->m_objectCollider, *obj->m_objectCollider);
			auto normNorm = norm;
			normNorm.Normalize();

			auto splitVecA = Decompose(norm, m_objectA->m_objectVel);
			auto splitVecB = Decompose(norm, obj->m_objectVel);
			m_objectA->m_objectVel = splitVecB.first + splitVecA.second;
			obj->m_objectVel = splitVecA.first + splitVecB.second;
			//float f = m_objectA->m_objectCollider->Radius / (m_objectA->m_objectCollider->Radius + obj->m_objectCollider->Radius);
			//auto center = Vector3::Lerp(m_objectA->m_objectPos, obj->m_objectPos, f);
			//m_objectA->m_objectPos = center - norm * .5f;
			//obj->m_objectPos = center + norm * .5f;
		}
	}
}

void MyGame::Render(GameContext & context)
{
	auto& cam = context.GetCamera();
	cam.view = Matrix::CreateTranslation(-m_objectA->m_objectPos) * m_pDebugCamera->getViewMatrix();
	m_pGridFloor->draw(context);

	// オブジェクトの描画
	m_objectA->Render(context);
	for (auto& obj : m_objectB)
		obj->Render(context);

	// 文字描画
	m_batch->Begin();
	m_font->DrawString(m_batch.get(), L"WASD 横移動\nQE 上下移動\nZC 拡大縮小\n左ドラッグ デバッグカメラ\n右ドラッグ オブジェ回転", Vector3::Zero, Colors::White);
	m_batch->End();
}

void MyGame::Finalize(GameContext & context)
{
	// オブジェクトの解放
	m_objectA->Finalize(context);
	for (auto& obj : m_objectB)
		obj->Finalize(context);
}
