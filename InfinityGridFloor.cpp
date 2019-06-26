#include "pch.h"

#include "InfinityGridFloor.h"
#include "GameContext.h"
#include "GameCamera.h"

#include <algorithm>

// コンストラクタ
InfinityGridFloor::InfinityGridFloor(GameContext& ctx, float cellsize) : m_cellsize(cellsize)
{
	auto device = ctx.GetDR().GetD3DDevice();
	auto context = ctx.GetDR().GetD3DDeviceContext();

	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	// 頂点カラー(有効)
	m_basicEffect->SetVertexColorEnabled(true);
	// プリミティブオブジェクト生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	// インプットレイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_pInputLayout.GetAddressOf());
}

// デストラクタ
InfinityGridFloor::~InfinityGridFloor()
{
	// エフェクトを解放する
	m_basicEffect.reset();
	// 入力レイアウトを解放する
	m_pInputLayout.Reset();
}

// 描画
void InfinityGridFloor::draw(GameContext& ctx, DirectX::GXMVECTOR color)
{
	auto device = ctx.GetDR().GetD3DDevice();
	auto context = ctx.GetDR().GetD3DDeviceContext();
	auto& states = ctx.GetStates();

	DirectX::SimpleMath::Matrix world;

	context->OMSetBlendState(states.Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states.DepthDefault(), 0);

	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(ctx.GetCamera().view);
	m_basicEffect->SetProjection(ctx.GetCamera().projection);


	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_pInputLayout.Get());

	auto ray1 = ctx.GetCamera().ViewportPointToRay(DirectX::SimpleMath::Vector3(-1, -1, 0));
	auto ray2 = ctx.GetCamera().ViewportPointToRay(DirectX::SimpleMath::Vector3(1, 1, 0));
	DirectX::SimpleMath::Plane plane = DirectX::SimpleMath::Plane(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::Up);
	float dist1, dist2;
	ray1.Intersects(plane, dist1);
	ray2.Intersects(plane, dist2);

	m_primitiveBatch->Begin();

	const DirectX::XMVECTORF32 xAxis = { 10, 0.f, 0.f };
	const DirectX::XMVECTORF32 yAxis = { 0.f, 0.f, 10 };

	size_t divs = std::max<size_t>(1, 10);
	DirectX::FXMVECTOR origin = DirectX::g_XMZero;
	for (size_t i = 0; i <= divs; ++i)
	{
		float fPercent = float(i) / float(divs);
		fPercent = (fPercent * 1.0f) - 0.5f;
		DirectX::XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
		vScale = DirectX::XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(vScale, yAxis * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(vScale, yAxis * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divs; i++)
	{
		FLOAT fPercent = float(i) / float(divs);
		fPercent = (fPercent * 1.0f) - 0.5f;
		DirectX::XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
		vScale = DirectX::XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(vScale, xAxis * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(vScale, xAxis * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	m_primitiveBatch->End();
}

