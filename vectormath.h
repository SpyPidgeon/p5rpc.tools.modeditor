#pragma once
#include "ImReflect.hpp"

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
};
IMGUI_REFLECT(Vector3, x, y, z);