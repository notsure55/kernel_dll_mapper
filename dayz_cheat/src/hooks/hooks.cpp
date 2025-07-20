#include "hooks.hpp"
#include <iostream>
#include "present/present.hpp"
#include <print>
#include "../utility/utility.hpp"

namespace Hooks {

    void setup() {

        std::println("Setup Start!");

        if (MH_Initialize() != MH_OK) {
            std::cout << "Failed to init minhook" << std::endl;
            return;
        }

        std::println("Going into present");

        Present::setup();

        // OVER WRITE BYTES AT BASE + 0x8adeb6
        /*const auto loc{reinterpret_cast<BYTE*>(cast_ptr(GetModuleHandle(nullptr)) + 0x8adeb6)};
        *loc = 0x74;*/

        MH_EnableHook(MH_ALL_HOOKS);
    }

    void disable() {
        Present::setlong();
        MH_DisableHook(MH_ALL_HOOKS);
    }
}
