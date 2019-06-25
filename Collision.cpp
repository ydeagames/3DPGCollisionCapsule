#include "pch.h"
#include "Collision.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace Collisions
{
	namespace
	{
		float SqDistPointSegment(Vector3 a, Vector3 b, Vector3 c)
		{
			Vector3 ab = b - a;
			Vector3 ac = c - a;
			Vector3 bc = c - b;
			float e = ac.Dot(ab);
			if (e <= 0.0f)
				return ac.LengthSquared();
			float f = ab.LengthSquared();
			if (e >= f)
				return bc.LengthSquared();
			return ac.LengthSquared() - e * e / f;
		}
	}
	template<>
	bool IsHit(const Box& obj1, const Box& obj2)
	{
		return obj1.Intersects(obj2);
	}
	template<>
	bool IsHit(const Sphere& obj1, const Sphere& obj2)
	{
		//return obj1.Intersects(obj2);
		auto& p1 = obj1.Center;
		auto& p2 = obj2.Center;
		auto& h1 = obj1.Radius;
		auto& h2 = obj2.Radius;
		return Vector3::DistanceSquared(p1, p2) < std::pow(h1 + h2, 2);
	}
	//template<>
	//bool IsHit(const Capsule& obj1, const Capsule& obj2)
	//{
	//}
	//template<>
	//bool IsHit(const Box& obj1, const Sphere& obj2)
	//{
	//}
	template<>
	bool IsHit(const Sphere& obj1, const Capsule& obj2)
	{
		float dist2 = SqDistPointSegment(obj2.CenterA, obj2.CenterB, obj1.Center);
		float radius = obj1.Radius + obj2.Radius;
		return dist2 <= radius * radius;
	}
	//template<>
	//bool IsHit(const Capsule& obj1, const Box& obj2)
	//{
	//}
}
