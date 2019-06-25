#pragma once
#include "CapsulePrimitive.h"
#include "Collision.h"

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
	public:
		CollisionModel(ID3D11DeviceContext1* context);
		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;
	};
	template<>
	class CollisionModel<Collisions::Box>
	{
	public:
		CollisionModel(ID3D11DeviceContext1* context);
		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;
	};
	template<>
	class CollisionModel<Collisions::Capsule>
	{
	public:
		CollisionModel(ID3D11DeviceContext1* context);
		void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;
	};
}
