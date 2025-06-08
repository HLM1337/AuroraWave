#ifndef UI_H
#define UI_H

#include "base.h"
#include "ImGui/imgui.h"
#include <cmath>
#include <algorithm>

//extern HWND g_hWnd; // 窗口句柄
extern UINT g_width; // 窗口宽度
extern UINT g_height; // 窗口高度
extern int Pid; // 进程ID

// ImGui绘制函数相关声明
// clamp模板已移除，推荐直接用std::clamp

// 定义颜色变量
extern ImU32 White;
extern ImU32 Black;
extern ImU32 Red;
extern ImU32 Green;
extern ImU32 Blue;
extern ImU32 Yellow;

// ImGui绘制函数声明
// GetBackgroundDrawList 中的函数绘制在ImGui窗口的下面
// GetForegroundDrawList 中的函数绘制在ImGui窗口的上面

// 绘制文本
void DrawTexts(double x, double y, const char* text, ImU32 color, ImFont* font, double size, ImU32 outline_color = ImColor(0, 0, 0, 255), float outline_thickness = 1.0f);
// 绘制矩形
void DrawRectangle(double x, double y, double width, double height, ImU32 color = Green, float thickness = 1.0f, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);
// 绘制填充矩形
void DrawFilledRectangle(double x, double y, double width, double height, ImU32 color = ImColor(0, 255, 0, 30), float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);
// 绘制线段
void DrawLine(double x1, double y1, double x2, double y2, ImU32 color = Green, float thickness = 1.0f);
// 绘制圆形
void DrawCircle(double x, double y, float radius, ImU32 color = White, int num_segments = 0, float thickness = 1.0f);
// 绘制填充圆形
void DrawFilledCircle(ImVec2 center, float radius, ImU32 color, int num_segments = 12);

// 新增功能
// 绘制描边方框（带外框和内框的双层矩形）
void DrawOutlinedBox(double x, double y, double width, double height, ImU32 inner_color, ImU32 outer_color, float thickness = 1.0f, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);
// 绘制生命值条
void DrawHealthBar(double x, double y, double width, double height, int health, int max_health = 100, bool vertical = true, ImU32 color_full = ImColor(0, 255, 0, 210), ImU32 color_empty = ImColor(255, 0, 0, 210), ImU32 outline_color = ImColor(0, 0, 0, 255), float thickness = 1.0f);

void UI();

#endif // UI_H
