//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "CommonStates.h"
#include "SimpleMath.h"
#include "Mouse.h"
#include "Keyboard.h"

#include "GeometricPrimitive.h"
#include "CapsulePrimitive.h"

class DebugCamera;
class GridFloor;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

	Game() noexcept(false);
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize(int& width, int& height) const;

private:

	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;

	// 射影行列
	DirectX::SimpleMath::Matrix				m_projection;

	// マウス
	std::unique_ptr<DirectX::Mouse>			m_pMouse;

	// デバッグカメラ
	DebugCamera*							m_pDebugCamera;
	// グリッド床
	GridFloor*								m_pGridFloor;
	// コモンステート
	std::unique_ptr<DirectX::CommonStates>	m_pState;

	// オブジェクト関連
	std::unique_ptr<DirectX::GeometricPrimitive>	m_pObject1;			// オブジェクト１
	std::unique_ptr<CapsulePrimitive>				m_pObject2;			// オブジェクト２

	DirectX::SimpleMath::Matrix						m_object1Matrix;	// オブジェクト１のワールド行列
	DirectX::SimpleMath::Matrix						m_object2Matrix;	// オブジェクト２のワールド行列

	DirectX::SimpleMath::Vector3					m_object1Pos;		// オブジェクト１の座標
	DirectX::SimpleMath::Vector3					m_object2Pos;		// オブジェクト２の座標
};