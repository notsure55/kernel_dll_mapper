#pragma once

#include <Windows.h>
#include <MinHook.h>
#include <dxgi.h>
#include <d3d11.h>

namespace Hooks {
    void setup();
    void disable();
}
