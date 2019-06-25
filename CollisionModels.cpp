#include "pch.h"
#include "CollisionModels.h"

namespace CollisionModels
{
	namespace
	{
		std::unique_ptr<DirectX::GeometricPrimitive> s_geoSphere;
		std::unique_ptr<DirectX::GeometricPrimitive> s_geoBox;
		std::unique_ptr<CapsulePrimitive> s_geoCapsule;
	}

	CollisionModel<Collisions::Sphere>::CollisionModel(ID3D11DeviceContext1* context)
	{
		if (!s_geoSphere)
			s_geoSphere = DirectX::GeometricPrimitive::CreateSphere(context);
	}

	void CollisionModel<Collisions::Sphere>::Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color, _In_opt_ ID3D11ShaderResourceView* texture, bool wireframe, _In_opt_ std::function<void __cdecl()> setCustomState) const
	{
		s_geoSphere->Draw(world, view, projection, color, texture, wireframe, setCustomState);
	}

	CollisionModel<Collisions::Box>::CollisionModel(ID3D11DeviceContext1* context)
	{
		if (!s_geoBox)
			s_geoBox = DirectX::GeometricPrimitive::CreateCube(context);
	}

	void CollisionModel<Collisions::Box>::Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color, _In_opt_ ID3D11ShaderResourceView* texture, bool wireframe, _In_opt_ std::function<void __cdecl()> setCustomState) const
	{
		s_geoBox->Draw(world, view, projection, color, texture, wireframe, setCustomState);
	}

	CollisionModel<Collisions::Capsule>::CollisionModel(ID3D11DeviceContext1* context)
	{
		if (!s_geoCapsule)
			s_geoCapsule = CapsulePrimitive::Create(context);
	}

	void CollisionModel<Collisions::Capsule>::Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color, _In_opt_ ID3D11ShaderResourceView* texture, bool wireframe, _In_opt_ std::function<void __cdecl()> setCustomState) const
	{
		s_geoCapsule->Draw(world, view, projection, color, texture, wireframe, setCustomState);
	}
}