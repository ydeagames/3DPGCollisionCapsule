#pragma once

#include "SimpleMath.h"
#include "GeometricPrimitive.h"

class CapsulePrimitive
{
public:
	static std::unique_ptr<CapsulePrimitive> __cdecl Create(_In_ ID3D11DeviceContext1* context, float segmentLength = 1.0f, float diameter = 1.0f);
	
	~CapsulePrimitive();

	void Draw(DirectX::SimpleMath::Matrix world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color = DirectX::Colors::White, _In_opt_ ID3D11ShaderResourceView* texture = nullptr, bool wireframe = false,
		_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;

private:
	CapsulePrimitive(ID3D11DeviceContext1* context, float segmentLength, float diameter) noexcept(false);

private:
	DirectX::SimpleMath::Matrix						m_worldMtx;		// ƒ[ƒ‹ƒhÀ•W

	std::unique_ptr<DirectX::GeometricPrimitive>	m_pCylinder;	// “›•”•ª
	std::unique_ptr<DirectX::GeometricPrimitive>	m_pSphere1;		// ‹…•”•ª‚P
	std::unique_ptr<DirectX::GeometricPrimitive>	m_pSphere2;		// ‹…•”•ª‚Q

	float											m_segmentLength;
	float											m_diameter;
};

