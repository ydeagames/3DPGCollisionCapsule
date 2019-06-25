#include "pch.h"
#include "CollisionObject.h"
#include "GameCamera.h"
#include "GameContext.h"
#include "CapsulePrimitive.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace CollisionGenerators
{
	template<>
	Collisions::Box GetCollision(CollisionObject<Collisions::Box>& obj)
	{
		return Collisions::Box(obj.m_objectPos, obj.m_objectSize * .5f);
	}
	template<>
	Collisions::Sphere GetCollision(CollisionObject<Collisions::Sphere>& obj)
	{
		return Collisions::Sphere(obj.m_objectPos, std::max({ obj.m_objectSize.x, obj.m_objectSize.y, obj.m_objectSize.z }) *.5f);
	}
	template<>
	Collisions::Capsule GetCollision(CollisionObject<Collisions::Capsule>& obj)
	{
		float radius = std::max({ obj.m_objectSize.x, obj.m_objectSize.y, obj.m_objectSize.z }) *.5f;
		return Collisions::Capsule(
			obj.m_objectPos + Vector3::Transform(Vector3::Up, obj.m_objectRot) * (obj.m_objectSize.y - radius),
			obj.m_objectPos + Vector3::Transform(Vector3::Down, obj.m_objectRot) * (obj.m_objectSize.y - radius),
			radius);
	}
}
