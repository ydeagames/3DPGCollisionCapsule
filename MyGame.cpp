#include "pch.h"
#include "MyGame.h"
#include "GameCamera.h"
#include "GameContext.h"
#include "Collision.h"
#include "Input.h"
#include "DebugCamera.h"
#include "InfinityGridFloor.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace physx;

MyGame::MyGame()
{
}

MyGame::~MyGame()
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

void MyGame::Initialize(GameContext& context)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	// グリッド床作成
	m_pGridFloor = std::make_unique<InfinityGridFloor>(context, 1.0f);
	// デバッグカメラ作成
	m_pDebugCamera = std::make_unique<DebugCamera>();

	// オブジェクトの要素を順に処理
	{
		auto& obj = m_objectA;

		obj = std::make_unique<CollisionObject<Collisions::Capsule>>();
		obj->Initialize(context);
		obj->m_objectPos = Vector3(0, 0, 0);
		obj->m_objectColor = Colors::Yellow;
		obj->m_objectVel = Vector3::Zero;
		obj->m_objectSize = Vector3::One;
		obj->m_objectRot = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, XMConvertToRadians(45));
		obj->m_objectWeight = 10;

		float radius = std::max({ obj->m_objectSize.x, obj->m_objectSize.y, obj->m_objectSize.z }) * .5f;
		PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(radius, obj->m_objectSize.y - radius), *gMaterial);
		PxTransform localTm(PxVec3(obj->m_objectPos.x, obj->m_objectPos.y, obj->m_objectPos.z));
		auto body = gPhysics->createRigidDynamic(localTm);
		body->attachShape(*shape);
		PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		gScene->addActor(*body);
		obj->m_objectRigidbody = body;
	}

	// オブジェクトの要素を順に処理
	for (int iz = -4; iz <= 4; iz++) for (int ix = -4; ix <= 4; ix++)
	{
		if (-1 <= ix && ix <= 1 && -1 <= iz && iz <= 1)
			continue;
		auto obj = std::make_unique<CollisionObject<Collisions::Capsule>>();
		obj->Initialize(context);
		obj->m_objectPos = Vector3(ix * 1.05f, 0, iz * 1.05f);
		obj->m_objectColor = Colors::Blue;
		obj->m_objectVel = Vector3::Zero;
		obj->m_objectSize = Vector3::One;
		obj->m_objectWeight = 1;

		float radius = std::max({ obj->m_objectSize.x, obj->m_objectSize.y, obj->m_objectSize.z }) * .5f;
		PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(radius, obj->m_objectSize.y - radius), *gMaterial);
		PxTransform localTm(PxVec3(obj->m_objectPos.x, obj->m_objectPos.y, obj->m_objectPos.z));
		auto body = gPhysics->createRigidDynamic(localTm);
		body->attachShape(*shape);
		PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		gScene->addActor(*body);
		obj->m_objectRigidbody = body;

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

	template<typename T>
	void Collision(T& obj1, T& obj2)
	{
		if (Collisions::IsHit(*obj1.m_objectCollider, *obj2.m_objectCollider))
		{
			auto norm = Collisions::GetHitNormal(*obj1.m_objectCollider, *obj2.m_objectCollider);

			auto splitVecA = Decompose(norm, obj1.m_objectVel);
			auto splitVecB = Decompose(norm, obj2.m_objectVel);
			obj1.m_objectVel = splitVecB.first + splitVecA.second;
			obj2.m_objectVel = splitVecA.first + splitVecB.second;

			//float f = m_objectA->m_objectCollider->Radius / (m_objectA->m_objectCollider->Radius + obj->m_objectCollider->Radius);
			//auto center = Vector3::Lerp(m_objectA->m_objectPos, obj->m_objectPos, f);
			//m_objectA->m_objectPos = center - norm * (1 - f);
			//obj->m_objectPos = center + norm * f;
		}
	}

	Vector3 EulerAngle(const Quaternion& q)
	{
		Vector3 euler;

		// roll (x-axis rotation)
		float sinr_cosp = +2.0f * (q.w * q.x + q.y * q.z);
		float cosr_cosp = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		euler.x = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		float sinp = +2.0f * (q.w * q.y - q.z * q.x);
		if (std::fabs(sinp) >= 1)
			euler.y = std::copysign(XM_PI / 2, sinp); // use 90 degrees if out of range
		else
			euler.y = std::asin(sinp);

		// yaw (z-axis rotation)
		float siny_cosp = +2.0f * (q.w * q.z + q.x * q.y);
		float cosy_cosp = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		euler.z = std::atan2(siny_cosp, cosy_cosp);

		return euler;
	}
}

