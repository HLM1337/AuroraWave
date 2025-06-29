#include "Glow.h"
#include "../../Memory_LC.h"
#include "../../output/client_dll.hpp"

namespace Visuals
{
    Glow g_glow;

    Glow::Glow() {}
    Glow::~Glow() {}

    void Glow::InitializeOffsets()
    {
        if (!m_offsets.initialized) {
            m_offsets.m_glow = cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Glow;
            m_offsets.m_glowcolor = cs2_dumper::schemas::client_dll::CGlowProperty::m_glowColorOverride;
            m_offsets.m_bEligibleForScreenHighlight = cs2_dumper::schemas::client_dll::CGlowProperty::m_bEligibleForScreenHighlight;
            m_offsets.m_bglowing = cs2_dumper::schemas::client_dll::CGlowProperty::m_bGlowing;
            m_offsets.initialized = true;
        }
    }

    void Glow::ApplyGlowEffect(DWORD64 entity, bool is_teammate, int health)
    {
        // 检查实体是否有效、是否活着
        if (entity == 0 || health <= 0) return;
        
        // 根据是否为队友选择对应的发光设置
        if (is_teammate) {
            // 如果是队友但队友发光关闭，则直接返回
            if (!m_settings.m_teammateGlow) return;
            m_currentGlowColor = m_settings.m_teammateGlowColor; // 使用队友发光颜色
        } else {
            // 如果是敌人但敌人发光关闭，则直接返回
            if (!m_settings.m_enemyGlow) return;
            m_currentGlowColor = m_settings.m_enemyGlowColor; // 使用敌人发光颜色
        }
        
        // 确保偏移量已初始化
        InitializeOffsets();
        
        // 获取边缘发光地址 (Glow属性位于C_BaseModelEntity::m_Glow)
        const uintptr_t glowaddr = entity + m_offsets.m_glow;
        
        // 判断glowaddr是否有效，防止未初始化时写入导致崩溃
        if (glowaddr < 0x10000 || ReadLong(glowaddr) == 0) return;
        
        // 将颜色转换为DWORD格式 (ABGR)
        const DWORD glowColorDWORD = (DWORD(m_currentGlowColor.w * 255) << 24) | 
                                     (DWORD(m_currentGlowColor.z * 255) << 16) | 
                                     (DWORD(m_currentGlowColor.y * 255) << 8) | 
                                      DWORD(m_currentGlowColor.x * 255);
        
        // 写入边缘发光颜色
        WriteLong(glowaddr + m_offsets.m_glowcolor, glowColorDWORD);
        
        // 启用边缘发光高亮
        WriteByte(glowaddr + m_offsets.m_bEligibleForScreenHighlight, 1);
        
        // 启用边缘发光
        WriteByte(glowaddr + m_offsets.m_bglowing, 1);
    }

    void Glow::RenderUI()
    {
        // 敌人发光设置
        ImGui::Checkbox("敌人发光", &m_settings.m_enemyGlow);
        if (m_settings.m_enemyGlow)
        {
            ImGui::SameLine();
            ImGui::ColorEdit4("##敌人发光颜色", (float*)&m_settings.m_enemyGlowColor, ImGuiColorEditFlags_NoInputs);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "(发光功能可能导致封禁!)");
        }

        // 队友发光设置
        ImGui::Checkbox("队友发光##Friend", &m_settings.m_teammateGlow);
        if (m_settings.m_teammateGlow)
        {
            ImGui::SameLine();
            ImGui::ColorEdit4("##队友发光颜色", (float*)&m_settings.m_teammateGlowColor, ImGuiColorEditFlags_NoInputs);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "(发光功能可能导致封禁!)");
        }
    }
} 