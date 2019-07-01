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
	std::unique_ptr<CollisionModels::CollisionModel<T>>	m_object;			// �I�u�W�F�N�g

public:
	DirectX::SimpleMath::Matrix						m_objectMatrix;		// �I�u�W�F�N�g�̃��[���h�s��
	DirectX::SimpleMath::Vector3					m_objectPos;		// �I�u�W�F�N�g�̍��W
	DirectX::SimpleMath::Vector3					m_objectSize = DirectX::SimpleMath::Vector3(1);		// �I�u�W�F�N�g�̑傫��
	DirectX::SimpleMath::Vector3					m_objectVel;		// �I�u�W�F�N�g�̑��x
	DirectX::SimpleMath::Vector3					m_objectAcc;		// �I�u�W�F�N�g�̉����x
	DirectX::SimpleMath::Vector4					m_objectColor;		// �I�u�W�F�N�g�̐F
	DirectX::SimpleMath::Quaternion					m_objectRot;		// �I�u�W�F�N�g�̉�]
	std::unique_ptr<T>								m_objectCollider;	// �I�u�W�F�N�g�̓����蔻��
	bool											m_objectWireframe;	// �I�u�W�F�N�g�����C���[�t���[����
	float											m_objectWeight;		// �I�u�W�F�N�g�̏d��
	physx::PxRigidBody*								m_objectRigidbody = nullptr;	// ���W�b�h�{�f�B�[

public:
	~CollisionObject()
	{
	}

	// ����
	void CollisionObject::Initialize(GameContext & context)
	{
		// �I�u�W�F�N�g�쐬
		m_object = std::make_unique<CollisionModels::CollisionModel<T>>(context.GetDR().GetD3DDeviceContext());
	}

	// �X�V
	void CollisionObject::Update(GameContext & context)
	{
		int num = m_objectRigidbody->getNbShapes();
		std::vector<physx::PxShape*> shapes(num);
		m_objectRigidbody->getShapes(&shapes[0], num);

		auto& shape = shapes[0];
		auto transform = physx::PxShapeExt::getGlobalPose(*shape, *m_objectRigidbody);
		m_objectPos = DirectX::SimpleMath::Vector3(transform.p.x, transform.p.y, transform.p.z);
		m_objectRot = DirectX::SimpleMath::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);

		//// �I�u�W�F�N�g�̕���
		//m_objectVel += m_objectAcc;
		//m_objectVel *= .98f;
		//m_objectPos += m_objectVel;
		// �I�u�W�F�N�g�̍X�V
		m_objectMatrix = Matrix::CreateRotationZ(DirectX::XMConvertToRadians(90)) * Matrix::CreateScale(m_objectSize) * Matrix::CreateFromQuaternion(m_objectRot) * Matrix::CreateTranslation(m_objectPos);
		// �I�u�W�F�N�g�̓����蔻��
		m_objectCollider = std::make_unique<T>(CollisionGenerators::GetCollision<T>(*this));
	}

	// �`��
	void CollisionObject::Render(GameContext & context)
	{
		// �I�u�W�F�N�g�̕`��
		m_object->Draw(m_objectMatrix, context.GetCamera().view, context.GetCamera().projection, m_objectColor, false, m_objectWireframe);
	}

	// �j��
	void CollisionObject::Finalize(GameContext & context)
	{
	}
};
