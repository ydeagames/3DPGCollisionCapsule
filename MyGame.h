#pragma once
#include "CollisionObject.h"

class GameContext;

class MyGame
{
private:
	// オブジェクト関連
	std::unique_ptr<CollisionObject<Collisions::Sphere>> m_objectA;
	std::vector<std::unique_ptr<CollisionObject<Collisions::Capsule>>> m_objectB;

	// フォント
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_batch;

public:
	MyGame();
	~MyGame();

	// 生成
	void Initialize(GameContext& context);
	// 更新
	void Update(GameContext& context);
	// 描画
	void Render(GameContext& context);
	// 破棄
	void Finalize(GameContext& context);
};

