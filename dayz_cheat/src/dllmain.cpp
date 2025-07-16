// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <print>

#include "sig_scanner/sig_scanner.hpp"
#include "classes/classes.hpp"

using std::println;

#define print_ptr(ptr) println("{:s} = {:X}", #ptr, reinterpret_cast<DWORD_PTR>(ptr));
#define ptr_to_string(ptr) reinterpret_cast<DWORD_PTR>(ptr)

void setup() {
    Scanner::cache();
}

void cleanup(const LPVOID hModule, FILE* f) {
    // cleanup
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
}

DWORD WINAPI entry_point(const LPVOID hModule) {

    AllocConsole();
    FILE* f{ nullptr };
    freopen_s(&f, "CONOUT$", "w", stdout);

    setup();

    const auto world{ *Scanner::get<World**>("world") };
    print_ptr(world);

    while (!GetAsyncKeyState(VK_DELETE)) {
        Sleep(1000);
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