void MyGame::Update(GameContext & context)
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);

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

		auto force = Vector3::Transform(input, rotation) * 500.f;
		m_objectA->m_objectRigidbody->addForce(PxVec3(force.x, force.y, force.z));
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

	//for (auto& obj1 : m_objectB)
	//	Collision(*obj1, *m_objectA);

	//for (auto& obj1 : m_objectB)
	//	for (auto& obj2 : m_objectB)
	//		if (obj1 < obj2)
	//		{
	//			Collision(*obj1, *obj2);
	//		}
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

	{
		std::wostringstream sb;

		{
			auto& trans = m_objectA->m_objectPos;
			sb << L"カプセル: トランスフォーム" << std::endl;
			sb << L"  座標: ("
				<< std::fixed << std::setprecision(2) << trans.x << L", "
				<< std::fixed << std::setprecision(2) << trans.y << L", "
				<< std::fixed << std::setprecision(2) << trans.z << L")" << std::endl;

			auto euler = EulerAngle(m_objectA->m_objectRot);
			sb << L"  回転: ("
				<< std::fixed << std::setprecision(2) << XMConvertToDegrees(euler.x) << L", "
				<< std::fixed << std::setprecision(2) << XMConvertToDegrees(euler.y) << L", "
				<< std::fixed << std::setprecision(2) << XMConvertToDegrees(euler.z) << L")" << std::endl;

			auto scale = m_objectA->m_objectSize;
			sb << L"  サイズ: ("
				<< std::fixed << std::setprecision(2) << scale.x << L", "
				<< std::fixed << std::setprecision(2) << scale.y << L", "
				<< std::fixed << std::setprecision(2) << scale.z << L")" << std::endl;
		}
		std::wstring strpos = sb.str();
		auto pos = context.GetCamera().ViewportToScreenPoint(Vector3(1, 1, 0)) + Vector3::Left * Vector3(m_font->MeasureString(strpos.c_str())).x;
		m_font->DrawString(m_batch.get(), strpos.c_str(), pos, Colors::White);
	}
	{
		std::wostringstream sb;

		{
			Vector3 trans, scale;
			Quaternion rot;
			context.GetCamera().view.Decompose(scale, rot, trans);
			sb << L"カメラ: トランスフォーム" << std::endl;
			sb << L"  座標: ("
				<< std::fixed << std::setprecision(2) << trans.x << L", "
				<< std::fixed << std::setprecision(2) << trans.y << L", "
				<< std::fixed << std::setprecision(2) << trans.z << L")" << std::endl;

			auto euler = EulerAngle(rot);
			sb << L"  回転: ("
				<< std::fixed << std::setprecision(2) << XMConvertToDegrees(euler.x) << L", "
				<< std::fixed << std::setprecision(2) << XMConvertToDegrees(euler.y) << L", "
				<< std::fixed << std::setprecision(2) << XMConvertToDegrees(euler.z) << L")" << std::endl;

			sb << L"  サイズ: ("
				<< std::fixed << std::setprecision(2) << scale.x << L", "
				<< std::fixed << std::setprecision(2) << scale.y << L", "
				<< std::fixed << std::setprecision(2) << scale.z << L")" << std::endl;
		}
		std::wstring strpos = sb.str();
		auto pos = context.GetCamera().ViewportToScreenPoint(Vector3(1, -1, 0)) - Vector3(m_font->MeasureString(strpos.c_str()));
		m_font->DrawString(m_batch.get(), strpos.c_str(), pos, Colors::White);
	}

	m_batch->End();

	//static auto primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context.GetDR().GetD3DDeviceContext());
	//primitiveBatch->Begin();
	//primitiveBatch->DrawLine(VertexPositionColor(Vector3(-1, 0, 0), Colors::White), VertexPositionColor(Vector3(1, 1, 0), Colors::Blue));
	//primitiveBatch->End();
}

void MyGame::Finalize(GameContext & context)
{
	// オブジェクトの解放
	m_objectA->Finalize(context);
	for (auto& obj : m_objectB)
		obj->Finalize(context);
}
