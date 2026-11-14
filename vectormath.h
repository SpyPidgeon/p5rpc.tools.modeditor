#pragma once
#include "ImReflect.hpp"

struct Vector2
{
	Vector2() : x(0),y(0) {}
	Vector2(const Vector2& other) : x(other.x), y(other.y) {}
	Vector2(const float x, const float y) : x(x), y(y) {}

	float x, y;

	Vector2 Normal()
	{
		Vector2 copy = Vector2(x, y);

		float magnitude = sqrtf(x * x + y * y);

		if (magnitude > 0)
		{
			copy.x /= magnitude;
			copy.y /= magnitude;
		}

		return copy;
	}
};

struct Vector3
{
	Vector3() : x(0),y(0),z(0) {}
	Vector3(float x,  float y, float z) : x(x),y(y),z(z) {}
	~Vector3() {}
	float x, y, z;

	Vector3 Normal()
	{
		Vector3 copy = Vector3(x,y,z);

		float magnitude = sqrtf( x * x + y * y + z * z);

		if (magnitude > 0)
		{
			copy.x /= magnitude;
			copy.y /= magnitude;
			copy.z /= magnitude;
		}

		return copy;
	}

	void operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; }
	void operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; }

	Vector3 operator*(const float m) const { return { x * m, y * m, z * m }; }
	Vector3 operator*(const Vector3& m) const { return {x * m.x, y * m.y, z * m.z}; }

	Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }

	Vector3 operator-() const { return { -x,-y,-z }; }

	float DotProduct(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
	Vector3 CrossProduct(const Vector3& other) const { return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
	}
};
IMGUI_REFLECT(Vector3, x, y, z);

template<>
struct std::formatter<Vector3> : std::formatter<std::string>
{
	auto format(const Vector3& vec, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "X: {:.6f}, Y: {:.6f}, Z: {:.6f}", vec.x, vec.y, vec.z);
	}
};

struct Quaternion
{
	float x, y, z, w;

	static Quaternion EulerToQuat(const float yaw, const float pitch, const float roll)
	{
		float cy = std::cosf(yaw * 0.5f);
		float sy = std::sinf(yaw * 0.5f);
		float cp = std::cosf(pitch * 0.5f);
		float sp = std::sinf(pitch * 0.5f);
		float cr = std::cosf(roll * 0.5f);
		float sr = std::sinf(roll * 0.5f);

		float x = cy * sp * cr + sy * cp * sr;
		float y = sy * cp * cr - cy * sp * sr;
		float z = cy * cp * sr - sy * sp * cr;
		float w = cy * cp * cr + sy * sp * sr;

		return { x, y, z, w };
	}

	bool operator==(const Quaternion& other) { return x == other.x && y == other.y && z == other.z && w == other.w; }
	bool operator!=(const Quaternion& other) { return  x != other.x && y != other.y && z != other.z && w != other.w; }

	Quaternion operator-() { return { -x,-y,-z,-w }; }

	float DotProduct(const Quaternion& other) { return x * other.x + y * other.y + z * other.z + w * other.w; }
};
IMGUI_REFLECT(Quaternion, x, y, z, w);

template<>
struct std::formatter<Quaternion> : std::formatter<std::string>
{
	auto format(const Quaternion& q, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "X: {:.6f}, Y: {:.6f}, Z: {:.6f}, W: {:.6f}", q.x, q.y, q.z, q.w);
	}
};

struct Euler
{
	float yaw, pitch, roll;

	static Vector3 RotateAxis(const Vector3& vec, const Vector3& axis, float radians)
	{
		float c = std::cosf(radians);
		float s = std::sinf(radians);
		float d = vec.DotProduct(axis);

		Vector3 term1 = vec * c;
		Vector3 axisCross = axis.CrossProduct(vec);
		Vector3 term2 = axisCross * s;
		Vector3 term3 = axis * (d * (1.0f - c));

		return term1 + term2 + term3;
	}

	static Euler QuatToYXZRight(const Quaternion& q)
	{
		float t0 = 2.0f * (q.w * q.x - q.y * q.z);
		t0 = std::clamp(t0, -1.0f, 1.0f);
		float xPitch = std::asinf(t0);

		float t1 = 2.0f * (q.w * q.y + q.z * q.x);
		float t2 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		float yYaw = std::atan2f(-t1, -t2);

		float t3 = 2.0f * (q.w * q.z + q.x * q.y);
		float t4 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		float zRoll = std::atan2f(t3, t4);

		constexpr float Deg = 180.0f / IM_PI;
		return { yYaw * Deg, xPitch * Deg, zRoll * Deg };
	}
};
IMGUI_REFLECT(Euler, yaw, pitch, roll);

template<>
struct std::formatter<Euler> : std::formatter<std::string>
{
	auto format(const Euler& a, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "Yaw: {:.6f}, Pitch: {:.6f}, Roll: {:.6f}", a.yaw, a.pitch, a.roll);
	}
};