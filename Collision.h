#pragma once

namespace Collisions
{
	struct BoundingCapsule
	{
		DirectX::SimpleMath::Vector3 CenterA;
		DirectX::SimpleMath::Vector3 CenterB;
		float Radius;

		// Creators
		BoundingCapsule()
			: CenterA(0, 0, 0)
			, CenterB(0, 0, 0)
			, Radius(1.f) {}
		constexpr BoundingCapsule(const DirectX::SimpleMath::Vector3& centerA, const DirectX::SimpleMath::Vector3& centerB, float radius)
			: CenterA(centerA)
			, CenterB(centerB)
			, Radius(radius) {}
	};
	using Box = DirectX::BoundingBox;
	using Sphere = DirectX::BoundingSphere;
	using Capsule = BoundingCapsule;

	template<typename A, typename B>
	bool IsHit(const A& obj1, const B& obj2)
	{
		return IsHit<B, A>(obj2, obj1);
	}
	template<>
	bool IsHit(const Box& obj1, const Box& obj2);
	template<>
	bool IsHit(const Sphere& obj1, const Sphere& obj2);
	template<>
	bool IsHit(const Sphere& obj1, const Capsule& obj2);
}
