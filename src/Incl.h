#include <iostream>
#include <thread>
#include <string>
#include <windows.h>
#include <winnt.h>
#include <Lmcons.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <Windows.h>
#include <sstream>
#include <d3d10.h>
#include <d3d11.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

ImGuiWindow& BeginScene();
VOID EndScene(ImGuiWindow& window);

template<typename T>
T ReadMemory(DWORD_PTR address, const T& def = T())
{
	return *(T*)address;
}

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{
	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{
	}
	~Vector3()
	{
	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}
};