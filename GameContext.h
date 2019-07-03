#pragma once
#include "StepTimer.h"
#include "DeviceResources.h"

class GameObject;
class GameCamera;

// GameContextインターフェイス
class GameContext
{
public:
	GameContext() = default;
	virtual ~GameContext() = default;

public:
	// DeviceResource取得
	virtual DX::DeviceResources& GetDR() const = 0;
	// タイマー取得
	virtual const DX::StepTimer& GetTimer() const = 0;
	// カメラ取得
	virtual GameCamera& GetCamera() = 0;
	// コモンステート取得
	virtual DirectX::CommonStates& GetStates() const = 0;
	// ラスタライザーステート取得
	virtual ID3D11RasterizerState* GetRasterizerState() const = 0;
};
