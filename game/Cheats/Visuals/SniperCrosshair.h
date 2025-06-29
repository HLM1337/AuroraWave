#pragma once

#include "../../Library/ImGui/imgui.h"
#include "../../Base.h"
#include "../../Memory_LC.h"
#include "../../output/client_dll.hpp"
#include "../../output/offsets.hpp"
#include "../../UI.h"

namespace Visuals {

class SniperCrosshair {
public:
    struct Settings {
        bool m_enabled = true;        // 启用状态
        float m_size = 10.0f;         // 准星大小
        float m_thickness = 1.5f;     // 线条粗细
        ImColor m_color = ImColor(0, 255, 0, 200); // 准星颜色(默认绿色)
    };

    SniperCrosshair();
    ~SniperCrosshair() = default;

    void RenderUI();                         // 渲染UI设置界面
    void DrawCrosshair(DWORD64 localPlayer, int screenWidth, int screenHeight); // 绘制十字准星

    Settings m_settings;

private:
    bool IsSniper(short weapon_id);          // 判断武器是否为狙击枪
};

extern SniperCrosshair g_sniperCrosshair;    // 全局实例

} // namespace Visuals 