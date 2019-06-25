#include "pch.h"
#include "MyGame.h"
#include "GameCamera.h"
#include "GameContext.h"
#include "Collision.h"
#include "Input.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MyGame::MyGame()
{
}

MyGame::~MyGame()
{
}

void MyGame::Initialize(GameContext & context)
{
	// �I�u�W�F�N�g�̗v�f�����ɏ���
	m_objectA = std::make_unique<CollisionObject<Collisions::Capsule>>();
	m_objectA->Initialize(context);
	m_objectA->m_objectPos = Vector3::Zero;
	m_objectA->m_objectColor = Colors::Yellow;
	m_objectA->m_objectVel = Vector3::Zero;
	m_objectA->m_objectSize = Vector3::One * 4;
	m_objectA->m_objectRot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, XMConvertToRadians(45));
	m_objectA->m_objectWeight = 1;

	// �I�u�W�F�N�g�̗v�f�����ɏ���
	for (int iz = -5; iz <= 5; iz++) for (int ix = -5; ix <= 5; ix++)
	{
		auto obj = std::make_unique<CollisionObject<Collisions::Sphere>>();
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
	// ���`���
	float Lerp(float a, float b, float x)
	{
		return a + (b - a) * x;
	}
}

void MyGame::Update(GameContext & context)
{
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
		m_objectA->m_objectAcc = input * .01f;
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

	// �I�u�W�F�N�g�̍X�V
	m_objectA->Update(context);
	for (auto& obj : m_objectB)
		obj->Update(context);

	for (auto& obj : m_objectB)
	{
		if (Collisions::IsHit(*m_objectA->m_objectCollider, *obj->m_objectCollider))
		{
			//std::swap(m_objectA->m_objectVel, m_objectB->m_objectVel);
			obj->m_objectAcc.y = .2f;
		}
		else
		{
			obj->m_objectAcc.y = -.2f;
		}
		if (obj->m_objectPos.y <= 0.f)
		{
			obj->m_objectPos.y = 0.f;
			obj->m_objectVel.y = 0.f;
		}
	}
}

void MyGame::Render(GameContext & context)
{
	// �I�u�W�F�N�g�̕`��
	m_objectA->Render(context);
	for (auto& obj : m_objectB)
		obj->Render(context);

	// �����`��
	m_batch->Begin();
	m_font->DrawString(m_batch.get(), L"WASD ���ړ�\nQE �㉺�ړ�\nZC �g��k��\n���h���b�O �f�o�b�O�J����\n�E�h���b�O �I�u�W�F��]", Vector3::Zero, Colors::White);
	m_batch->End();
}

void MyGame::Finalize(GameContext & context)
{
	// �I�u�W�F�N�g�̉��
	m_objectA->Finalize(context);
	for (auto& obj : m_objectB)
		obj->Finalize(context);
}
