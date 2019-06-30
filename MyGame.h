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
	// �I�u�W�F�N�g�֘A
	std::unique_ptr<CollisionObject<Collisions::Capsule>> m_objectA;
	std::vector<std::unique_ptr<CollisionObject<Collisions::Capsule>>> m_objectB;

	// �t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_batch;

	// �f�o�b�O�J����
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	// �O���b�h��
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

	// ����
	void Initialize(GameContext& context);
	// �X�V
	void Update(GameContext& context);
	// �`��
	void Render(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};

