#ifndef BASE_H
#define BASE_H

//#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <thread>
//#include <chrono>
//#include <mmsystem.h>
#include <map>
#include <cmath>
using namespace std;

//#pragma comment(lib, "winmm.lib")  // 显式链接 winmm.lib 库

// 向量和结构体定义
struct Vector2D
{
	float x, y;

    Vector2D(float x, float y) : x(x), y(y) {}
    Vector2D() : x(0), y(0) {}

	static float CalculateDistance(Vector2D& v1, Vector2D& v2)
	{
		return (float)sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
	}
};

struct Vector3D : public Vector2D
{
    float z;

    Vector3D(float x, float y, float z) : Vector2D(x, y), z(z) {}
    Vector3D() : Vector2D(), z(0) {}

	static float CalculateDistance(Vector3D& v1, Vector3D& v2)
	{
		return (float)sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
	}
	
	bool IsValid() const { return x != -1.0f && y != -1.0f && z != -1.0f; }
};

struct Vector4D : public Vector3D
{
    float w;

    Vector4D(float x, float y, float z, float w) : Vector3D(x, y, z), w(w) {}
    Vector4D() : Vector3D(), w(0) {}

	static float CalculateDistance(Vector4D& v1, Vector4D& v2)
	{
		return (float)sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2) + pow(v1.w - v2.w, 2));
	}
};

struct QAngle_t
{
    float x, y, z;
    
    QAngle_t() : x(0), y(0), z(0) {}
    QAngle_t(float x, float y, float z = 0) : x(x), y(y), z(z) {}
    
    bool IsValid() const { return x != 0.0f || y != 0.0f || z != 0.0f; }
    
    void Normalize()
    {
        while (x > 180.0f) x -= 360.0f;
        while (x < -180.0f) x += 360.0f;
        while (y > 180.0f) y -= 360.0f;
        while (y < -180.0f) y += 360.0f;
        
        if (x > 89.0f) x = 89.0f;
        if (x < -89.0f) x = -89.0f;
        
        z = 0.0f;
    }
};

struct Box
{
    float x, y, width, height;
};

struct FNameEntry
{
    uint16_t bIsWide : 1;
    uint16_t LowercaseProbeHash : 5;
    uint16_t Len : 10;

    union
    {
        char AnsiName[1024];
        wchar_t WideName[1024];
    };

    static string GetName(uint8_t** GName, uint32_t Id)
    {
        uint32_t Block = Id >> 16; // 取高16位得到块
        uint32_t offset = Id & 0xFFFF; // 取低16位得到偏移
        FNameEntry* Info = (FNameEntry*)(GName[2 + Block] + 2 * offset); // 得到FNameEntry结构体的地址
        return string(Info->AnsiName, Info->Len); // 得到字符串
    }
};

#endif // BASE_H
