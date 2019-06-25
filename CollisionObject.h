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
	std::unique_ptr<CollisionModels::CollisionModel<T>>	m_object;			// オブジェクト

public:
	DirectX::SimpleMath::Matrix						m_objectMatrix;		// オブジェクトのワールド行列
	DirectX::SimpleMath::Vector3					m_objectPos;		// オブジェクトの座標
	DirectX::SimpleMath::Vector3					m_objectSize = DirectX::SimpleMath::Vector3(1);		// オブジェクトの大きさ
	DirectX::SimpleMath::Vector3					m_objectVel;		// オブジェクトの速度
	DirectX::SimpleMath::Vector3					m_objectAcc;		// オブジェクトの加速度
	DirectX::SimpleMath::Vector4					m_objectColor;		// オブジェクトの色
	std::unique_ptr<T>								m_objectCollider;	// オブジェクトの当たり判定
	bool											m_objectWireframe;	// オブジェクトがワイヤーフレームか
	float											m_objectWeight;		// オブジェクトの重さ

public:
	// 生成
	void CollisionObject::Initialize(GameContext & context)
	{
		// オブジェクト作成
		m_object = std::make_unique<CollisionModels::CollisionModel<T>>(context.GetDR().GetD3DDeviceContext());
	}

	// 更新
	void CollisionObject::Update(GameContext & context)
	{
		// オブジェクトの物理
		m_objectVel += m_objectAcc;
		m_objectVel *= .98f;
		m_objectPos += m_objectVel;
		// オブジェクトの更新
		m_objectMatrix = Matrix::CreateScale(m_objectSize) * Matrix::CreateTranslation(m_objectPos);
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
