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
        //Globals::world->print_entities();
        for (const auto entity : Globals::world->get_entities()) {
            glm::vec3 new_pos{};
            Enfusion::get_screen_pos(0x00007FF68CE2B020, &new_pos.x, &entity->vis_state->pos.x);

            std::println("X:{} Y:{}", new_pos.x, new_pos.y);

            if (strcmp(entity->cls->name, "SurvivorBase") == 0) {
            }
            else {
                const auto type{ Enfusion::GetDayZInfectedType_t(entity) };
                println("NAME:{}, ADDRESS:{:X}, TYPE:{:X}", entity->cls->name, cast_ptr(entity), cast_ptr(type));
            }
        }
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