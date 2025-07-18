#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

namespace Present {
    using D3D11CreateDeviceAndSwapChain_t = HRESULT(__stdcall*)(
        IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
        const D3D_FEATURE_LEVEL*, UINT, UINT,
        const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**,
        ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**
        );

    // are type for present hook
    using DirectPresent_t = HRESULT(__stdcall*)(
        IDXGISwapChain*,
        UINT,
        UINT
        );

    using WNDPROC = LRESULT(CALLBACK*)(
        HWND,
        UINT,
        WPARAM,
        LPARAM
        );

    void setlong();
    void setup();
}
