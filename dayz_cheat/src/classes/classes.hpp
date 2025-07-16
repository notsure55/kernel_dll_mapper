#pragma once
#include <Windows.h>
#include <cstdint>
#include <glm/vec3.hpp>

class EntityVisualState {
public:
    byte padding[0x2C];
    glm::vec3 pos;
};

class EntityClass {
public:
    byte padding[0x10];
    char* name;
};

class Entity {
public:
    byte padding[0x310];
    EntityClass* cls;
    byte padding1[0x1B8];
    EntityVisualState* vis_state;
};

class AiAgent {
public:
    byte padding[0x40];
    Entity* entity;
};

class AiWorld {
public:
    byte padding[0x30];
    AiAgent** agents;
    byte padding1[0x4];
    UINT32 agent_count;
};

class World {
public:
    byte padding[0x30];
    AiWorld* ai_world;
};

