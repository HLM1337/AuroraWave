#include "Base.h"
#include "game.h"
#include "UI.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "MinHook/include/MinHook.h"

static ID3D11Device* g_pd3dDevice = nullptr; // 设备指针
static IDXGISwapChain* g_pSwapChain = nullptr; // 交换链指针
static ID3D11DeviceContext* g_pd3dContext = nullptr; // 设备上下文指针
static ID3D11RenderTargetView* view = nullptr; // 主渲染目标视图指针
static HWND g_hwnd = nullptr;
static void** vtable;
static void* origin_present = nullptr;
static void* origin_resizeBuffers = nullptr;
using Present = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffers = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
WNDPROC origin_wndProc;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

HHOOK m_hook = NULL;
UINT g_width = 0;
UINT g_height = 0;

LRESULT __stdcall WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // 处理特定的消息
    switch (uMsg) {
    case WM_MOUSEMOVE:    // 处理鼠标移动消息
    case WM_LBUTTONDOWN:  // 处理左键按下消息
    case WM_LBUTTONUP:    // 处理左键释放消息
    case WM_RBUTTONDOWN:  // 处理右键按下消息
    case WM_RBUTTONUP:    // 处理右键释放消息
    case WM_KEYDOWN:      // 处理键盘按下消息
    case WM_KEYUP:        // 处理键盘释放消息
    case WM_CHAR:         // 处理字符输入消息
        if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) { return true; } // 处理ImGui消息
        break;
    default:
        break;
    }
    // 调用原始窗口过程
    return CallWindowProc(origin_wndProc, hwnd, uMsg, wParam, lParam);
}

// present函数的hook函数，用于渲染ImGui
static bool inited = false; // 每次改变窗口大小时执行一次
static bool is_Init = false; // 游戏开始时执行一次，不再执行
HRESULT __stdcall hkPresent(IDXGISwapChain* _this, UINT a, UINT b)
{
    if (!inited)
    {
        _this->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice); // 获取设备
        g_pd3dDevice->GetImmediateContext(&g_pd3dContext); // 获取设备上下文
        DXGI_SWAP_CHAIN_DESC sd;
        _this->GetDesc(&sd);
        g_hwnd = sd.OutputWindow;
        ID3D11Texture2D* pBackBuffer{};
        HRESULT hr;
        hr = _this->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr))
            return ((Present)origin_present)(_this, a, b);
        hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &view);
        if (FAILED(hr)) {
            pBackBuffer->Release();
            return ((Present)origin_present)(_this, a, b);
        }
        pBackBuffer->Release();
        if (!is_Init)
        {
            // 注册窗口过程
            origin_wndProc = (WNDPROC)SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
            // 设置 Dear ImGui 上下文
            IMGUI_CHECKVERSION(); // 检查 Dear ImGui 版本，确保版本一致性
            ImGui::CreateContext(); // 创建一个新的 ImGui 上下文
            ImGuiIO& io = ImGui::GetIO(); // 获取 ImGui 输入/输出设置对象（ImGuiIO），并确保它被使用
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // 启用键盘控制
            //io.IniFilename = nullptr; // 不让 ImGui 保存配置文件
            io.LogFilename = nullptr; // 不让 ImGui 保存日志文件
            // 设置 ImGui 主题样式
            ImGui::StyleColorsDark();
            // 初始化Win32实现
            ImGui_ImplWin32_Init(g_hwnd);
            // 载入中文字体
            ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
            is_Init = true; // 设置为true则不再执行
        }
        // 初始化DX11实现
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);
        inited = true; // 设置为true则窗口大小改变时改为false
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI();

    ImGui::EndFrame();
    ImGui::Render();
    g_pd3dContext->OMSetRenderTargets(1, &view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return ((Present)origin_present)(_this, a, b);
}

// resizeBuffers函数的hook函数，用于重置ImGui的渲染目标
HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    g_width = Width; // 更新窗口大小
    g_height = Height;
    if (g_pd3dDevice && view)
    {
        g_pd3dDevice->Release(); // 释放旧的设备
        g_pd3dDevice = nullptr; // 清空设备指针
        view->Release(); // 释放旧的渲染目标视图
        view = nullptr; // 清空渲染目标视图指针
        ImGui_ImplDX11_Shutdown(); // 关闭ImGui DX11实现

        inited = false; // 重置初始化状态
    }
    return ((ResizeBuffers)origin_resizeBuffers)(This, BufferCount, Width, Height, NewFormat, SwapChainFlags); // 调用原来的resizeBuffers函数
}

// Hook 回调
LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(m_hook, code, wParam, lParam);
}

void StartHook()
{
    const unsigned level_count = 2;
    D3D_FEATURE_LEVEL levels[level_count] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = FindWindowA(NULL, "Counter-Strike 2");
    sd.SampleDesc.Count = 4; // 4x MSAA
    sd.SampleDesc.Quality = 0; // 需检测支持情况
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    auto hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        levels,
        level_count,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        nullptr,
        nullptr);
    if (g_pSwapChain) {
        RECT rect;
        GetClientRect(sd.OutputWindow, &rect);
        g_width = rect.right - rect.left;
        g_height = rect.bottom - rect.top;
        auto vtable_ptr = (void***)(g_pSwapChain);
        vtable = *vtable_ptr;
        MH_Initialize();
        MH_CreateHook(vtable[8], hkPresent, &origin_present);
        MH_EnableHook(vtable[8]);
        MH_CreateHook(vtable[13], hkResizeBuffers, &origin_resizeBuffers);
        MH_EnableHook(vtable[13]);
        g_pd3dDevice->Release();
        g_pSwapChain->Release();
    }
    return ;
}

// 卸载 DX11 Hook
void UnloadHook() {
    // 释放 ImGui 相关的资源
    if (g_pSwapChain) {
        // 恢复 SwapChain 的 vtable
        if (vtable) {
            // 恢复原来的 Present 和 ResizeBuffers 函数指针
            MH_DisableHook(vtable[8]); // 还原原始的 Present 函数
            MH_RemoveHook(vtable[8]);
            MH_DisableHook(vtable[13]); // 还原原始的 ResizeBuffers 函数
            MH_RemoveHook(vtable[13]);
        }
        // 释放 ImGui 相关的资源
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        // 如果存在主渲染目标视图，需要释放
        if (view) {
            view->Release();
            view = nullptr;
        }
        // 释放设备上下文
        if (g_pd3dContext) {
            g_pd3dContext->Release();
            g_pd3dContext = nullptr;
        }
        // 释放设备
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = nullptr;
        }
        // 清空交换链指针
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    // 还原 WndProc
    if (origin_wndProc) {
        SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)origin_wndProc);
        origin_wndProc = nullptr; // 清空原来备份的WndProc指针
    }
    // 清空其他变量
    g_hwnd = nullptr;
    g_width = 0;
    g_height = 0;
    vtable = nullptr;
    origin_present = nullptr;
    origin_resizeBuffers = nullptr;
}