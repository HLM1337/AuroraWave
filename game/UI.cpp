#include "UI.h"
#include "base.h"
#include "game.h"
#include "Library/ImGui/imgui.h"
#include "output/offsets.hpp"
#include "output/client_dll.hpp"
#include "output/buttons.hpp"
#include "Memory_LC.h"
#include "Cheats/Visuals/Watermark.h"
#include <algorithm> // 添加algorithm头文件以使用std::min函数

// ImGui绘制函数实现 - 从ImGui_LC.cpp整合而来
// 定义颜色变量
ImU32 White = ImColor(255, 255, 255, 255);
ImU32 Black = ImColor(0, 0, 0, 255);
ImU32 Red = ImColor(255, 0, 0, 210);
ImU32 Green = ImColor(0, 255, 0, 210);
ImU32 Blue = ImColor(0, 0, 255, 210);
ImU32 Yellow = ImColor(255, 255, 0, 210);

// 绘制文本
void DrawTexts(double x, double y, const char* text, ImU32 color, ImFont* font, double size, ImU32 outline_color, float outline_thickness)
{
    // 优化：只绘制4个方向的描边
    float offset = outline_thickness;
    ImGui::GetBackgroundDrawList()->AddText(font, (float)size, ImVec2((float)x - offset, (float)y), outline_color, text);
    ImGui::GetBackgroundDrawList()->AddText(font, (float)size, ImVec2((float)x + offset, (float)y), outline_color, text);
    ImGui::GetBackgroundDrawList()->AddText(font, (float)size, ImVec2((float)x, (float)y - offset), outline_color, text);
    ImGui::GetBackgroundDrawList()->AddText(font, (float)size, ImVec2((float)x, (float)y + offset), outline_color, text);
    // 最后绘制原始文本
    ImGui::GetBackgroundDrawList()->AddText(font, (float)size, ImVec2((float)x, (float)y), color, text);
}

// 绘制矩形
void DrawRectangle(double x, double y, double width, double height, ImU32 color, float thickness, float rounding, ImDrawFlags flags)
{
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2((float)x, (float)y), ImVec2((float)(x + width), (float)(y + height)), color, rounding, flags, thickness);
}

// 绘制填充矩形
void DrawFilledRectangle(double x, double y, double width, double height, ImU32 color, float rounding, ImDrawFlags flags)
{
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2((float)x, (float)y), ImVec2((float)(x + width), (float)(y + height)), color, rounding, flags);
}

// 绘制线段
void DrawLine(double x1, double y1, double x2, double y2, ImU32 color, float thickness)
{
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2((float)x1, (float)y1), ImVec2((float)x2, (float)y2), color, thickness);
}

// 绘制圆形
void DrawCircle(double x, double y, float radius, ImU32 color, int num_segments, float thickness)
{
    // 分段数自适应，提升观感和性能
    int seg = std::clamp((int)(radius * 1.5f), 12, 64);
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2((float)x, (float)y), radius, color, seg, thickness);
}

// 绘制填充圆形
void DrawFilledCircle(ImVec2 center, float radius, ImU32 color, int num_segments)
{
    ImGui::GetBackgroundDrawList()->AddCircleFilled(center, radius, color, num_segments);
}

// 绘制描边方框（带外框和内框的双层矩形）
void DrawOutlinedBox(double x, double y, double width, double height, ImU32 inner_color, ImU32 outer_color, float thickness, float rounding, ImDrawFlags flags)
{
    // 外框（稍大一点）
    DrawRectangle(x - thickness, y - thickness, width + thickness * 2, height + thickness * 2, outer_color, thickness, rounding, flags);
    // 内框
    DrawRectangle(x, y, width, height, inner_color, thickness, rounding, flags);
}

// 绘制生命值条
void DrawHealthBar(double x, double y, double width, double height, int health, int max_health, bool vertical, ImU32 color_full, ImU32 color_empty, ImU32 outline_color, float thickness)
{
    // 确保血量在有效范围内
    health = (health < 0) ? 0 : (health > max_health) ? max_health : health;
    
    // 计算血量占比
    float health_ratio = static_cast<float>(health) / static_cast<float>(max_health);
    
    // 计算描边偏移量
    float outline_offset = thickness;
    
    // 绘制外部描边（黑色外框）
    DrawRectangle(x - outline_offset, y - outline_offset, 
                 width + outline_offset * 2, height + outline_offset * 2, 
                 outline_color, thickness);
    
    // 计算填充区域的大小
    float fill_width = vertical ? static_cast<float>(width) : static_cast<float>(width * health_ratio);
    float fill_height = vertical ? static_cast<float>(height * health_ratio) : static_cast<float>(height);
    
    // 计算填充区域的起始位置
    float fill_x = static_cast<float>(x);
    float fill_y = vertical ? static_cast<float>(y + height - fill_height) : static_cast<float>(y);
    
    // 绘制血量底色（空血条）
    DrawFilledRectangle(x, y, width, height, color_empty, 0.0f);
    
    // 绘制当前血量（实际血量）
    DrawFilledRectangle(fill_x, fill_y, fill_width, fill_height, color_full, 0.0f);
    
    // 绘制内部描边（内框线）
    DrawRectangle(x, y, width, height, ImColor(255, 255, 255, 100), 1.0f);
}

// 添加一个基于距离计算间距的工具函数
float CalculateDistanceBasedValue(float distance, float minValue, float maxValue, float minDistance = 100.0f, float maxDistance = 1000.0f)
{
    // 将距离限制在有效范围内
    float clampedDistance = std::clamp(distance, minDistance, maxDistance);
    
    // 计算距离比例因子（近距离为1，远距离为0）
    float distanceFactor = 1.0f - ((clampedDistance - minDistance) / (maxDistance - minDistance));
    
    // 根据距离因子计算返回值
    return minValue + (maxValue - minValue) * distanceFactor;
}

static bool is_draw = true; // 绘制开关
static bool is_draw_line = false; // 绘制射线
static bool is_draw_box = false; // 绘制方框
static bool is_draw_bone = true; // 绘制骨骼
static bool is_draw_friend = true; // 绘制队友开关
static bool is_draw_name = false; // 绘制名称开关
static bool is_draw_health = false; // 绘制血量开关
static bool is_draw_distance = false; // 绘制距离开关
static bool is_draw_weapon = false; // 绘制武器开关
static bool is_draw_obstacles = false; // 障碍判断开关
static bool is_draw_aim_range = true; // 瞄准范围线
static bool is_aim = true; // 瞄准开关
static bool is_aim_obstacles = false; // 障碍判断开关

// 新增队友专用绘制选项
static bool is_draw_friend_box = false; // 队友绘制方框
static bool is_draw_friend_bone = true; // 队友绘制骨骼
static bool is_draw_friend_line = false; // 队友绘制射线
static bool is_draw_friend_name = false; // 队友绘制名称
static bool is_draw_friend_health = false; // 队友绘制血量
static bool is_draw_friend_healthbar = false; // 队友生命值条开关
static bool is_draw_friend_distance = false; // 队友绘制距离
static bool is_draw_friend_weapon = false; // 队友绘制武器
static bool is_draw_friend_obstacles = false; // 队友障碍判断

