#include "Watermark.h"
#include <cstdio>
#include <ctime>
#include <string>
#include "../../Library/ImGui/imgui.h"

extern std::string My_Name;

namespace Visuals
{
    Watermark g_watermark;

    Watermark::Watermark() {}
    Watermark::~Watermark() {}

    void Watermark::ShowWatermark(bool* p_open)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                        ImGuiWindowFlags_NoNav;
        if (m_settings.m_watermarkLocation >= 0)
        {
            const float PAD = 10.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos;
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (m_settings.m_watermarkLocation & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (m_settings.m_watermarkLocation & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (m_settings.m_watermarkLocation & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (m_settings.m_watermarkLocation & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        else if (m_settings.m_watermarkLocation == -1)
        {
            static bool firstOpen = true;
            if (firstOpen) {
                ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver);
                firstOpen = false;
            }
        }
        ImGui::SetNextWindowBgAlpha(0.7f);
        if (ImGui::Begin("Watermark", p_open, window_flags))
        {
            char watermarkLine[256] = "";
            int offset = 0;
            offset += sprintf_s(watermarkLine + offset, sizeof(watermarkLine) - offset, "AuroraWave");
            if (m_settings.m_watermarkShowNickname && !My_Name.empty()) {
                offset += sprintf_s(watermarkLine + offset, sizeof(watermarkLine) - offset, " | %s", My_Name.c_str());
            }
            if (m_settings.m_watermarkShowTime)
            {
                time_t now = time(0);
                struct tm timeinfo;
                char timeStr[32];
                localtime_s(&timeinfo, &now);
                sprintf_s(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
                offset += sprintf_s(watermarkLine + offset, sizeof(watermarkLine) - offset, " | %s", timeStr);
            }
            if (m_settings.m_watermarkShowFPS)
            {
                offset += sprintf_s(watermarkLine + offset, sizeof(watermarkLine) - offset, " | %.1f", io.Framerate);
            }
            ImGui::Text("%s", watermarkLine);
        }
        ImGui::End();
    }
}
