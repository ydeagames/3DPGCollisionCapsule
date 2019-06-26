#pragma once
#include "CollisionObject.h"

class GameContext;
class DebugCamera;
class GridFloor;

class MyGame
{
private:
	// �I�u�W�F�N�g�֘A
	std::unique_ptr<CollisionObject<Collisions::Capsule>> m_objectA;
	std::vector<std::unique_ptr<CollisionObject<Collisions::Capsule>>> m_objectB;

	// �t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_batch;

	// �f�o�b�O�J����
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	// �O���b�h��
	std::unique_ptr<GridFloor> m_pGridFloor;

public:
	MyGame();
	~MyGame();

	// ����
	void Initialize(GameContext& context);
	// �X�V
	void Update(GameContext& context);
	// �`��
	void Render(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};

