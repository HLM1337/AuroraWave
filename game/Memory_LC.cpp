#include "Base.h"
#include "Memory_LC.h"

DWORD64 ReadLong(const DWORD64 address)
{
    DWORD64 value = 0;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(DWORD64*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = 0;
    }
    return value;
}
short ReadShort(const DWORD64 address)
{
    short value = -1;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(short*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1;
    }
    return value;
}
uint32_t ReadUint(const DWORD64 address)
{
    uint32_t value = 0;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(uint32_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = 0;
    }
    return value;
}
int ReadInt(const DWORD64 address)
{
    int value = -1;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(int*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1;
    }
    return value;
}
float ReadFloat(const DWORD64 address)
{
    float value = -1.0f;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(float*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1.0f;
    }
    return value;
}
double ReadDouble(const DWORD64 address)
{
    double value = -1.0;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(double*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1.0;
    }
    return value;
}
BYTE ReadByte(const DWORD64 address)
{
    BYTE value = 0;
    if (address < 0xFFFF)
        return value;
    __try
    {
        value = *(BYTE*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = 0;
    }
    return value;
}
vector<BYTE> ReadBytes(const DWORD64 address, SIZE_T length)
{
    vector<BYTE> buffer(length);
    if (address < 0xFFFF)
        return { };
    memcpy(buffer.data(), (void*)address, length);
    return buffer;
}
string ReadString(const long long address, SIZE_T maxLength)
{
    vector<BYTE> buffer(maxLength);
    if (address < 0xFFFF)
        return "";
    memcpy(buffer.data(), (void*)address, maxLength);
    size_t nullIndex = maxLength;
    size_t questionIndex = maxLength;
    for (size_t i = 0; i < maxLength; ++i) {
        if (buffer[i] == 0 && nullIndex == maxLength) {
            nullIndex = i;
        }
        if (buffer[i] == '?' && questionIndex == maxLength) {
            questionIndex = i;
        }
    }
    size_t effectiveLength = min(nullIndex, questionIndex);
    return string(buffer.begin(), buffer.begin() + effectiveLength);
}
void ReadMatrixArray(const DWORD64 matrixHead, float* Matrix_Array)
{
    for (int i = 0; i < 16; i++)
    {
        Matrix_Array[i] = ReadFloat(matrixHead + i * 0x4);
    }
}
bool WriteLong(const DWORD64 address, DWORD64 value)
{
    __try
    {
        *(long long*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
bool WriteUint(const DWORD64 address, const uint32_t value)
{
    __try
    {
        *(uint32_t*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
bool WriteInt(const DWORD64 address, int value)
{
    __try
    {
        *(int*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
bool WriteFloat(const DWORD64 address, float value)
{
    __try
    {
        *(float*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
bool WriteDouble(const DWORD64 address, double value)
{
    __try
    {
        *(double*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
bool WriteByte(const DWORD64 address, BYTE value)
{
    __try
    {
        *(BYTE*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
bool WriteBytes(const DWORD64 address, const std::vector<BYTE>& buffer) {
    DWORD old_protect;
    if (!VirtualProtect((LPVOID)address, buffer.size(), PAGE_EXECUTE_READWRITE, &old_protect)) return false;
    __try {
        memcpy((void*)address, buffer.data(), buffer.size());
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        VirtualProtect((LPVOID)address, buffer.size(), old_protect, &old_protect);
        return false;
    }
    VirtualProtect((LPVOID)address, buffer.size(), old_protect, &old_protect);
    return true;
}

template<typename T>
T ReadMemory(uintptr_t address)
{
    T value;
    __try
    {
        value = *(T*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = new T();
    }
    return value;
}

template<typename T>
bool WriteMemory(uintptr_t address, T value)
{
    __try
    {
        *(T*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

Vector2D WorldToScreen_UE(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH)
{
    Vector2D screenCoordinate = { -1.0, -1.0 };
    float cameraZ = MatrixArray[3] * (float)peopleCoordinate.x + MatrixArray[7] * (float)peopleCoordinate.y + MatrixArray[11] * (float)peopleCoordinate.z + MatrixArray[15];
    if (cameraZ > 0)
    {
        float cameraX = MatrixArray[0] * (float)peopleCoordinate.x + MatrixArray[4] * (float)peopleCoordinate.y + MatrixArray[8] * (float)peopleCoordinate.z + MatrixArray[12];
        float cameraY = MatrixArray[1] * (float)peopleCoordinate.x + MatrixArray[5] * (float)peopleCoordinate.y + MatrixArray[9] * (float)peopleCoordinate.z + MatrixArray[13];
        float RHW = 1.0f / cameraZ;
        float ndcX = cameraX * RHW;
        float ndcY = cameraY * RHW;
        screenCoordinate.x = (ndcX * 0.5f + 0.5f) * GameWindowsW;
        screenCoordinate.y = (1.0f - (ndcY * 0.5f + 0.5f)) * GameWindowsH;
    }
    return screenCoordinate;
}

Vector2D WorldToScreen_Source(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH)
{
    Vector2D screenCoordinate = { -1.0, -1.0 };
    float cameraZ = MatrixArray[8] * (float)peopleCoordinate.x + MatrixArray[9] * (float)peopleCoordinate.y + MatrixArray[10] * (float)peopleCoordinate.z + MatrixArray[11];
    if (cameraZ > 0)
    {
        float cameraX = MatrixArray[0] * (float)peopleCoordinate.x + MatrixArray[1] * (float)peopleCoordinate.y + MatrixArray[2] * (float)peopleCoordinate.z + MatrixArray[3];
        float cameraY = MatrixArray[4] * (float)peopleCoordinate.x + MatrixArray[5] * (float)peopleCoordinate.y + MatrixArray[6] * (float)peopleCoordinate.z + MatrixArray[7];
        float RHW = 1.0f / cameraZ;
        float ndcX = cameraX * RHW;
        float ndcY = cameraY * RHW;
        screenCoordinate.x = (ndcX * 0.5f + 0.5f) * GameWindowsW;
        screenCoordinate.y = (1.0f - (ndcY * 0.5f + 0.5f)) * GameWindowsH;
    }
    return screenCoordinate;
}

Box WorldToScreen_Source_Box(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH, float size_1, float size_2)
{
    Box screenBox = { -1.0, -1.0, -1.0, -1.0 };
    float cameraZ = MatrixArray[8] * (float)peopleCoordinate.x + MatrixArray[9] * (float)peopleCoordinate.y + MatrixArray[10] * (float)peopleCoordinate.z + MatrixArray[11];
    if (cameraZ > 0)
    {
        float cameraX = MatrixArray[0] * (float)peopleCoordinate.x + MatrixArray[1] * (float)peopleCoordinate.y + MatrixArray[2] * (float)peopleCoordinate.z + MatrixArray[3];
        float cameraY = MatrixArray[4] * (float)peopleCoordinate.x + MatrixArray[5] * (float)peopleCoordinate.y + MatrixArray[6] * (float)peopleCoordinate.z + MatrixArray[7];
        float RHW = 1.0f / cameraZ;
        float ndcX = cameraX * RHW;
        float ndcY = cameraY * RHW;
        Vector2D screenCoordinate = {
            (ndcX * 0.5f + 0.5f) * GameWindowsW,
            (1.0f - (ndcY * 0.5f + 0.5f)) * GameWindowsH
        };
        Vector2D cameraYY = {
            MatrixArray[4] * peopleCoordinate.x + MatrixArray[5] * peopleCoordinate.y + MatrixArray[6] * (peopleCoordinate.z + size_1) + MatrixArray[7],
            MatrixArray[4] * peopleCoordinate.x + MatrixArray[5] * peopleCoordinate.y + MatrixArray[6] * (peopleCoordinate.z + size_2) + MatrixArray[7]
        };
        Vector2D screenY = {
            (1.0f - ((cameraYY.x * RHW) * 0.5f + 0.5f)) * GameWindowsH,
            (1.0f - ((cameraYY.y * RHW) * 0.5f + 0.5f)) * GameWindowsH
        };
        screenBox.height = static_cast<float>(abs(screenY.x - screenY.y));
        screenBox.width = screenBox.height * 0.526515151552f;
        screenBox.x = static_cast<float>(screenCoordinate.x - screenBox.width / 2);
        screenBox.y = static_cast<float>(screenY.x - screenBox.height);
    }
    return screenBox;
}

Vector2D AutoAiming(Vector3D myself, Vector3D enemy) {
    Vector3D difference = { myself.x - enemy.x, myself.y - enemy.y, myself.z - enemy.z};
    Vector2D a = { 0, 0 };
    if (difference.x > 0 && difference.y > 0)
        a.x = atan(difference.y / difference.x) / (float)PI * 180 - 180;
    else if (difference.x <= 0 && difference.y > 0)
        a.x = atan(difference.y / difference.x) / (float)PI * 180;
    else if (difference.x <= 0 && difference.y < 0)
        a.x = atan(difference.y / difference.x) / (float)PI * 180;
    else if (difference.x >= 0 && difference.y < 0)
        a.x = atan(difference.y / difference.x) / (float)PI * 180 + 180;
    a.y = atan(difference.z / sqrt(difference.x * difference.x + difference.y * difference.y)) / (float)PI * 180;
    return a;
}
static Vector2D Towards_tp_offset(float Yaw, float Yaw_add, float speed) {
    Vector2D return_coordinate = { -1, -1 };
    if (Yaw != -1) {
        Yaw += Yaw_add;
        if (Yaw >= 0 + Yaw_add && Yaw < 90 + Yaw_add) {
            return_coordinate.x = (1 - Yaw / 90) * speed;
            return_coordinate.y = Yaw / 90 * speed;
        }
        else if (Yaw >= 90 + Yaw_add && Yaw < 180 + Yaw_add) {
            return_coordinate.x = (1 - Yaw / 90) * speed;
            return_coordinate.y = (180 - Yaw) / 90 * speed;
        }
        else if (Yaw >= 180 + Yaw_add && Yaw < 270 + Yaw_add) {
            return_coordinate.x = -(1 - (Yaw - 180) / 90) * speed;
            return_coordinate.y = -((Yaw - 180) / 90) * speed;
        }
        else if (Yaw >= 270 + Yaw_add && Yaw < 360 + Yaw_add) {
            return_coordinate.x = -(1 - (Yaw - 180) / 90) * speed;
            return_coordinate.y = -((360 - Yaw) / 90) * speed;
        }
    }
    return return_coordinate;
}
static Vector2D Towards_tp(float Yaw, float Yaw_add, Vector3D myself, float speed) {
    Vector2D return_coordinate = Towards_tp_offset(Yaw, Yaw_add, speed);
    return_coordinate.x += (float)myself.x;
    return_coordinate.y += (float)myself.y;
    return return_coordinate;
}
void Coordinate_Telesport(DWORD64 my_location, Vector3D Translation_My, float Yaw, float speed, int Function_Key, int Front_Key, int Rear_Key, int Left_Key, int Right_Key)
{
    if ((Function_Key >= 0 && Front_Key <= 256) ? GetAsyncKeyState(Function_Key) != 0 : (Function_Key == -1))
    {
        if (GetAsyncKeyState(Front_Key) != 0 || GetAsyncKeyState(Rear_Key) != 0 || GetAsyncKeyState(Left_Key) != 0 || GetAsyncKeyState(Right_Key) != 0)
        {
            float Yaw_add = -1;
            if (Yaw != -1 && Translation_My.x != -1 && Translation_My.y != -1 && Translation_My.z != -1)
            {
                if (GetAsyncKeyState(Front_Key) != 0)
                {
                    Yaw_add = 0;
                }
                else if (GetAsyncKeyState(Left_Key) != 0)
                {
                    Yaw_add = -90;
                }
                else if (GetAsyncKeyState(Right_Key) != 0)
                {
                    Yaw_add = 90;
                }
                if (Yaw_add != -1)
                {
                    Vector2D Write_data = Towards_tp(Yaw, Yaw_add, Translation_My, speed);
                    if (Write_data.x != -1 && Write_data.y != -1)
                    {
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x0, (float)Write_data.x);
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x4, (float)Write_data.y);
                    }
                }
                else if (GetAsyncKeyState(Rear_Key) != 0)
                {
                    Vector2D Write_data = Towards_tp_offset(Yaw, 0, speed);
                    if (Write_data.x != -1 && Write_data.y != -1)
                    {
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x0, (float)(-Write_data.x + Translation_My.x));
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x4, (float)(-Write_data.y + Translation_My.y));
                    }
                }
            }
        }
    }
}

string IntToHex(int value) {
    stringstream ss;
    ss << hex << uppercase << value;
    return ss.str();
}
bool IsKeyPressed(int key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}
bool ForceTerminateProcess(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
        return false;
    }
    bool success = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return success;
}
DWORD GetProcessIdByName(const wchar_t* processName)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    if (!Process32First(hSnapshot, &pe32))
    {
        CloseHandle(hSnapshot);
        return -1;
    }
    DWORD processId = 0;
    do
    {
        if (_wcsicmp(pe32.szExeFile, processName) == 0)
        {
            processId = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    if (processId == 0)
    {
        return -1;
    }
    return processId;
}

uintptr_t AllocateMemory(size_t size, DWORD protection)
{
    return (uintptr_t)VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, protection);
}