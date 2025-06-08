#include "Base.h"
#include "Memory_LC.h"

// 读长整数类型，失败返回0。
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
// 读短整数
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
// 读非负整数类型，失败返回0。
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
// 读整数
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
// 读浮点数
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
// 读双精度小数点
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
// 读字节类型，失败返回0。
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
// 读取字节数组
vector<BYTE> ReadBytes(const DWORD64 address, SIZE_T length)
{
    vector<BYTE> buffer(length);
    if (address < 0xFFFF)
        return { };
    memcpy(buffer.data(), (void*)address, length);
    return buffer;
}
// 读取字符串
string ReadString(const long long address, SIZE_T maxLength)
{
    vector<BYTE> buffer(maxLength);
    if (address < 0xFFFF)
        return "";
    // 从指定地址读取数据到缓冲区
    memcpy(buffer.data(), (void*)address, maxLength);

    // 通过查找 null 或问号的索引缩短缓冲区
    size_t nullIndex = maxLength; // 默认最大长度
    size_t questionIndex = maxLength;

    for (size_t i = 0; i < maxLength; ++i) {
        if (buffer[i] == 0 && nullIndex == maxLength) {
            nullIndex = i; // 找到第一个 null 字符
        }
        if (buffer[i] == '?' && questionIndex == maxLength) {
            questionIndex = i; // 找到第一个问号
        }
    }

    // 确定有效长度
    size_t effectiveLength = min(nullIndex, questionIndex);

    // 从缓冲区创建字符串
    return string(buffer.begin(), buffer.begin() + effectiveLength);
}
// 读取矩阵
void ReadMatrixArray(const DWORD64 matrixHead, float* Matrix_Array)
{
    for (int i = 0; i < 16; i++)
    {
        Matrix_Array[i] = ReadFloat(matrixHead + i * 0x4);
    }
}

