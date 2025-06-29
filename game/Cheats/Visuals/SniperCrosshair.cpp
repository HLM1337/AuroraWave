#include "SniperCrosshair.h"

// 声明全局实例
namespace Visuals {
    SniperCrosshair g_sniperCrosshair;
}

// 构造函数
Visuals::SniperCrosshair::SniperCrosshair() {
    // 初始化默认设置
    m_settings.m_enabled = true;
    m_settings.m_size = 10.0f;
    m_settings.m_thickness = 1.5f;
    m_settings.m_color = ImColor(0, 255, 0, 200);
}

// 判断武器是否为狙击枪
bool Visuals::SniperCrosshair::IsSniper(short weapon_id) {
    // 判断是否为狙击枪
    return (weapon_id == 9  ||  // AWP
            weapon_id == 40 ||  // SSG_08
            weapon_id == 11 ||  // G3SG1
            weapon_id == 38);   // SCAR_20
}

// 渲染UI设置界面
void Visuals::SniperCrosshair::RenderUI() {
    if (ImGui::TreeNode("狙击枪十字准星设置")) {
        ImGui::Checkbox("启用狙击枪十字准星", &m_settings.m_enabled);
        
        if (m_settings.m_enabled) {
            ImGui::SliderFloat("准星大小", &m_settings.m_size, 5.0f, 30.0f);
            ImGui::SliderFloat("线条粗细", &m_settings.m_thickness, 0.5f, 5.0f);
            ImGui::ColorEdit4("准星颜色", (float*)&m_settings.m_color);
        }
        
        ImGui::TreePop();
    }
}

// 绘制十字准星
void Visuals::SniperCrosshair::DrawCrosshair(DWORD64 localPlayer, int screenWidth, int screenHeight) {
    // 如果功能未启用，直接返回
    if (!m_settings.m_enabled || !localPlayer)
        return;
        
    // 获取当前持有的武器
    DWORD64 currentWeapon = ReadLong(localPlayer + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_pClippingWeapon);
    if (!currentWeapon)
        return;
        
    // 获取武器ID
    short weaponIndex = ReadShort(currentWeapon + cs2_dumper::schemas::client_dll::C_EconEntity::m_AttributeManager + 
                                 cs2_dumper::schemas::client_dll::C_AttributeContainer::m_Item + 
                                 cs2_dumper::schemas::client_dll::C_EconItemView::m_iItemDefinitionIndex);
    
    // 如果不是狙击枪，不绘制
    if (!IsSniper(weaponIndex))
        return;
    
    // 中心点
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    
    // 绘制十字准星
    ImGui::GetBackgroundDrawList()->AddLine(
        ImVec2(centerX - m_settings.m_size, centerY),
        ImVec2(centerX + m_settings.m_size, centerY),
        m_settings.m_color,
        m_settings.m_thickness
    );
    
    ImGui::GetBackgroundDrawList()->AddLine(
        ImVec2(centerX, centerY - m_settings.m_size),
        ImVec2(centerX, centerY + m_settings.m_size),
        m_settings.m_color,
        m_settings.m_thickness
    );
} 