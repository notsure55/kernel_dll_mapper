#include "hooks.hpp"
#include <iostream>
#include "present/present.hpp"
#include <print>

namespace Hooks {

    void setup() {

        std::println("Setup Start!");

        if (MH_Initialize() != MH_OK) {
            std::cout << "Failed to init minhook" << std::endl;
            return;
        }

        std::println("Going into present");

        Present::setup();

        MH_EnableHook(MH_ALL_HOOKS);
    }

    void disable() {
        Present::setlong();
        MH_DisableHook(MH_ALL_HOOKS);
    }
}
