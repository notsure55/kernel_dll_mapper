#pragma once
#include <Windows.h>
#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>
#include <print>
#include "../utility/utility.hpp"
#include "../offsets/offsets.hpp"

class AnimationSystem {
};

class AnimCore {
public:
    byte padding[0x8];
    AnimationSystem* anim_system;
};

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

    std::vector<Entity*> get_equipment() {
        const auto inventory_start{ *reinterpret_cast<DWORD_PTR*>(
            *reinterpret_cast<DWORD_PTR*>(this) + 0x150) + 0x8 };

        std::vector<Entity*> equipment;

        for (size_t i{ 0 }; i < 16; ++i) {
            const auto entity{ *reinterpret_cast<Entity**>(inventory_start + 0x10 * i) };
            if (entity != nullptr) {
                equipment.push_back(entity);
            }
            else {
                break;
            }
        }

        return equipment;
    }

    Entity* get_in_hand() {
        const auto in_hand{ *reinterpret_cast<Entity**>(
              *reinterpret_cast<DWORD_PTR*>(this) + 0x1B0) };

        return in_hand;
    }
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

struct Mat4x4 {
    float v[4][4];
};

class StaticCamera {
public:
};

class CameraObject {
public:
    byte padding[0x68];
    StaticCamera* static_camera;
};

class Camera {
public:
    byte padding[0x8];
    Mat4x4 view_matrix;
};

class Player {
public:
};

class World {
public:
    byte padding3[0x1B8];
    Camera* camera;
    byte padding[0xD88];
    Entity** entity_list;
    int entity_count;
    byte padding1[0x110C];
    byte* item_list;
    byte padding2[0x8];
    int item_count;

    std::vector<Entity*> get_items();

	std::vector<Entity*> get_entities();
	
	std::vector<Entity*> get_all_entities();
	
	void print_entities();
};