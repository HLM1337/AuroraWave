#pragma once
#include <string>

struct ImFont;

namespace Visuals
{
    struct WatermarkSettings {
        int m_watermarkLocation = 1; // 0:Top-Left 1:Top-Right 2:Bottom-Left 3:Bottom-Right -1:Custom
        bool m_watermarkShowNickname = true;
        bool m_watermarkShowTime = true;
        bool m_watermarkShowFPS = true;
    };

    class Watermark {
    public:
        Watermark();
        ~Watermark();
        void ShowWatermark(bool* p_open = nullptr);
        WatermarkSettings m_settings;
    };

    extern Watermark g_watermark;
}
