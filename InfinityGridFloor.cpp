﻿#include "pch.h"

#include "InfinityGridFloor.h"
#include "GameContext.h"
#include "GameCamera.h"

#include <algorithm>

namespace
{
	float GetClamp(float x, float min, float max)
	{
		return std::min(std::max(x, min), max);
	}

	float GetNearest(float x, float target, float a, float b)
	{
		return std::abs(a - target) < std::abs(b - target) ? a : b;
	}
}

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

	auto campos = ctx.GetCamera().GetPosition();
	auto ray1 = ctx.GetCamera().ViewportPointToRay(DirectX::SimpleMath::Vector3(-1, -1, 0));
	auto ray2 = ctx.GetCamera().ViewportPointToRay(DirectX::SimpleMath::Vector3(1, 1, 0));
	DirectX::SimpleMath::Plane plane = DirectX::SimpleMath::Plane(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::Up);

	DirectX::SimpleMath::Vector3 q1 = campos + DirectX::SimpleMath::Vector3(1, 0, 1) * 50;
	DirectX::SimpleMath::Vector3 q2 = campos + DirectX::SimpleMath::Vector3(1, 0, 1) * -50;

	m_primitiveBatch->Begin();

	float ax1 = std::min(q1.x, q2.x);
	float ax2 = std::max(q1.x, q2.x);
	float az1 = std::min(q1.z, q2.z);
	float az2 = std::max(q1.z, q2.z);
	int tx1 = int(std::floor(ax1 / m_cellsize));
	int tx2 = int(std::floor(ax2 / m_cellsize));
	int tz1 = int(std::floor(az1 / m_cellsize));
	int tz2 = int(std::floor(az2 / m_cellsize));
	for (int i = tx1; i <= tx2; ++i)
	{
		DirectX::VertexPositionColor v1(DirectX::SimpleMath::Vector3(i * m_cellsize, 0, az1), color);
		DirectX::VertexPositionColor v2(DirectX::SimpleMath::Vector3(i * m_cellsize, 0, az2), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}
	for (int i = tz1; i <= tz2; ++i)
	{
		DirectX::VertexPositionColor v1(DirectX::SimpleMath::Vector3(ax1, 0, i * m_cellsize), color);
		DirectX::VertexPositionColor v2(DirectX::SimpleMath::Vector3(ax2, 0, i * m_cellsize), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	m_primitiveBatch->End();
}

