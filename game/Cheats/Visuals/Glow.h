#pragma once
#include <Windows.h>
#include "../../Library/ImGui/imgui.h"

namespace Visuals
{
    struct GlowSettings {
        bool m_enemyGlow = false;    // 敌人发光开关
        bool m_teammateGlow = false; // 队友发光开关
        ImVec4 m_enemyGlowColor = ImVec4(1.0f, 0.0f, 0.0f, 0.7f);    // 敌人发光颜色 (RGBA)
        ImVec4 m_teammateGlowColor = ImVec4(0.0f, 1.0f, 0.0f, 0.7f); // 队友发光颜色 (RGBA)
    };

    class Glow {
    public:
        Glow();
        ~Glow();

        // 应用边缘发光效果
        void ApplyGlowEffect(DWORD64 entity, bool is_teammate, int health);
        
        // 在UI中渲染设置界面
        void RenderUI();

        // 获取设置
        GlowSettings& GetSettings() { return m_settings; }

    private:
        // 初始化偏移量
        void InitializeOffsets();
        
        GlowSettings m_settings;
        
        // 记录偏移量的结构体
        struct GlowOffsets {
            uintptr_t m_glow;                       // 边缘发光基址偏移
            uintptr_t m_glowcolor;                  // 边缘发光颜色偏移
            uintptr_t m_bglowing;                   // 边缘发光状态偏移
            uintptr_t m_bEligibleForScreenHighlight; // 边缘发光屏幕高亮状态偏移
            bool initialized = false;               // 是否已初始化
        } m_offsets;
        
        // 临时变量
        ImVec4 m_currentGlowColor;                  // 当前边缘发光颜色
    };

    extern Glow g_glow;
} 