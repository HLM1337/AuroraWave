#ifndef MEMORY_LC_H
#define MEMORY_LC_H

#include "Base.h"

const double PI = 3.14159265358979323846;

// 获取内存相关
DWORD64 ReadLong(const DWORD64 address);
short ReadShort(const DWORD64 address);
uint32_t ReadUint(const DWORD64 address);
int ReadInt(const DWORD64 address);
float ReadFloat(const DWORD64 address);
double ReadDouble(const DWORD64 address);
BYTE ReadByte(const DWORD64 address);
vector<BYTE> ReadBytes(const DWORD64 address, SIZE_T length);
string ReadString(const long long address, SIZE_T maxLength);
void ReadMatrixArray(const DWORD64 matrixHead, float* Matrix_Array);
template<typename T>
T ReadMemory(uintptr_t address);

// 写入内存相关
bool WriteLong(const DWORD64 address, const DWORD64 value);
bool WriteUint(const DWORD64 address, const uint32_t value);
bool WriteInt(const DWORD64 address, const int value);
bool WriteFloat(const DWORD64 address, const float value);
bool WriteDouble(const DWORD64 address, const double value);
bool WriteByte(const DWORD64 address, const BYTE value);
bool WriteBytes(const DWORD64 address, const vector<BYTE>& bytes);
template<typename T>
bool WriteMemory(uintptr_t address, T value);

// 坐标转换实现函数
Vector2D WorldToScreen_UE(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH);
Vector2D WorldToScreen_Source(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH);
Box WorldToScreen_Source_Box(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH, float size_1 = -8.0f, float size_2 = 78.0f);
Vector2D AutoAiming(Vector3D myself, Vector3D enemy);
void Coordinate_Telesport(DWORD64 my_location, Vector3D Translation_My, float Yaw, float speed = 10.0f, int Function_Key = -1, int Front_Key = 0x26, int Rear_Key = 0x28, int Left_Key = 0x25, int Right_Key = 0x27);

// 辅助功能
string IntToHex(int value);
bool IsKeyPressed(int key);
bool ForceTerminateProcess(DWORD pid);
DWORD GetProcessIdByName(const wchar_t* processName);
uintptr_t AllocateMemory(size_t size, DWORD protection);

#endif // MEMORY_LC_H
