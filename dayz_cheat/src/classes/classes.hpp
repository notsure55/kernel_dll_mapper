#pragma once
#include <Windows.h>

#include "world/world.hpp"

class AnimationSystem {
};

class AnimCore {
public:
    byte padding[0x8];
    AnimationSystem* anim_system;
};