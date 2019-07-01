#pragma once
#include "Collision.h"
#include "CapsulePrimitive.h"
#include "CollisionModels.h"

class GameContext;

template<typename T>
class CollisionObject;

namespace CollisionGenerators
{
	template<typename T>
	T GetCollision(CollisionObject<T>& obj);
}

template<typename T>
class CollisionObject
{
public:
	std::unique_ptr<CollisionModels::CollisionModel<T>>	m_object;			// オブジェクト

public:
	DirectX::SimpleMath::Matrix						m_objectMatrix;		// オブジェクトのワールド行列
	DirectX::SimpleMath::Vector3					m_objectPos;		// オブジェクトの座標
	DirectX::SimpleMath::Vector3					m_objectSize = DirectX::SimpleMath::Vector3(1);		// オブジェクトの大きさ
	DirectX::SimpleMath::Vector3					m_objectVel;		// オブジェクトの速度
	DirectX::SimpleMath::Vector3					m_objectAcc;		// オブジェクトの加速度
	DirectX::SimpleMath::Vector4					m_objectColor;		// オブジェクトの色
	DirectX::SimpleMath::Quaternion					m_objectRot;		// オブジェクトの回転
	std::unique_ptr<T>								m_objectCollider;	// オブジェクトの当たり判定
	bool											m_objectWireframe;	// オブジェクトがワイヤーフレームか
	float											m_objectWeight;		// オブジェクトの重さ
	physx::PxRigidBody*								m_objectRigidbody = nullptr;	// リジッドボディー

public:
	~CollisionObject()
	{
	}

	// 生成
	void CollisionObject::Initialize(GameContext & context)
	{
		// オブジェクト作成
		m_object = std::make_unique<CollisionModels::CollisionModel<T>>(context.GetDR().GetD3DDeviceContext());
	}

	// 更新
	void CollisionObject::Update(GameContext & context)
	{
		int num = m_objectRigidbody->getNbShapes();
		std::vector<physx::PxShape*> shapes(num);
		m_objectRigidbody->getShapes(&shapes[0], num);

		auto& shape = shapes[0];
		auto transform = physx::PxShapeExt::getGlobalPose(*shape, *m_objectRigidbody);
		m_objectPos = DirectX::SimpleMath::Vector3(transform.p.x, transform.p.y, transform.p.z);
		m_objectRot = DirectX::SimpleMath::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);

		//// オブジェクトの物理
		//m_objectVel += m_objectAcc;
		//m_objectVel *= .98f;
		//m_objectPos += m_objectVel;
		// オブジェクトの更新
		m_objectMatrix = Matrix::CreateRotationZ(DirectX::XMConvertToRadians(90)) * Matrix::CreateScale(m_objectSize) * Matrix::CreateFromQuaternion(m_objectRot) * Matrix::CreateTranslation(m_objectPos);
		// オブジェクトの当たり判定
		m_objectCollider = std::make_unique<T>(CollisionGenerators::GetCollision<T>(*this));
	}

	// 描画
	void CollisionObject::Render(GameContext & context)
	{
		// オブジェクトの描画
		m_object->Draw(m_objectMatrix, context.GetCamera().view, context.GetCamera().projection, m_objectColor, false, m_objectWireframe);
	}

	// 破棄
	void CollisionObject::Finalize(GameContext & context)
	{
	}
};
