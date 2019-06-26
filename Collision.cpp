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

		float Clamp(float n, float min, float max)
		{
			if (n < min) return min;
			if (n > max) return max;
			return n;
		}

		Vector3 ClosestPtSegmentSegmentNorm(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2, float &s, float &t, Vector3& c1, Vector3& c2)
		{
			Vector3 d1 = q1 - p1; Vector3 d2 = q2 - p2; Vector3 r = p1 - p2;
			float a = d1.LengthSquared(); float e = d2.LengthSquared(); float f = d2.Dot(r);
			if (a <= FLT_EPSILON && e <= FLT_EPSILON)
			{
				s = t = 0.0f;
				c1 = p1;
				c2 = p2;
				return (c1 - c2);
			}
			if (a <= FLT_EPSILON)
			{
				s = 0.0f;
				t = f / e;
				t = Clamp(t, 0.0f, 1.0f);
			}
			else
			{
				float c = d1.Dot(r);
				if (e <= FLT_EPSILON)
				{
					t = 0.0f;
					s = Clamp(-c / a, 0.0f, 1.0f);
				}
				else
				{
					float b = d1.Dot(d2);
					float denom = a * e - b * b;
					if (denom != 0.0f)
					{
						s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
					}
					else
					{
						s = 0.0f;
					}
					float tnom = (b * s + f);
					if (tnom < 0.0f)
					{
						t = 0.0f;
						s = Clamp(-c / a, 0.0f, 1.0f);
					}
					else if (tnom > e)
					{
						t = 1.0f;
						s = Clamp((b - c) / a, 0.0f, 1.0f);
					}
					else
					{
						t = tnom / e;
					}
				}
			}
			c1 = p1 + d1 * s;
			c2 = p2 + d2 * t;
			return (c1 - c2);
		}

		float ClosestPtSegmentSegment(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2, float &s, float &t, Vector3& c1, Vector3& c2)
		{
			return ClosestPtSegmentSegmentNorm(p1, q1, p2, q2, s, t, c1, c2).LengthSquared();
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
	template<>
	bool IsHit(const Capsule& obj1, const Capsule& obj2)
	{
		float s, t;
		DirectX::SimpleMath::Vector3 c1, c2;
		float dist2 = ClosestPtSegmentSegment(obj1.CenterA, obj1.CenterB, obj2.CenterA, obj2.CenterB, s, t, c1, c2);
		float radius = obj1.Radius + obj2.Radius;
		return dist2 <= radius * radius;
	}

	template<>
	DirectX::SimpleMath::Vector3 GetHitNormal(const Capsule& obj1, const Capsule& obj2)
	{
		float s, t;
		DirectX::SimpleMath::Vector3 c1, c2;
		return ClosestPtSegmentSegmentNorm(obj1.CenterA, obj1.CenterB, obj2.CenterA, obj2.CenterB, s, t, c1, c2);
	}
}