static bool is_aim_lock_teammate = false; // 自瞄是否锁队友开关

static bool is_recoil_sub = true; // 后坐力补偿开关
static Vector2D recoil_offset{}; // 后坐力补偿偏移
static int is_aim_bone = 12; // 瞄准部位
static int is_aim_key = VK_XBUTTON2; // 瞄准按键
static float aim_max_range = 150.0f; // 用户设置的自瞄最大范围
static float aim_max = 150.0f; // 瞄准距离 (动态)
static DWORD64 Aim_Address = 0; // 瞄准地址
static Vector3D Aim_Pos = { -1.0, -1.0, -1.0 }; // 瞄准目标
static bool is_fov = false; // 视角开关
static float fov = 90.0f; // 视角大小
atomic<bool> is_fov_thread_running(false); // 视角线程运行状态
static bool is_enable_flash = true; // 防止闪光开关

// 文本描边相关
static ImColor text_outline_color = ImColor(0, 0, 0, 255); // 文本描边颜色
static float text_outline_thickness = 1.0f; // 文本描边粗细

// 新增 - 方框类型
static bool is_draw_healthbar = false; // 生命值条开关
static ImColor box_outline_color = ImColor(0, 0, 0, 255); // 方框描边颜色

static DWORD64 client = (DWORD64)GetModuleHandleA("client.dll"); // 客户端基址
static DWORD64 server = (DWORD64)GetModuleHandleA("server.dll"); // 服务器基址
static DWORD64 engine2 = (DWORD64)GetModuleHandleA("engine2.dll"); // 引擎基址
std::string My_Name = ""; // 自身昵称
static uint32_t My_fFlag = 0; // 自身状态
static float My_flashDuration = 0.0f; // 自身遭受闪光时间
static Vector3D My_Pos = { -1.0f, -1.0f, -1.0f }; // 自身坐标
static Vector3D Enemy_Pos = { -1.0f, -1.0f, -1.0f }; // 对象坐标
static int Pid = GetCurrentProcessId(); // 获取当前进程ID

static float line_cx = 1.0f; // 射线粗细
static float bone_cx = 2.5f; // 骨骼粗细
static float Circle_cx = 1.0f; // 圆形粗细
static float Circle_min = 1.5f; // 圆形最小半径
static float Circle_max = 50.0f; // 圆形最大半径
static float box_cx = 1.0f; // 矩形粗细
static float text_cx = 18.0f; // 文本大小
static float box_yj = 0; // 矩形圆角
static ImColor Circle_color = ImColor{ 255, 255, 255, 210 }; // 圆形颜色
static ImColor line_color = ImColor{ 255, 255, 255, 100 }; // 射线颜色
static ImColor bone_color = ImColor{ 255, 0, 0, 150 }; // 骨骼颜色
static ImColor box_color = ImColor{ 255, 0, 0, 210 }; // 矩形颜色
static ImColor text_color = ImColor{ 255, 255, 255, 255 }; // 文本颜色
static int style_idx = 2; // 初始化风格选择框现行选中项 0.白色 1.蓝色 2.紫色
static float round_ = 10.0f; // 圆角半径

static DWORD64 My_Address; // 自身地址

const unsigned int INAIR = 65664;// 在空中
const unsigned int STANDING = 65665;// 站立
const unsigned int CROUCHING = 65667;// 蹲伏
const unsigned int PLUS_ATTACK = 65537;// +attack
const unsigned int MINUS_ATTACK = 256;// -attack

// 新增：自瞄平滑参数
static float aimSmoothX = 1.0f; // 自瞄平滑X
static float aimSmoothY = 1.0f; // 自瞄平滑Y

// 新增：边缘发光变量
static bool is_glow_enemy = false;    // 敌人发光开关
static bool is_glow_teammate = false; // 队友发光开关
static ImVec4 g_glowcolor = ImVec4(1.0f, 0.0f, 0.0f, 0.7f); // 敌人发光颜色 (RGBA)
static ImVec4 g_glowcolor_teammate = ImVec4(0.0f, 1.0f, 0.0f, 0.7f); // 队友发光颜色 (RGBA)

// 全局上下文结构体，存储偏移和其他游戏信息
struct GameContext {
    uintptr_t m_glow;                       // 边缘发光基址偏移
    uintptr_t m_glowcolor;                  // 边缘发光颜色偏移
    uintptr_t m_bglowing;                   // 边缘发光状态偏移
    uintptr_t m_bEligibleForScreenHighlight; // 边缘发光屏幕高亮状态偏移
} g_ctx;

// 全局变量集合，存储设置和状态
struct GlobalVars {
    ImVec4 glowcolor;                       // 边缘发光颜色
} g_vars;

static void Unload() // 卸载 DLL
{
    UnloadHook();
}

static string GetWeaponName(short weapon_id) // 获取武器名称
{
    static map<short, string> weaponNames = {
        {1, "DESERT_EAGLE"},
        {2, "DUAL_BERETTAS"},
        {3, "FIVE_SEVEN"},
        {4, "GLOCK_18"},
        {7, "AK_47"},
        {8, "AUG"},
        {9, "AWP"},
        {10, "FAMAS"},
        {11, "G3SG1"},
        {13, "GALIL_AR"},
        {14, "M249"},
        {16, "M4A4"},
        {17, "MAC_10"},
        {19, "P90"},
        {20, "REPULSOR_DEVICE"},
        {23, "MP5_SD"},
        {24, "UMP_45"},
        {25, "XM1014"},
        {26, "PP_BIZON"},
        {27, "MAG_7"},
        {28, "NEGEV"},
        {29, "SAWED_OFF"},
        {30, "TEC_9"},
        {31, "ZEUS_X27"},
        {32, "P2000"},
        {33, "MP7"},
        {34, "MP9"},
        {35, "NOVA"},
        {36, "P250"},
        {37, "RIOT_SHIELD"},
        {38, "SCAR_20"},
        {39, "SG_553"},
        {40, "SSG_08"},
        {41, "KNIFE"},
        {42, "KNIFE"},
        {43, "FLASHBANG"},
        {44, "HIGH_EXPLOSIVE_GRENADE"},
        {45, "SMOKE_GRENADE"},
        {46, "MOLOTOV"},
        {47, "DECOY_GRENADE"},
        {48, "INCENDIARY_GRENADE"},
        {49, "C4_EXPLOSIVE"},
        {50, "KEVLAR_VEST"},
        {51, "KEVLAR__HELMETT"},
        {52, "HEAVY_ASSAULT_SUIT"},
        {55, "DEFUSE_KIT"},
        {56, "RESCUE_KIT"},
        {57, "MEDI_SHOT"},
        {58, "MUSIC_KIT"},
        {59, "KNIFE"},
        {60, "M4A1_S"},
        {61, "USP_S"},
        {62, "TRADE_UP_CONTRACT"},
        {63, "CZ75_AUTO"},
        {64, "R8_REVOLVER"},
        {68, "TACTICAL_AWARENESS_GRENADE"},
        {69, "BARE_HANDS"},
        {70, "BREACH_CHARGE"},
        {72, "TABLET"},
        {74, "KNIFE"},
        {75, "AXE"},
        {76, "HAMMER"},
        {78, "WRENCH"},
        {80, "SPECTRAL_SHIV"},
        {81, "FIRE_BOMB"},
        {82, "DIVERSION_DEVICE"},
        {83, "FRAG_GRENADE"},
        {84, "SNOWBALL"},
        {85, "BUMP_MINE"},
        {500, "BAYONET"},
        {503, "CLASSIC_KNIFE"},
        {505, "FLIP_KNIFE"},
        {506, "GUT_KNIFE"},
        {507, "KARAMBIT"},
        {508, "M9_BAYONET"},
        {509, "HUNTSMAN_KNIFE"},
        {512, "FALCHION_KNIFE"},
        {514, "BOWIE_KNIFE"},
        {515, "BUTTERFLY_KNIFE"},
        {516, "SHADOW_DAGGERS"},
        {517, "PARACORD_KNIFE"},
        {518, "SURVIVAL_KNIFE"},
        {519, "URSUS_KNIFE"},
        {520, "NAVAJA_KNIFE"},
        {521, "NOMAD_KNIFE"},
        {522, "STILETTO_KNIFE"},
        {523, "TALON_KNIFE"},
        {525, "SKELETON_KNIFE"},
        {526, "KUKRI_KNIFE"},
    };
    auto find_id = weaponNames.find(weapon_id);
    return (find_id != weaponNames.end()) ? find_id->second : "Unknown";
}