// 写长整数
bool WriteLong(const DWORD64 address, DWORD64 value)
{
    __try
    {
        *(long long*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // 写入失败，不做任何处理
        return false;
    }
}
// 写非负整数
bool WriteUint(const DWORD64 address, const uint32_t value)
{
    __try
    {
        *(uint32_t*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // 写入失败，不做任何处理
        return false;
    }
}
// 写整数
bool WriteInt(const DWORD64 address, int value)
{
    __try
    {
        *(int*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // 写入失败，不做任何处理
        return false;
    }
}
// 写浮点数
bool WriteFloat(const DWORD64 address, float value)
{
    __try
    {
        *(float*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // 写入失败，不做任何处理
        return false;
    }
}
// 写双精度小数点
bool WriteDouble(const DWORD64 address, double value)
{
    __try
    {
        *(double*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // 写入失败，不做任何处理
        return false;
    }
}
// 写字节型
bool WriteByte(const DWORD64 address, BYTE value)
{
    __try
    {
        *(BYTE*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // 写入失败，不做任何处理
        return false;
    }
}
bool WriteBytes(const DWORD64 address, const std::vector<BYTE>& buffer) {
    DWORD old_protect;
    // 修改内存保护类型为可读写
    if (!VirtualProtect((LPVOID)address, buffer.size(), PAGE_EXECUTE_READWRITE, &old_protect)) return false;
    // 写入数据
    __try {
        memcpy((void*)address, buffer.data(), buffer.size());
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        VirtualProtect((LPVOID)address, buffer.size(), old_protect, &old_protect);
        return false;
    }
    // 恢复原始保护类型
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
        // 写入失败，不做任何处理
        return false;
    }
}

// 世界坐标转屏幕坐标（虚幻引擎）
Vector2D WorldToScreen_UE(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH)
{
    // 初始化屏幕坐标
    Vector2D screenCoordinate = { -1.0, -1.0 };

    // 世界坐标转换为相机坐标
    float cameraZ = MatrixArray[3] * (float)peopleCoordinate.x + MatrixArray[7] * (float)peopleCoordinate.y + MatrixArray[11] * (float)peopleCoordinate.z + MatrixArray[15];

    // 检查相机坐标的W值，确保物体在相机前方
    if (cameraZ > 0)
    {
        // 世界坐标转换为相机坐标
        float cameraX = MatrixArray[0] * (float)peopleCoordinate.x + MatrixArray[4] * (float)peopleCoordinate.y + MatrixArray[8] * (float)peopleCoordinate.z + MatrixArray[12];
        float cameraY = MatrixArray[1] * (float)peopleCoordinate.x + MatrixArray[5] * (float)peopleCoordinate.y + MatrixArray[9] * (float)peopleCoordinate.z + MatrixArray[13];

        // 相机坐标转换为标准化设备坐标（NDC）
        float RHW = 1.0f / cameraZ; // 相机深度倒数
        float ndcX = cameraX * RHW;
        float ndcY = cameraY * RHW;

        // 将NDC转换为屏幕坐标
        screenCoordinate.x = (ndcX * 0.5f + 0.5f) * GameWindowsW;
        screenCoordinate.y = (1.0f - (ndcY * 0.5f + 0.5f)) * GameWindowsH;
    }

    return screenCoordinate; // 返回屏幕坐标
}

// 世界坐标转屏幕坐标（起源引擎）
Vector2D WorldToScreen_Source(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH)
{
    // 初始化屏幕坐标
    Vector2D screenCoordinate = { -1.0, -1.0 };

    // 世界坐标转换为相机坐标
    float cameraZ = MatrixArray[8] * (float)peopleCoordinate.x + MatrixArray[9] * (float)peopleCoordinate.y + MatrixArray[10] * (float)peopleCoordinate.z + MatrixArray[11];

    // 检查相机坐标的W值，确保物体在相机前方
    if (cameraZ > 0)
    {
        // 世界坐标转换为相机坐标
        float cameraX = MatrixArray[0] * (float)peopleCoordinate.x + MatrixArray[1] * (float)peopleCoordinate.y + MatrixArray[2] * (float)peopleCoordinate.z + MatrixArray[3];
        float cameraY = MatrixArray[4] * (float)peopleCoordinate.x + MatrixArray[5] * (float)peopleCoordinate.y + MatrixArray[6] * (float)peopleCoordinate.z + MatrixArray[7];

        // 相机坐标转换为标准化设备坐标（NDC）
        float RHW = 1.0f / cameraZ; // 相机深度倒数
        float ndcX = cameraX * RHW;
        float ndcY = cameraY * RHW;

        // 将NDC转换为屏幕坐标
        screenCoordinate.x = (ndcX * 0.5f + 0.5f) * GameWindowsW;
        screenCoordinate.y = (1.0f - (ndcY * 0.5f + 0.5f)) * GameWindowsH;
    }

    return screenCoordinate; // 返回屏幕坐标
}

// 世界坐标转屏幕坐标_盒子（起源引擎）
Box WorldToScreen_Source_Box(Vector3D peopleCoordinate, float* MatrixArray, int GameWindowsW, int GameWindowsH, float size_1, float size_2)
{
    // 初始化屏幕坐标
    Box screenBox = { -1.0, -1.0, -1.0, -1.0 };

    // 世界坐标转换为相机坐标
    float cameraZ = MatrixArray[8] * (float)peopleCoordinate.x + MatrixArray[9] * (float)peopleCoordinate.y + MatrixArray[10] * (float)peopleCoordinate.z + MatrixArray[11];

    // 检查相机坐标的W值，确保物体在相机前方
    if (cameraZ > 0)
    {
        // 世界坐标转换为相机坐标
        float cameraX = MatrixArray[0] * (float)peopleCoordinate.x + MatrixArray[1] * (float)peopleCoordinate.y + MatrixArray[2] * (float)peopleCoordinate.z + MatrixArray[3];
        float cameraY = MatrixArray[4] * (float)peopleCoordinate.x + MatrixArray[5] * (float)peopleCoordinate.y + MatrixArray[6] * (float)peopleCoordinate.z + MatrixArray[7];

        // 相机坐标转换为标准化设备坐标（NDC）
        float RHW = 1.0f / cameraZ; // 相机深度倒数
        float ndcX = cameraX * RHW;
        float ndcY = cameraY * RHW;

        // 将NDC转换为屏幕坐标
        Vector2D screenCoordinate = {
            (ndcX * 0.5f + 0.5f) * GameWindowsW,
            (1.0f - (ndcY * 0.5f + 0.5f)) * GameWindowsH
        };

        // 计算物体高度在屏幕上的映射
        Vector2D cameraYY = {
            MatrixArray[4] * peopleCoordinate.x + MatrixArray[5] * peopleCoordinate.y + MatrixArray[6] * (peopleCoordinate.z + size_1) + MatrixArray[7],
            MatrixArray[4] * peopleCoordinate.x + MatrixArray[5] * peopleCoordinate.y + MatrixArray[6] * (peopleCoordinate.z + size_2) + MatrixArray[7]
        };

        // 将Y轴上的高度转换为屏幕坐标
        Vector2D screenY = {
            (1.0f - ((cameraYY.x * RHW) * 0.5f + 0.5f)) * GameWindowsH,
            (1.0f - ((cameraYY.y * RHW) * 0.5f + 0.5f)) * GameWindowsH
        };

        // 计算矩形的高度和宽度
        screenBox.height = static_cast<float>(abs(screenY.x - screenY.y));
        screenBox.width = screenBox.height * 0.526515151552f; // 根据人体比例计算宽度

        // 设置矩形的位置
        screenBox.x = static_cast<float>(screenCoordinate.x - screenBox.width / 2);
        screenBox.y = static_cast<float>(screenY.x - screenBox.height);
    }

    return screenBox; // 返回屏幕坐标
}

// 一阶自动瞄准计算
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
// 计算朝向瞬移（没有高度 + 返回世界坐标）
static Vector2D Towards_tp_offset(float Yaw, float Yaw_add, float speed) {
    // 初始化返回坐标
    Vector2D return_coordinate = { -1, -1 };
    // 判断参数是否有效
    if (Yaw != -1) {
        // 角度调整
        Yaw += Yaw_add;
        // 判断加速方向
        if (Yaw >= 0 + Yaw_add && Yaw < 90 + Yaw_add) { // 0 x+
            return_coordinate.x = (1 - Yaw / 90) * speed;
            return_coordinate.y = Yaw / 90 * speed;
        }
        else if (Yaw >= 90 + Yaw_add && Yaw < 180 + Yaw_add) { // 90 y+
            return_coordinate.x = (1 - Yaw / 90) * speed;
            return_coordinate.y = (180 - Yaw) / 90 * speed;
        }
        else if (Yaw >= 180 + Yaw_add && Yaw < 270 + Yaw_add) { // 180 x-
            return_coordinate.x = -(1 - (Yaw - 180) / 90) * speed;
            return_coordinate.y = -((Yaw - 180) / 90) * speed;
        }
        else if (Yaw >= 270 + Yaw_add && Yaw < 360 + Yaw_add) { // 270 y-
            return_coordinate.x = -(1 - (Yaw - 180) / 90) * speed;
            return_coordinate.y = -((360 - Yaw) / 90) * speed;
        }
    }
    return return_coordinate;
}
// 一阶计算朝向瞬移（包含高度）
static Vector2D Towards_tp(float Yaw, float Yaw_add, Vector3D myself, float speed) {
    // 初始化返回坐标
    Vector2D return_coordinate = Towards_tp_offset(Yaw, Yaw_add, speed);
    // 计算坐标数据
    return_coordinate.x += (float)myself.x;
    return_coordinate.y += (float)myself.y;
    // 返回数据
    return return_coordinate;
}
// 坐标瞬移（平滑穿墙 Alt:0x12 方向键（上下左右）:0x26 0x28 0x25 0x27）
void Coordinate_Telesport(DWORD64 my_location, Vector3D Translation_My, float Yaw, float speed, int Function_Key, int Front_Key, int Rear_Key, int Left_Key, int Right_Key)
{
    if ((Function_Key >= 0 && Front_Key <= 256) ? GetAsyncKeyState(Function_Key) != 0 : (Function_Key == -1)) // 判断是否需要功能键
    {
        if (GetAsyncKeyState(Front_Key) != 0 || GetAsyncKeyState(Rear_Key) != 0 || GetAsyncKeyState(Left_Key) != 0 || GetAsyncKeyState(Right_Key) != 0) // 判断是否有方向键
        {
            // 初始化瞬移参数
            float Yaw_add = -1;
            // 判断坐标信息是否获取成功
            if (Yaw != -1 && Translation_My.x != -1 && Translation_My.y != -1 && Translation_My.z != -1)
            {
                // 判断是否按"上"
                if (GetAsyncKeyState(Front_Key) != 0)
                {
                    Yaw_add = 0;
                }
                // 判断是否按"左"
                else if (GetAsyncKeyState(Left_Key) != 0)
                {
                    Yaw_add = -90;
                }
                // 判断是否按"右"
                else if (GetAsyncKeyState(Right_Key) != 0)
                {
                    Yaw_add = 90;
                }
                // 判断是否按键加速
                if (Yaw_add != -1)
                {
                    // 计算加速变量
                    Vector2D Write_data = Towards_tp(Yaw, Yaw_add, Translation_My, speed);
                    // 判断加速变量是否为空
                    if (Write_data.x != -1 && Write_data.y != -1)
                    {
                        // 写入加速变量
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x0, (float)Write_data.x);
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x4, (float)Write_data.y);
                    }
                }
                // 判断是否按"下"
                else if (GetAsyncKeyState(Rear_Key) != 0)
                {
                    // 计算加速变量
                    Vector2D Write_data = Towards_tp_offset(Yaw, 0, speed);
                    // 判断加速变量是否为空
                    if (Write_data.x != -1 && Write_data.y != -1)
                    {
                        // 写入加速变量
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x0, (float)(-Write_data.x + Translation_My.x));
                        WriteFloat(my_location + 0x1C0 + 0x10 + 0x4, (float)(-Write_data.y + Translation_My.y));
                    }
                }
            }
        }
    }
}

// 整数转16进制字符串
string IntToHex(int value) {
    stringstream ss;
    ss << hex << uppercase << value; // 十六进制大写
    return ss.str();
}
// 检测是否被按下
bool IsKeyPressed(int key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}
// 通过进程ID结束进程
bool ForceTerminateProcess(DWORD pid)
{
    // 打开指定进程
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
        return false;
    }
    // 强制终止进程
    bool success = TerminateProcess(hProcess, 0);
    // 关闭进程句柄
    CloseHandle(hProcess);
    return success;
}
// 通过名称获取进程ID
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

// 分配内存
uintptr_t AllocateMemory(size_t size, DWORD protection)
{
    return (uintptr_t)VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, protection);
}