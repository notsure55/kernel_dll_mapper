#include "present.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <dxgi.h>
#include <d3d11.h>
#include <Windows.h>
#include <iostream>
#include <Minhook.h>
#include "../../ui/menu/menu.hpp"
#include <print>
#include "../../ui/toggles/toggles.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Present {

    bool initialized { false };
    ID3D11RenderTargetView* mainRenderTargetView;
    ID3D11DeviceContext* context { nullptr };
    void* pDirectPresent { nullptr };
    DirectPresent_t oDirectPresent { nullptr };
    D3D11CreateDeviceAndSwapChain_t pD3D11CreateDeviceAndSwapChain { nullptr };
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    WNDPROC oWndProc;

    LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
            return true;
        }

        if (Toggles::menu) {
            switch (uMsg)
            {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MOUSEMOVE:
                case WM_MOUSEWHEEL:
                    return true; // prevent game from processing mouse
            }
        }

        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    void menu_init(IDXGISwapChain *swap_chain) {
        ID3D11Device* device;
        HWND window { nullptr };

        swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device));
        swap_chain->GetDesc(&swap_chain_desc);
        device->GetImmediateContext(&context);
        window = swap_chain_desc.OutputWindow;

        ID3D11Texture2D* pBackBuffer;
        swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
        device->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
        pBackBuffer->Release();
        oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(device, context);
    }


    HRESULT __stdcall MyDirectPresent(
        IDXGISwapChain *swapchain,
        UINT SyncInterval,
        UINT Flags
    )
    {
        if (!initialized) {
            menu_init(swapchain);
            initialized = true;
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            Toggles::menu = !Toggles::menu;
        }

        Menu::display(context, mainRenderTargetView);

        return oDirectPresent(swapchain, SyncInterval, Flags);
    }

    void setlong() {
        SetWindowLongPtr(swap_chain_desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
    }

    void init() {
        std::println("Whus Up");
        // get access to the funciton in memoryy
        const auto d3d11_hmodule = GetModuleHandle("d3d11.dll");

        const auto pD3D11CreateDeviceAndSwapChain = reinterpret_cast<D3D11CreateDeviceAndSwapChain_t>
        (GetProcAddress(d3d11_hmodule, "D3D11CreateDeviceAndSwapChain"));

        D3D_FEATURE_LEVEL feature_level;
        constexpr D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

        DXGI_RATIONAL refresh_rate;
        refresh_rate.Numerator = 60;
        refresh_rate.Denominator = 1;

        DXGI_MODE_DESC buffer_desc;
        buffer_desc.Width = 100;
        buffer_desc.Height = 100;
        buffer_desc.RefreshRate = refresh_rate;
        buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        buffer_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        buffer_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        DXGI_SAMPLE_DESC sample_desc;
        sample_desc.Count = 1;
        sample_desc.Quality = 0;

        DXGI_SWAP_CHAIN_DESC swap_chain_desc;
        swap_chain_desc.BufferDesc = buffer_desc;
        swap_chain_desc.SampleDesc = sample_desc;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = 1;
        swap_chain_desc.OutputWindow = GetForegroundWindow();
        swap_chain_desc.Windowed = 1;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        IDXGISwapChain* swap_chain;
        ID3D11Device* device;
        ID3D11DeviceContext* context;

        // call in game creatdevice for our dummy device
        if (pD3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            feature_levels,
            1,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &swap_chain,
            &device,
            &feature_level,
            &context
            )< 0)
        {
            std::cout << "Failed to create swap chain" << std::endl;
        }

        void** pVTable = *reinterpret_cast<void***>(swap_chain);

        pDirectPresent = pVTable[8];
        std::cout << std::hex << reinterpret_cast<uintptr_t>(pDirectPresent) << std::endl;
        swap_chain->Release();
    }

    void setup() {

        init();

        if (!pDirectPresent) {
            std::cout << "Failed to setup swap chain!" << std::endl;
            return;
        }

        // create hook at pDirectPresent
        if (MH_CreateHook(pDirectPresent, MyDirectPresent, reinterpret_cast<void**>(&oDirectPresent)) != MH_OK)
        {
            std::cout << "Failed to create hook D3D11CreateDeviceAndSwapChain!" << std::endl;
        }
    }
}