struct Bone
{
    Vector3D head, neck, chest, crotch; // 头部、脖子、胸部、屁股
    Vector3D neck_l, wrist_l, palm_l; // 左手臂
    Vector3D neck_r, wrist_r, palm_r; // 右手臂
    Vector3D leg_l, knee_l, foot_l; // 左腿
    Vector3D leg_r, knee_r, foot_r; // 右腿

    Vector2D head_2D, neck_2D, chest_2D, crotch_2D; // 头部、脖子、胸部、屁股
    Vector2D neck_l_2D, wrist_l_2D, palm_l_2D; // 左手臂
    Vector2D neck_r_2D, wrist_r_2D, palm_r_2D; // 右手臂
    Vector2D leg_l_2D, knee_l_2D, foot_l_2D; // 左腿
    Vector2D leg_r_2D, knee_r_2D, foot_r_2D; // 右腿
};

static Vector3D GetBonePos(DWORD64 entity, int bone_id) // 获取实体骨骼位置
{
    DWORD64 GameSceneNode = ReadLong(entity + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
    DWORD64 BoneArrayAddress = ReadLong(GameSceneNode + 0x1F0);
    Vector3D Tmp_Pos = {
        ReadFloat(BoneArrayAddress + 0x10 * bone_id + 0x0),
        ReadFloat(BoneArrayAddress + 0x10 * bone_id + 0x4),
        ReadFloat(BoneArrayAddress + 0x10 * bone_id + 0x8)
    };
    return Tmp_Pos;
}

static Bone GetAllBonePos(DWORD64 entity, float* Matrix, int g_Width, int g_Height) // 获取实体骨骼位置
{
    Bone bone;
    bone.head = GetBonePos(entity, 12); // 头部
    bone.neck = GetBonePos(entity, 10); // 脖子
    bone.chest = GetBonePos(entity, 8); // 胸部
    bone.crotch = GetBonePos(entity, 0); // 屁股
    bone.neck_l = GetBonePos(entity, 16); // 左肩
    bone.wrist_l = GetBonePos(entity, 18); // 左手腕
    bone.palm_l = GetBonePos(entity, 42); // 左手掌
    bone.neck_r = GetBonePos(entity, 26); // 右肩
    bone.wrist_r = GetBonePos(entity, 28); // 右手腕
    bone.palm_r = GetBonePos(entity, 40); // 右手掌
    bone.leg_l = GetBonePos(entity, 44); // 左腿
    bone.knee_l = GetBonePos(entity, 46); // 左膝
    bone.foot_l = GetBonePos(entity, 48); // 左脚
    bone.leg_r = GetBonePos(entity, 50); // 右腿
    bone.knee_r = GetBonePos(entity, 52); // 右膝
    bone.foot_r = GetBonePos(entity, 54); // 右脚
    bone.head_2D = WorldToScreen_Source(bone.head, Matrix, g_Width, g_Height); // 头部2D坐标
    bone.neck_2D = WorldToScreen_Source(bone.neck, Matrix, g_Width, g_Height); // 脖子2D坐标
    bone.chest_2D = WorldToScreen_Source(bone.chest, Matrix, g_Width, g_Height); // 胸部2D坐标
    bone.crotch_2D = WorldToScreen_Source(bone.crotch, Matrix, g_Width, g_Height); // 屁股2D坐标
    bone.neck_l_2D = WorldToScreen_Source(bone.neck_l, Matrix, g_Width, g_Height); // 左肩2D坐标
    bone.wrist_l_2D = WorldToScreen_Source(bone.wrist_l, Matrix, g_Width, g_Height); // 左手腕2D坐标
    bone.palm_l_2D = WorldToScreen_Source(bone.palm_l, Matrix, g_Width, g_Height); // 左手掌2D坐标
    bone.neck_r_2D = WorldToScreen_Source(bone.neck_r, Matrix, g_Width, g_Height); // 右肩2D坐标
    bone.wrist_r_2D = WorldToScreen_Source(bone.wrist_r, Matrix, g_Width, g_Height); // 右手腕2D坐标
    bone.palm_r_2D = WorldToScreen_Source(bone.palm_r, Matrix, g_Width, g_Height); // 右手掌2D坐标
    bone.leg_l_2D = WorldToScreen_Source(bone.leg_l, Matrix, g_Width, g_Height); // 左腿2D坐标
    bone.knee_l_2D = WorldToScreen_Source(bone.knee_l, Matrix, g_Width, g_Height); // 左膝2D坐标
    bone.foot_l_2D = WorldToScreen_Source(bone.foot_l, Matrix, g_Width, g_Height); // 左脚2D坐标
    bone.leg_r_2D = WorldToScreen_Source(bone.leg_r, Matrix, g_Width, g_Height); // 右腿2D坐标
    bone.knee_r_2D = WorldToScreen_Source(bone.knee_r, Matrix, g_Width, g_Height); // 右膝2D坐标
    bone.foot_r_2D = WorldToScreen_Source(bone.foot_r, Matrix, g_Width, g_Height); // 右脚2D坐标
    return bone;
}

static void DrawAllBone(Bone bone, float circle_size)
{
    // 骨骼连线关系，便于维护和扩展
    struct BoneLine { const Vector2D* a; const Vector2D* b; };
    const BoneLine lines[] = {
        {&bone.head_2D, &bone.neck_2D}, // 头部-脖子
        {&bone.neck_2D, &bone.chest_2D}, // 脖子-胸部
        {&bone.chest_2D, &bone.crotch_2D}, // 胸部-屁股
        {&bone.neck_2D, &bone.neck_l_2D}, // 脖子-左肩
        {&bone.neck_l_2D, &bone.wrist_l_2D}, // 左肩-左手腕
        {&bone.wrist_l_2D, &bone.palm_l_2D}, // 左手腕-左手掌
        {&bone.neck_2D, &bone.neck_r_2D}, // 脖子-右肩
        {&bone.neck_r_2D, &bone.wrist_r_2D}, // 右肩-右手腕
        {&bone.wrist_r_2D, &bone.palm_r_2D}, // 右手腕-右手掌
        {&bone.crotch_2D, &bone.leg_l_2D}, // 屁股-左腿
        {&bone.leg_l_2D, &bone.knee_l_2D}, // 左腿-左膝
        {&bone.knee_l_2D, &bone.foot_l_2D}, // 左膝-左脚
        {&bone.crotch_2D, &bone.leg_r_2D}, // 屁股-右腿
        {&bone.leg_r_2D, &bone.knee_r_2D}, // 右腿-右膝
        {&bone.knee_r_2D, &bone.foot_r_2D}, // 右膝-右脚
    };
    // 判断点是否在屏幕内
    auto in_screen = [](const Vector2D& pt) {
        return pt.x > 0 && pt.y > 0 && pt.x < g_width && pt.y < g_height;
    };
    // 绘制骨骼线
    for (const auto& l : lines) {
        if (in_screen(*l.a) && in_screen(*l.b))
            DrawLine(l.a->x, l.a->y, l.b->x, l.b->y, bone_color, bone_cx);
    }
    // 可选：绘制关节点圆点，提升观感
    #ifdef DRAW_BONE_JOINT_CIRCLE
    const Vector2D* joints[] = {
        &bone.head_2D, &bone.neck_2D, &bone.chest_2D, &bone.crotch_2D,
        &bone.neck_l_2D, &bone.wrist_l_2D, &bone.palm_l_2D,
        &bone.neck_r_2D, &bone.wrist_r_2D, &bone.palm_r_2D,
        &bone.leg_l_2D, &bone.knee_l_2D, &bone.foot_l_2D,
        &bone.leg_r_2D, &bone.knee_r_2D, &bone.foot_r_2D
    };
    for (const auto& j : joints) {
        if (in_screen(*j))
            DrawCircle(j->x, j->y, circle_size, bone_color, 0, bone_cx);
    }
    #endif
}

static void FovThreadFunction() // FOV修改线程
{
    while (is_fov_thread_running)
    {
        auto localPlayer = ReadLong(client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
        auto cam_server = ReadLong(localPlayer + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pCameraServices);
        auto isScoped = ReadInt(localPlayer + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_bIsScoped); // 是否处于瞄准镜状态
        if (cam_server && !isScoped) // 非瞄准镜状态
            WriteUint(cam_server + cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_iFOV, (uint32_t)round(fov));
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

// 实现边缘发光功能
void ApplyGlowEffect(DWORD64 entity, bool is_teammate, int health) {
    // 检查实体是否有效、是否活着
    if (entity == 0 || health <= 0) return;
    
    // 根据是否为队友选择对应的发光设置
    if (is_teammate) {
        // 如果是队友但队友发光关闭，则直接返回
        if (!is_glow_teammate) return;
        g_vars.glowcolor = g_glowcolor_teammate; // 使用队友发光颜色
    } else {
        // 如果是敌人但敌人发光关闭，则直接返回
        if (!is_glow_enemy) return;
        g_vars.glowcolor = g_glowcolor; // 使用敌人发光颜色
    }
    
    // 获取边缘发光地址 (Glow属性位于C_BaseModelEntity::m_Glow)
    const uintptr_t glowaddr = entity + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Glow;
    // 新增：判断glowaddr是否有效，防止未初始化时写入导致崩溃
    if (glowaddr < 0x10000 || ReadLong(glowaddr) == 0) return;
    
    // 将颜色转换为DWORD格式 (ABGR)
    const ImVec4& g_color = g_vars.glowcolor;
    const DWORD g_c_stack = (DWORD(g_color.w * 255) << 24) | (DWORD(g_color.z * 255) << 16) | (DWORD(g_color.y * 255) << 8) | DWORD(g_color.x * 255);
    
    // 初始化偏移量（仅初始化一次）
    static bool offsetsInitialized = false;
    if (!offsetsInitialized) {
        g_ctx.m_glow = cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Glow;
        g_ctx.m_glowcolor = cs2_dumper::schemas::client_dll::CGlowProperty::m_glowColorOverride;
        g_ctx.m_bEligibleForScreenHighlight = cs2_dumper::schemas::client_dll::CGlowProperty::m_bEligibleForScreenHighlight;
        g_ctx.m_bglowing = cs2_dumper::schemas::client_dll::CGlowProperty::m_bGlowing;
        offsetsInitialized = true;
    }
    
    // 写入边缘发光颜色
    WriteLong(glowaddr + g_ctx.m_glowcolor, g_c_stack);
    
    // 启用边缘发光高亮
    WriteByte(glowaddr + g_ctx.m_bEligibleForScreenHighlight, 1);
    
    // 启用边缘发光
    WriteByte(glowaddr + g_ctx.m_bglowing, 1);
}

static bool watermark = true;

void UI()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io; // 获取输入输出对象
    ImFont* font = io.Fonts->Fonts[0]; // 获取默认字体
    ImGuiStyle& style = ImGui::GetStyle(); // 获取当前样式
    // 设置抗锯齿、圆角、透明度
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    style.FrameRounding = round_;
    style.Alpha = 0.95f;

    ImGui::Begin("DX11 Hook");
    if (ImGui::BeginTabBar("Tab"))
    {
        if (ImGui::BeginTabItem("绘制"))
        {
            ImGui::Checkbox("绘制总开关", &is_draw);
            if (is_draw)
            {
                if (ImGui::TreeNode("敌人绘制设置"))
                {
                    ImGui::Checkbox("绘制方框", &is_draw_box); ImGui::SameLine();
                    ImGui::Checkbox("绘制骨骼", &is_draw_bone); ImGui::SameLine();
                    ImGui::Checkbox("绘制射线", &is_draw_line);
                    ImGui::Checkbox("绘制血量", &is_draw_health); ImGui::SameLine();
                    ImGui::Checkbox("绘制距离", &is_draw_distance); ImGui::SameLine();
                    ImGui::Checkbox("绘制名称", &is_draw_name);
                    ImGui::Checkbox("绘制手持", &is_draw_weapon); ImGui::SameLine();
                    ImGui::Checkbox("掩体判断", &is_draw_obstacles); ImGui::SameLine();
                    ImGui::Checkbox("自瞄范围线", &is_draw_aim_range);
                    ImGui::Checkbox("生命值条", &is_draw_healthbar); ImGui::SameLine();
                    ImGui::Checkbox("敌人发光", &is_glow_enemy);
                    if (is_glow_enemy)
                    {
                        ImGui::SameLine();
                        ImGui::ColorEdit4("##敌人发光颜色", (float*)&g_glowcolor, ImGuiColorEditFlags_NoInputs);
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "(发光功能可能导致封禁!)");
                    }
                    ImGui::TreePop();
                }
                
                if (ImGui::TreeNode("队友绘制设置"))
                {
                    ImGui::Checkbox("绘制队友", &is_draw_friend);
                    if (is_draw_friend) {
                        ImGui::Checkbox("绘制方框##Friend", &is_draw_friend_box); ImGui::SameLine();
                        ImGui::Checkbox("绘制骨骼##Friend", &is_draw_friend_bone); ImGui::SameLine();
                        ImGui::Checkbox("绘制射线##Friend", &is_draw_friend_line);
                        ImGui::Checkbox("绘制血量##Friend", &is_draw_friend_health); ImGui::SameLine();
                        ImGui::Checkbox("绘制距离##Friend", &is_draw_friend_distance); ImGui::SameLine();
                        ImGui::Checkbox("绘制名称##Friend", &is_draw_friend_name);
                        ImGui::Checkbox("绘制手持##Friend", &is_draw_friend_weapon); ImGui::SameLine();
                        ImGui::Checkbox("掩体判断##Friend", &is_draw_friend_obstacles); ImGui::SameLine();
                        ImGui::Checkbox("生命值条##Friend", &is_draw_friend_healthbar);
                        ImGui::Checkbox("队友发光##Friend", &is_glow_teammate);
                        if (is_glow_teammate)
                        {
                            ImGui::SameLine();
                            ImGui::ColorEdit4("##队友发光颜色", (float*)&g_glowcolor_teammate, ImGuiColorEditFlags_NoInputs);
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "(发光功能可能导致封禁!)");
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("自瞄"))
        {
            ImGui::Checkbox("自瞄", &is_aim); ImGui::SameLine();
            ImGui::Checkbox("后坐力补偿", &is_recoil_sub);
            if (is_aim)
            {
                ImGui::Checkbox("障碍判断", &is_aim_obstacles);
                ImGui::Checkbox("自瞄锁队友", &is_aim_lock_teammate); // 新增自瞄锁队友开关
                ImGui::SliderFloat("自瞄范围", &aim_max_range, 0.0f, 1000.0f);
                ImGui::SliderFloat("自瞄平滑X", &aimSmoothX, 1.0f, 50.0f); // 新增X轴平滑
                ImGui::SliderFloat("自瞄平滑Y", &aimSmoothY, 1.0f, 50.0f); // 新增Y轴平滑
                ImGui::RadioButton("头部", &is_aim_bone, 12); ImGui::SameLine();
                ImGui::RadioButton("颈部", &is_aim_bone, 10); ImGui::SameLine();
                ImGui::RadioButton("胸部", &is_aim_bone, 8); ImGui::SameLine();
                ImGui::RadioButton("裆部", &is_aim_bone, 0); ImGui::SameLine();
                ImGui::RadioButton("左肩", &is_aim_bone, 16); ImGui::SameLine();
                ImGui::RadioButton("右肩", &is_aim_bone, 26); ImGui::SameLine();
                ImGui::RadioButton("左膝", &is_aim_bone, 46); ImGui::SameLine();
                ImGui::RadioButton("右膝", &is_aim_bone, 52);
                ImGui::RadioButton("左脚", &is_aim_bone, 48); ImGui::SameLine();
                ImGui::RadioButton("右脚", &is_aim_bone, 54);
                ImGui::RadioButton("鼠标侧键4", &is_aim_key, VK_XBUTTON1); ImGui::SameLine();
                ImGui::RadioButton("鼠标侧键5", &is_aim_key, VK_XBUTTON2); ImGui::SameLine();
                ImGui::RadioButton("鼠标左键", &is_aim_key, VK_LBUTTON); ImGui::SameLine();
                ImGui::RadioButton("鼠标右键", &is_aim_key, VK_RBUTTON);
            }
            ImGui::EndTabItem();
        }
        // 新增视觉设置TabItem
        if (ImGui::BeginTabItem("功能"))
        {
            ImGui::Checkbox("FOV修改", &is_fov);
            if (is_fov)
                ImGui::SliderFloat("FOV", &fov, 60.0f, 120.0f);
            ImGui::Checkbox("屏蔽闪光", &is_enable_flash);
            // --- 水印功能 ---
            ImGui::Separator();
            ImGui::Text("水印");
            ImGui::Checkbox("Show Watermark", &watermark);
            static const char* watermarkLocations[] = { "Top-Left", "Top-Right", "Bottom-Left", "Bottom-Right", "Custom" };
            int comboLocation = Visuals::g_watermark.m_settings.m_watermarkLocation;
            if (comboLocation < -1 || comboLocation > 3) comboLocation = 1;
            int comboIndex = (comboLocation == -1) ? 4 : comboLocation;
            if (ImGui::Combo("Watermark Position", &comboIndex, watermarkLocations, IM_ARRAYSIZE(watermarkLocations))) {
                Visuals::g_watermark.m_settings.m_watermarkLocation = (comboIndex == 4) ? -1 : comboIndex;
            }
            ImGui::Checkbox("Show Nickname", &Visuals::g_watermark.m_settings.m_watermarkShowNickname);
            ImGui::Checkbox("Show Time", &Visuals::g_watermark.m_settings.m_watermarkShowTime);
            ImGui::Checkbox("Show FPS", &Visuals::g_watermark.m_settings.m_watermarkShowFPS);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("设置区域"))
        {
            if (ImGui::TreeNode("绘制项目"))
            {
                if (ImGui::TreeNode("圆角"))
                {
                    ImGui::SliderFloat("矩形圆角", &box_yj, 0.0f, 10.0f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("粗细"))
                {
                    ImGui::SliderFloat("圆形粗细", &Circle_cx, 1.0f, 10.0f);
                    ImGui::SliderFloat("圆形最小半径", &Circle_min, 1.0f, 50.0f);
                    ImGui::SliderFloat("圆形最大半径", &Circle_max, 50.0f, 100.0f);
                    ImGui::SliderFloat("射线粗细", &line_cx, 1.0f, 10.0f);
                    ImGui::SliderFloat("骨骼粗细", &bone_cx, 1.0f, 10.0f);
                    ImGui::SliderFloat("矩形粗细", &box_cx, 1.0f, 10.0f);
                    ImGui::SliderFloat("文本大小", &text_cx, 8.0f, 50.0f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("颜色"))
                {
                    ImGui::ColorEdit3("射线颜色", (float*)&line_color);
                    ImGui::ColorEdit3("骨骼颜色", (float*)&bone_color);
                    ImGui::ColorEdit3("圆形颜色", (float*)&Circle_color);
                    ImGui::ColorEdit3("矩形颜色", (float*)&box_color);
                    ImGui::ColorEdit3("文本颜色", (float*)&text_color);
                    ImGui::ColorEdit4("文本描边颜色", (float*)&text_outline_color);
                    ImGui::SliderFloat("文本描边粗细", &text_outline_thickness, 0.5f, 3.0f);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("风格"))
            {
                ImGui::SliderFloat("界面圆角", &round_, 0.0f, 12.0f);
                ImGui::SliderFloat("背景透明度", &style.Alpha, 0.1f, 1.0f);
                // 这里创建一个下拉列表以便选择窗体风格
                if (ImGui::Combo("颜色", &style_idx, "白色\0蓝色\0紫色\0"))
                {
                    // 这里通过switch判断风格选择并执行相应操作
                    switch (style_idx)
                    {
                    case 0:
                        ImGui::StyleColorsLight();
                        style.Colors[ImGuiCol_Header] = ImColor(36, 54, 74, 79);
                        break;
                    case 1:
                        ImGui::StyleColorsDark();
                        break;
                    case 2:
                        ImGui::StyleColorsClassic();
                        break;
                    }
                }
                ImGui::ColorEdit3("字体颜色", (float*)&style.Colors[0]);
                ImGui::TreePop();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("调试"))
        {
            ImGui::Text("进程ID：%d", Pid);
            //ImGui::Text("窗口句柄：%d", g_hWnd);
            ImGui::Text("分辨率：%dx%d", g_width, g_height);
            ImGui::Text("Client 基址：0x%llX", client);
            ImGui::Text("Server 基址：0x%llX", server);
            ImGui::Text("Engine2 基址：0x%llX", engine2);
            ImGui::Text("昵称：%s", My_Name.c_str());
            ImGui::Text("自身坐标：%.2f\t%.2f\t%.2f", My_Pos.x, My_Pos.y, My_Pos.z);
            ImGui::Text("对象坐标：%.2f\t%.2f\t%.2f", Enemy_Pos.x, Enemy_Pos.y, Enemy_Pos.z);
            ImGui::Text("自瞄对象：0x%llX", Aim_Address);
            ImGui::Text("自瞄坐标：%.2f\t%.2f\t%.2f", Aim_Pos.x, Aim_Pos.y, Aim_Pos.z);
            ImGui::Text("压枪偏移量：%.2f\t%.2f", recoil_offset.x, recoil_offset.y);
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();

    ImGui::NewLine();
    ImGui::Separator();

    if (ImGui::Button("退出"))
    {
        HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Unload, NULL, 0, NULL);
        if (threadHandle != NULL) {
            CloseHandle(threadHandle); // 仅在线程创建成功时关闭句柄
        }
    }

    ImGui::Text("Framerate：%.1f FPS", io.Framerate);
    ImGui::End();

    aim_max = aim_max_range; // 重置瞄准最大距离为用户设置值
    Aim_Address = 0; // 重置瞄准地址
    Aim_Pos = { -1.0, -1.0, -1.0 }; // 重置瞄准目标

    DWORD64 Matrix_Address = client + cs2_dumper::offsets::client_dll::dwViewMatrix;
    float Matrix[16];
    ReadMatrixArray(Matrix_Address, Matrix);

    My_Address = ReadLong(client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
    My_Pos.x = ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin + 0x0);
    My_Pos.y = ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin + 0x4);
    My_Pos.z = ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin + 0x8);
    int My_Team = ReadInt(My_Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
    My_fFlag = ReadUint(My_Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);
    My_flashDuration = ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_flFlashDuration);
    DWORD64 My_Controller = ReadLong(client + cs2_dumper::offsets::client_dll::dwLocalPlayerController);
    My_Name = ReadString(My_Controller + cs2_dumper::schemas::client_dll::CBasePlayerController::m_iszPlayerName, 0x80);

    DWORD64 Entity_Address = ReadLong(client + cs2_dumper::offsets::client_dll::dwEntityList);

    for (size_t i = 1; i < 1024; i++)
    {
        DWORD64 List_Entry = ReadLong(Entity_Address + 0x8 * ((i & 0x7FFF) >> 0x9) + 0x10);
        if (!List_Entry)
            continue;
        DWORD64 Player_Entity = ReadLong(List_Entry + 0x78 * (i & 0x1FF));

        DWORD64 Player_Pawn = ReadLong(Player_Entity + cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
        DWORD64 List_Entry2 = ReadLong(Entity_Address + 0x8 * ((Player_Pawn & 0x7FFF) >> 0x9) + 0x10);
        DWORD64 Enemy_Address = ReadLong(List_Entry2 + 0x78 * (Player_Pawn & 0x1FF));
        if (Enemy_Address != 0 && Enemy_Address != My_Address)
        {
            // 加入实体人物数组 Enemy_Address
            Bone Enemy_Bone = GetAllBonePos(Enemy_Address, Matrix, g_width, g_height);
            int Enemy_Team = ReadInt(Enemy_Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
            if (Enemy_Team != 2 && Enemy_Team != 3) // 判断敌人阵营是否读取成功
                continue;
            if (!is_draw_friend && My_Team == Enemy_Team) // 判断是否绘制队友
                continue;
            bool is_teammate = (My_Team == Enemy_Team);
            // 修复：自瞄锁队友只影响自瞄目标，不影响绘制
            bool skip_aim = (!is_aim_lock_teammate && is_aim && is_teammate);

            // 获取生命值
            int Enemy_Health = ReadInt(Enemy_Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
            if (Enemy_Health <= 0)
                continue;
            // 应用边缘发光效果 - 传入是否为队友和生命值参数
            ApplyGlowEffect(Enemy_Address, is_teammate, Enemy_Health);

            // 判断掩体状态和颜色设置
            bool is_behind_cover = false;
            ImColor current_box_color = box_color;
            
            // 根据队友状态选择绘制选项(新版本 - 使用三元操作符精简代码)
            bool should_draw_box = is_teammate ? is_draw_friend_box : is_draw_box;
            bool should_draw_bone = is_teammate ? is_draw_friend_bone : is_draw_bone;
            bool should_draw_line = is_teammate ? is_draw_friend_line : is_draw_line;
            bool should_draw_name = is_teammate ? is_draw_friend_name : is_draw_name;
            bool should_draw_health = is_teammate ? is_draw_friend_health : is_draw_health;
            bool should_draw_distance = is_teammate ? is_draw_friend_distance : is_draw_distance;
            bool should_draw_weapon = is_teammate ? is_draw_friend_weapon : is_draw_weapon;
            bool should_draw_obstacles = is_teammate ? is_draw_friend_obstacles : is_draw_obstacles;
            
            if (should_draw_obstacles) // 掩体判断逻辑
            {
                int Enemy_Spotted = (int)ReadLong(Enemy_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask);
                int My_Spotted = (int)ReadLong(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask);
                
                // 判断是否在掩体后方
                is_behind_cover = (My_Spotted & (1 << Enemy_Address)) == 0 && (Enemy_Spotted & (1 << My_Address)) == 0;
                
                // 只设置方框颜色，不再设置文本颜色
                current_box_color = is_behind_cover ? ImColor(0, 255, 0, 210) : ImColor(255, 0, 0, 210);
            }
            
            DWORD64 Enemy_Current_Weapon = ReadLong(Enemy_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_pClippingWeapon);
            if (!Enemy_Current_Weapon)
                continue;
            short Enemy_Current_Weapon_Index = ReadShort(Enemy_Current_Weapon + cs2_dumper::schemas::client_dll::C_EconEntity::m_AttributeManager + cs2_dumper::schemas::client_dll::C_AttributeContainer::m_Item + cs2_dumper::schemas::client_dll::C_EconItemView::m_iItemDefinitionIndex);
            string Enemy_Weapon_Name = GetWeaponName(Enemy_Current_Weapon_Index);
            Enemy_Pos.x = ReadFloat(Enemy_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin + 0x0);
            Enemy_Pos.y = ReadFloat(Enemy_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin + 0x4);
            Enemy_Pos.z = ReadFloat(Enemy_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin + 0x8);
            if (Enemy_Pos.x == -1.0 && Enemy_Pos.y == -1.0 && Enemy_Pos.z == -1.0)
                continue;
            Box Enemy_Box = WorldToScreen_Source_Box(Enemy_Pos, Matrix, g_width, g_height);
            if (Enemy_Box.x == -1.0 && Enemy_Box.y == -1.0 && Enemy_Box.width == -1.0 && Enemy_Box.height == -1.0)
                continue;
            float Circle_size = (float)Enemy_Box.width / 10 < Circle_min ? Circle_min : (float)Enemy_Box.width / 10 > Circle_max ? Circle_max : (float)Enemy_Box.width / 10;
            double Distance = Vector3D::CalculateDistance(My_Pos, Enemy_Pos) / 30.0;
            string Player_Name = ReadString(Player_Entity + cs2_dumper::schemas::client_dll::CBasePlayerController::m_iszPlayerName, 0x80);
            if (Player_Name != "" && Player_Name != My_Name)
            {
                // 加入玩家名称数组 Player_Name
                if (is_aim && !skip_aim) // 自瞄功能
                {
                    if (is_aim_obstacles) // 障碍判断
                    {
                        int Enemy_Spotted = (int)ReadLong(Enemy_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask);
                        int My_Spotted = (int)ReadLong(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask);
                        if ((My_Spotted & (1 << Enemy_Address)) == 1 || (Enemy_Spotted & (1 << My_Address)) == 1) // 双方有一方可见则继续
                        {
                            Vector2D My_Center_2D = { g_width / 2.0f, g_height / 2.0f };
                            Vector2D Enemy_Pos_2D = Enemy_Bone.head_2D;
                            double tmp_distance = Vector2D::CalculateDistance(My_Center_2D, Enemy_Pos_2D);
                            if (tmp_distance < aim_max)
                            {
                                aim_max = (float)tmp_distance;
                                Aim_Address = Enemy_Address;
                            }
                        }
                    }
                    else
                    {
                        Vector2D My_Center_2D = { g_width / 2.0f, g_height / 2.0f };
                        Vector2D Enemy_Pos_2D = Enemy_Bone.head_2D;
                        double tmp_distance = Vector2D::CalculateDistance(My_Center_2D, Enemy_Pos_2D);
                        if (tmp_distance < aim_max)
                        {
                            aim_max = (float)tmp_distance;
                            Aim_Address = Enemy_Address;
                        }
                    }
                }
                if (is_draw) // 绘制功能
                {
                    if (should_draw_name || should_draw_health || should_draw_distance || should_draw_weapon)
                    {
                        string Info = "";
                        
                        // 绘制其他信息（除名称外）
                        if (should_draw_health) // 绘制玩家血量
                            Info += "血量：" + to_string(Enemy_Health) + "\n";
                        if (should_draw_distance) // 绘制玩家距离
                            Info += "距离：" + to_string(Distance) + "\n";
                        if (should_draw_weapon) // 绘制玩家武器
                            Info += "手持武器：" + Enemy_Weapon_Name + "\n";
                        
                        // 只有当有信息要显示时才绘制
                        if (!Info.empty()) {
                            DrawTexts(Enemy_Box.x, Enemy_Box.y + Enemy_Box.height, Info.c_str(), 
                                    text_color, font, text_cx, text_outline_color, text_outline_thickness);
                        }
                        
                        // 绘制玩家名称（安全地在方框上方显示）
                        if (should_draw_name && !Player_Name.empty()) {
                            try {
                                // 增加安全检查，确保font不为nullptr且有效
                                if (font && font->IsLoaded()) {
                                    // 使用更安全的方式计算文本宽度
                                    float name_width = Player_Name.length() * (text_cx * 0.5f); // 估算宽度
                                    float name_x = Enemy_Box.x + (Enemy_Box.width / 2.0f) - (name_width / 2.0f);
                                    float name_y = Enemy_Box.y - text_cx - 5.0f;
                                    
                                    // 绘制带描边的文本
                                    DrawTexts(name_x, name_y, Player_Name.c_str(), 
                                            text_color, font, text_cx, text_outline_color, text_outline_thickness);
                                }
                            } catch (...) {
                                // 捕获任何异常以防崩溃
                            }
                        }
                    }
                    if (should_draw_line)
                        DrawLine(Enemy_Box.x + Enemy_Box.width / 2.0, Enemy_Box.y, g_width / 2.0, 0, line_color, line_cx);
                    if (should_draw_box)
                    {
                        // 让方框颜色和骨骼一样根据掩体状态变色
                        ImColor box_display_color = should_draw_obstacles ? current_box_color : box_color;
                        float distance_based_thickness = CalculateDistanceBasedValue((float)Distance * 30.0f, 0.5f, box_cx, 100.0f, 1000.0f);
                        DrawOutlinedBox(Enemy_Box.x, Enemy_Box.y, Enemy_Box.width, Enemy_Box.height, 
                                       box_display_color, box_outline_color, distance_based_thickness, box_yj);
                    }
                    if ((is_draw_healthbar && !is_teammate) || (is_draw_friend_healthbar && is_teammate)) // 绘制生命值条
                    {
                        // 根据距离计算血条宽度和间隔
                        float health_bar_width = CalculateDistanceBasedValue((float)Distance * 30.0f, 3.0f, 5.0f, 100.0f, 1000.0f);
                        float health_bar_gap = CalculateDistanceBasedValue((float)Distance * 30.0f, 3.0f, 12.0f, 100.0f, 1000.0f);
                        
                        // 血条位置移到右侧，并使用基于距离的间隔
                        DrawHealthBar(Enemy_Box.x + Enemy_Box.width + health_bar_gap, Enemy_Box.y, 
                                     health_bar_width, Enemy_Box.height, 
                                     Enemy_Health, 100, true, 
                                     ImColor(0, 255, 0, 210), ImColor(255, 0, 0, 210), 
                                     ImColor(0, 0, 0, 255), 1.0f);
                    }
                    if (should_draw_bone)
                    {
                        // 骨骼颜色也根据掩体状态变色
                        ImColor bone_display_color = should_draw_obstacles ? current_box_color : bone_color;
                        
                        // 保存原来的骨骼颜色
                        ImColor original_bone_color = bone_color;
                        
                        // 临时修改骨骼颜色
                        bone_color = bone_display_color;
                        
                        // 使用临时颜色绘制骨骼
                        DrawAllBone(Enemy_Bone, Circle_size);
                        
                        // 恢复原来的骨骼颜色
                        bone_color = original_bone_color;
                    }
                }
            }
        }
    }

    if (is_recoil_sub) // 后坐力补偿实现（计算压枪偏移量）
    {
        static Vector2D oldPunch{};
        DWORD64 aimPunchCache_Count = ReadLong(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchCache + 0x0);
        DWORD64 aimPunchCache_Data = ReadLong(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchCache + 0x8);
        Vector2D aimPunchAngle = {
                ReadFloat(aimPunchCache_Data + (aimPunchCache_Count - 1) * sizeof(Vector3D) + 0x4),
                ReadFloat(aimPunchCache_Data + (aimPunchCache_Count - 1) * sizeof(Vector3D) + 0x0)
        };
        int shotsFired = ReadInt(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired); // 开枪次数
        if (shotsFired > 1) // 射击次数大于1
        {
            Vector2D aimViewAngles = {
                ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::v_angle + 0x4),
                ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::v_angle + 0x0)
            };
            Vector2D recoilVector = {
                aimViewAngles.x + oldPunch.x - aimPunchAngle.x * 2.0f,
                aimViewAngles.y + oldPunch.y - aimPunchAngle.y * 2.0f
            };
            if (recoilVector.y > 89.0f && recoilVector.y <= 180.0f) {
                recoilVector.y = 89.0f;
            }
            if (recoilVector.y > 180.0f) {
                recoilVector.y -= 360.0f;
            }
            if (recoilVector.y < -89.0f) {
                recoilVector.y = -89.0f;
            }
            if (recoilVector.x > 180.0f) {
                recoilVector.x -= 360.0f;
            }
            if (recoilVector.x < -180.0f) {
                recoilVector.x += 360.0f;
            }
            Vector2D O_Aim_Mouse = {
                ReadFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4),
                ReadFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0)
            };
            recoil_offset = {
                recoilVector.x - O_Aim_Mouse.x,
                recoilVector.y - O_Aim_Mouse.y
            };
        }
        else // 射击次数小于等于1
            recoil_offset = { 0.0f, 0.0f };
        oldPunch.x = aimPunchAngle.x * 2.0f;
        oldPunch.y = aimPunchAngle.y * 2.0f;
    }
    else // 后坐力补偿关闭清除后坐力补偿偏移量
        recoil_offset = { 0.0f, 0.0f };

    Vector2D Aim_Mouse_Offset = { // 修正压枪偏移量（只是压枪偏移，不自瞄）
        ReadFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4) + recoil_offset.x,
        ReadFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0) + recoil_offset.y
    };

    if (is_aim) // 自瞄实现
    {
        if (is_draw_aim_range) // 绘制自瞄范围
            DrawCircle(g_width / 2.0, g_height / 2.0, aim_max_range, White, 0, Circle_cx);
        if (Aim_Address)
        {
            Vector3D Camera_Pos = {
                    ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_vecLastClipCameraPos + 0x0),
                    ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_vecLastClipCameraPos + 0x4),
                    ReadFloat(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_vecLastClipCameraPos + 0x8)
            };
            DWORD64 GameSceneNode = ReadLong(Aim_Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
            DWORD64 BoneArrayAddress = ReadLong(GameSceneNode + 0x1F0);
            Aim_Pos.x = ReadFloat(BoneArrayAddress + 0x10 * is_aim_bone + 0x0); // is_aim_bone默认为头部骨骼索引
            Aim_Pos.y = ReadFloat(BoneArrayAddress + 0x10 * is_aim_bone + 0x4); // is_aim_bone默认为头部骨骼索引
            Aim_Pos.z = ReadFloat(BoneArrayAddress + 0x10 * is_aim_bone + 0x8); // is_aim_bone默认为头部骨骼索引
            if (Aim_Pos.x != -1.0 && Aim_Pos.y != -1.0 && Aim_Pos.z != -1.0)
            {
                Vector2D Aim_Mouse = AutoAiming(Camera_Pos, Aim_Pos);
                // 新增：自瞄平滑处理
                Vector2D currentAngles = {
                    ReadFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4), // x
                    ReadFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0)  // y
                };
                Aim_Mouse.x = currentAngles.x + (Aim_Mouse.x - currentAngles.x) / aimSmoothX;
                Aim_Mouse.y = currentAngles.y + (Aim_Mouse.y - currentAngles.y) / aimSmoothY;
                Vector2D Aim_Screem = WorldToScreen_Source(Aim_Pos, Matrix, g_width, g_height);
                if (IsKeyPressed(is_aim_key)) // 按下自瞄按键
                {
                    WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0, Aim_Mouse.y);
                    WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4, Aim_Mouse.x);
                }
                else // 没有触发自瞄按键
                {
                    WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0, Aim_Mouse_Offset.y);
                    WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4, Aim_Mouse_Offset.x);
                }
            }
            else // 读取位置失败
            {
                WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0, Aim_Mouse_Offset.y);
                WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4, Aim_Mouse_Offset.x);
            }
        }
        else // 未找到目标
        {
            WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0, Aim_Mouse_Offset.y);
            WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4, Aim_Mouse_Offset.x);
        }
    }
    else // 非自瞄模式
    {
        WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x0, Aim_Mouse_Offset.y);
        WriteFloat(client + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4, Aim_Mouse_Offset.x);
    }

    if (is_enable_flash)// && My_flashDuration > 0) // 禁止闪光实现
        WriteFloat(My_Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_flFlashBangTime, 0.0f);

    if (is_fov && !is_fov_thread_running) // 调用FOV修改线程
    {
        is_fov_thread_running = true;
        HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FovThreadFunction, NULL, 0, NULL);
        if (threadHandle != NULL) {
            CloseHandle(threadHandle); // 仅在线程创建成功时关闭句柄
        }
    }
    if (!is_fov && is_fov_thread_running) // 停止FOV修改线程
        is_fov_thread_running = false;

    if (watermark) {
        Visuals::g_watermark.ShowWatermark();
    }
}
