#pragma once
#include "CollisionObject.h"

class GameContext;

class MyGame
{
private:
	// �I�u�W�F�N�g�֘A
	std::unique_ptr<CollisionObject<Collisions::Capsule>> m_objectA;
	std::vector<std::unique_ptr<CollisionObject<Collisions::Sphere>>> m_objectB;

	// �t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_batch;

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

