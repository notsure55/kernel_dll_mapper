#pragma once
#include <Windows.h>
#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>
#include <print>
#include "../../utility/utility.hpp"
#include "../../offsets/offsets.hpp"

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

class EntityType {
public:
    byte padding[0x88];
    void* heap;
};

// "gear" "characters" "weapons"
enum Type : UINT32 {
    GEAR = 1, // gear
    CLOTHING = 2, // chararcter
    WEAPONS = 3, // weapons
    DEFAULT = 4,
    ANIMAL = 5, // animals
    VEHICLE = 6, // land vehicles
    BOAT = 7, // water vehicles
    PLAYER = 8,
    ZOMBIE = 9,
    INVALID = 999,
};

class Entity;

class EntityInventory {
private:
    byte padding[0x150];
    void* equipment;
    byte padding1[0x8];
    void* body_parts;
    byte padding2[0x48];
    Entity* in_hands;
public:

    std::vector<Entity*> get_equipment();
    Entity* get_in_hand();
};

class Entity {
public:
    void** vtable;
    EntityClass* cls; // 0x8
    byte padding2[0x170];
    EntityType* type; // 0x180
    byte padding1[0x48];
    EntityVisualState* vis_state; // 0x1C8
    byte padding[0x488];
    EntityInventory* inventory;
    // 1D0

    Type get_type() const;

    bool check_type();

    glm::vec3& get_pos() const;

    glm::vec3 get_bone_pos(const char* name);

    EntityInventory* get_inv() const {
        return reinterpret_cast<EntityInventory*>(cast_ptr(this) + 0x658);
    }
};
