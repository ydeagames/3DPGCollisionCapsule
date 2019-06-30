#pragma once
#include "CollisionObject.h"

class GameContext;
class DebugCamera;
class InfinityGridFloor;

class MyGame
{
public:
	class PxDefaultErrorCallback : public physx::PxErrorCallback
	{
	public:
		PxDefaultErrorCallback() = default;
		~PxDefaultErrorCallback() = default;

		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
		{
			
		}
	};


private:
	// オブジェクト関連
	std::unique_ptr<CollisionObject<Collisions::Capsule>> m_objectA;
	std::vector<std::unique_ptr<CollisionObject<Collisions::Capsule>>> m_objectB;

	// フォント
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_batch;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	// グリッド床
	std::unique_ptr<InfinityGridFloor> m_pGridFloor;

	physx::PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	physx::PxFoundation* gFoundation = NULL;
	physx::PxPhysics* gPhysics = NULL;

	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxScene* gScene = NULL;

	physx::PxMaterial* gMaterial = NULL;

	physx::PxPvd* gPvd = NULL;

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

