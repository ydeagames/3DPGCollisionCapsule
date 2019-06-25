#pragma once
#include "Collision.h"
#include "CapsulePrimitive.h"

class GameContext;

namespace CollisionModels
{
	template<typename T>
	class CollisionModel
	{
	public:
		CollisionModel(ID3D11DeviceContext1* context);

		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;
	};

		template<>
	class CollisionModel<Collisions::Sphere>
	{
		static std::unique_ptr<DirectX::GeometricPrimitive> s_geo;

	public:
		CollisionModel(ID3D11DeviceContext1* context)
		{
			if (!s_geo)
				s_geo = DirectX::GeometricPrimitive::CreateSphere(context);
		}

		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const
		{
			s_geo->Draw(world, view, projection, color, texture, wireframe, setCustomState);
		}
	};

	template<>
	class CollisionModel<Collisions::Box>
	{
		static std::unique_ptr<DirectX::GeometricPrimitive> s_geo;

	public:
		CollisionModel(ID3D11DeviceContext1* context)
		{
			if (!s_geo)
				s_geo = DirectX::GeometricPrimitive::CreateCube(context);
		}

		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const
		{
			s_geo->Draw(world, view, projection, color, texture, wireframe, setCustomState);
		}
	};


	template<>
	class CollisionModel<Collisions::Capsule>
	{
		static std::unique_ptr<CapsulePrimitive> s_geo;

	public:
		CollisionModel(ID3D11DeviceContext1* context)
		{
			if (!s_geo)
				s_geo = CapsulePrimitive::Create(context);
		}

		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const
		{
			s_geo->Draw(world, view, projection, color, texture, wireframe, setCustomState);
		}
	};
}

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
	std::unique_ptr<T>								m_objectCollider;	// �I�u�W�F�N�g�̓����蔻��
	bool											m_objectWireframe;	// �I�u�W�F�N�g�����C���[�t���[����
	float											m_objectWeight;		// �I�u�W�F�N�g�̏d��

public:
	// ����
	void CollisionObject::Initialize(GameContext & context)
	{
		// �I�u�W�F�N�g�쐬
		m_object = std::make_unique<CollisionModels::CollisionModel<T>>(context.GetDR().GetD3DDeviceContext());
	}

	// �X�V
	void CollisionObject::Update(GameContext & context)
	{
		// �I�u�W�F�N�g�̕���
		m_objectVel += m_objectAcc;
		m_objectVel *= .98f;
		m_objectPos += m_objectVel;
		// �I�u�W�F�N�g�̍X�V
		m_objectMatrix = Matrix::CreateScale(m_objectSize) * Matrix::CreateTranslation(m_objectPos);
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
