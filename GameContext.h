#pragma once
#include "StepTimer.h"
#include "DeviceResources.h"

class GameObject;
class GameCamera;

// GameContext�C���^�[�t�F�C�X
class GameContext
{
public:
	GameContext() = default;
	virtual ~GameContext() = default;

public:
	// DeviceResource�擾
	virtual DX::DeviceResources& GetDR() const = 0;
	// �^�C�}�[�擾
	virtual const DX::StepTimer& GetTimer() const = 0;
	// �J�����擾
	virtual GameCamera& GetCamera() = 0;
	// �R�����X�e�[�g�擾
	virtual DirectX::CommonStates& GetStates() const = 0;
	// ���X�^���C�U�[�X�e�[�g�擾
	virtual ID3D11RasterizerState* GetRasterizerState() const = 0;
};
