// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <print>

#include "sig_scanner/sig_scanner.hpp"
#include "classes/classes.hpp"
#include "hooks/hooks.hpp"
#include "enfusion/enfusion.hpp"
#include "utility/utility.hpp"
#include "globals/globals.hpp"
#include "features/aimbot/aimbot.hpp"

using std::println;

void setup() {
    Scanner::cache();
    Globals::cache();
    Enfusion::cache();
    Hooks::setup();
}

void cleanup(const LPVOID hModule, FILE* f) {
    // cleanup
    Hooks::disable();
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
}

DWORD WINAPI entry_point(const LPVOID hModule) {

    AllocConsole();
    FILE* f{ nullptr };
    freopen_s(&f, "CONOUT$", "w", stdout);

    setup();


    while (!GetAsyncKeyState(VK_DELETE)) { 
        if (GetAsyncKeyState(VK_XBUTTON2)) {
            Aimbot::run();
        }
    }
    
    cleanup(hModule, f);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        const HANDLE handle = CreateThread(
            nullptr,
            0,
            entry_point,
            hModule,
            0,
            nullptr
        );

        if (handle) {
            CloseHandle(handle);
        }
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}