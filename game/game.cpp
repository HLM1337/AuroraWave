// game.cpp - DirectX11 hook and ImGui overlay for Counter-Strike 2
// Author: [Your Name]
// Description: Implements DX11 Present/ResizeBuffers hooks, ImGui initialization, and resource cleanup for overlay rendering.
//
// Main responsibilities:
// - Hooking IDXGISwapChain::Present and ResizeBuffers for rendering overlays
// - Initializing and shutting down ImGui with DirectX11 and Win32
// - Managing DirectX device/context/swapchain resources
// - Handling custom window procedure for ImGui input
// - Cleaning up all hooks and resources on unload

#include "Base.h"
#include "game.h"
#include "UI.h"
#include "Library/ImGui/imgui_impl_dx11.h"
#include "Library/ImGui/imgui_impl_win32.h"
#include "Library/MinHook/include/MinHook.h"

// Global DirectX and ImGui related pointers
static ID3D11Device* g_pd3dDevice = nullptr; // Direct3D device
static IDXGISwapChain* g_pSwapChain = nullptr; // DXGI swapchain
static ID3D11DeviceContext* g_pd3dContext = nullptr; // Device context
static ID3D11RenderTargetView* view = nullptr; // Render target view
static HWND g_hwnd = nullptr; // Game window handle
static void** vtable; // Swapchain vtable pointer
static void* origin_present = nullptr; // Original Present function pointer
static void* origin_resizeBuffers = nullptr; // Original ResizeBuffers function pointer
using Present = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffers = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
WNDPROC origin_wndProc; // Original window procedure

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

HHOOK m_hook = NULL;
UINT g_width = 0;
UINT g_height = 0;

// Custom window procedure to handle ImGui input
LRESULT __stdcall WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
        if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) { return true; }
        break;
    default:
        break;
    }
    return CallWindowProc(origin_wndProc, hwnd, uMsg, wParam, lParam);
}

static bool inited = false; // ImGui DX11 initialized flag
static bool is_Init = false; // ImGui context initialized flag

// Present hook: initializes ImGui and renders overlay each frame
HRESULT __stdcall hkPresent(IDXGISwapChain* _this, UINT a, UINT b)
{
    if (!inited)
    {
        // Get device/context and window handle
        _this->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice);
        g_pd3dDevice->GetImmediateContext(&g_pd3dContext);
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
            // Hook window procedure and initialize ImGui context
            origin_wndProc = (WNDPROC)SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.LogFilename = nullptr;
            ImGui::StyleColorsDark();
            ImGui_ImplWin32_Init(g_hwnd);
            ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
            is_Init = true;
        }
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);
        inited = true;
    }

    // Start ImGui frame and render UI
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI(); // Call user-defined UI rendering

    ImGui::EndFrame();
    ImGui::Render();
    g_pd3dContext->OMSetRenderTargets(1, &view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return ((Present)origin_present)(_this, a, b);
}

// ResizeBuffers hook: cleans up resources on swapchain resize
HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    g_width = Width;
    g_height = Height;
    if (g_pd3dDevice && view)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
        view->Release();
        view = nullptr;
        ImGui_ImplDX11_Shutdown();
        inited = false;
    }
    return ((ResizeBuffers)origin_resizeBuffers)(This, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

// Dummy hook procedure (not used)
LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(m_hook, code, wParam, lParam);
}

// Initializes DirectX device/swapchain and sets up hooks
void StartHook()
{
    const unsigned level_count = 2;
    D3D_FEATURE_LEVEL levels[level_count] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = FindWindowA(NULL, "Counter-Strike 2");
    sd.SampleDesc.Count = 4;
    sd.SampleDesc.Quality = 0;
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

// Cleans up all hooks, ImGui, and DirectX resources
void UnloadHook() {
    if (g_pSwapChain) {
        if (vtable) {
            MH_DisableHook(vtable[8]);
            MH_RemoveHook(vtable[8]);
            MH_DisableHook(vtable[13]);
            MH_RemoveHook(vtable[13]);
        }
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        if (view) {
            view->Release();
            view = nullptr;
        }
        if (g_pd3dContext) {
            g_pd3dContext->Release();
            g_pd3dContext = nullptr;
        }
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = nullptr;
        }
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (origin_wndProc) {
        SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)origin_wndProc);
        origin_wndProc = nullptr;
    }
    g_hwnd = nullptr;
    g_width = 0;
    g_height = 0;
    vtable = nullptr;
    origin_present = nullptr;
    origin_resizeBuffers = nullptr;
}