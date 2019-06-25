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
	// オブジェクトの要素を順に処理
	m_objectA = std::make_unique<CollisionObject<Collisions::Sphere>>();
	m_objectA->Initialize(context);
	m_objectA->m_objectPos = Vector3::Zero;
	m_objectA->m_objectColor = Colors::Yellow;
	m_objectA->m_objectVel = Vector3::Zero;
	m_objectA->m_objectSize = Vector3::One;
	m_objectA->m_objectWeight = 1;

	// オブジェクトの要素を順に処理
	m_objectB = std::make_unique<CollisionObject<Collisions::Capsule>>();
	m_objectB->Initialize(context);
	m_objectB->m_objectPos = Vector3::Right;
	m_objectB->m_objectColor = Colors::Blue;
	m_objectB->m_objectVel = Vector3::Zero;
	m_objectB->m_objectSize = Vector3::One;
	m_objectB->m_objectWeight = 1;
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
		m_objectA->m_objectAcc = input * .01f;
	}

	// オブジェクトの更新
	m_objectA->Update(context);
	m_objectB->Update(context);

	if (Collisions::IsHit(*m_objectA->m_objectCollider, *m_objectB->m_objectCollider))
	{
		std::swap(m_objectA->m_objectVel, m_objectB->m_objectVel);
	}
}

void MyGame::Render(GameContext & context)
{
	// オブジェクトの描画
	m_objectA->Render(context);
	m_objectB->Render(context);
}

void MyGame::Finalize(GameContext & context)
{
	// オブジェクトの解放
	m_objectA->Finalize(context);
	m_objectB->Finalize(context);
}
