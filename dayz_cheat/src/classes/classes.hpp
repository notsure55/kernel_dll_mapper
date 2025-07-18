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


    INVALID = 999,
};

class Entity {
public:
    byte padding[0x8];
    EntityClass* cls; // 0x8
    byte padding2[0x170];
    EntityType* type; // 0x180
    byte padding1[0x48];
    EntityVisualState* vis_state; // 0x1C8
    // 1D0

    Type get_type() const {
        const auto type{ reinterpret_cast<const char*>(cast_ptr(this->type->heap) + 0x10) };

        if (type == nullptr) {
            return Type::INVALID;
        }

        if (strstr(type,"gear") != nullptr) {
            return Type::GEAR;
        }
        else if (strstr(type, "characters") != nullptr) {
            return Type::CLOTHING;
        }
        else if (strstr(type, "weapons") != nullptr) {
            return Type::WEAPONS;
        }
        else {
            return Type::DEFAULT;
        }
    }

    glm::vec3& get_pos() const {
        auto& pos{ *reinterpret_cast<glm::vec3*>(*reinterpret_cast<DWORD_PTR*>(
            cast_ptr(this) + Offsets::Entity::VIS_STATE) + Offsets::Entity::VIS_STATE_POS) };

        return pos;
    }
};

struct Mat4x4 {
    float v[4][4];
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

    std::vector<Entity*> get_items() {
        std::vector<Entity*> items;
        const auto start{ reinterpret_cast<DWORD_PTR>(this->item_list) };
        size_t count{ 0 };
        while (this->item_count != items.size()) {
            const auto in_render{ *reinterpret_cast<int*>(start + 0x4 * count) };
            if (in_render == 1) {
                const auto entity_value{ *reinterpret_cast<ULONGLONG*>(start + 0x4 * count + 0x8) };
                if (entity_value != NULL && entity_value > 0x10000 && entity_value < 0xFFFFFFFFF) {
                    const auto item{ *reinterpret_cast<Entity**>(start + 0x4 * count + 0x8) };

                    if (item == nullptr) { count++; continue; };
                    if (item->cls == nullptr) { count++;  continue; };
                    if (item->cls->name == nullptr) { count++; continue;  };
                    if (item->get_type() == Type::INVALID) { count++; continue; };

                    items.push_back(item);
                    count += 5;
                }
            }
            else if (in_render == 2) {
                // not in render
            }
            count++;
        }

        return items;
    }

    std::vector<Entity*> get_entities() {
        std::vector<Entity*> entities;
        for (size_t i{ 0 }; i < this->entity_count; ++i) {
            if (this->entity_list[i] == nullptr) continue;
            if (this->entity_list[i]->cls == nullptr) continue;
            if (this->entity_list[i]->cls->name == nullptr) continue;
            if (this->entity_list[i]->get_type() == Type::INVALID) continue;
            entities.push_back(this->entity_list[i]);
        }

        return entities;
    }

    

    std::vector<Entity*> get_all_entities() {
        auto entities{ this->get_entities() };
        const auto items{ this->get_items() };

        entities.insert(entities.end(), items.begin(), items.end());

        return entities;
    }

    void print_entities() {
        const auto items{ this->get_items() };
        const auto entities{ this->get_entities() };
        std::println("**** ITEMS ****");
        for (const auto item : items) {
            std::println("{}", item->cls->name);
        }
        std::println("**** DYNAMIC ENTITIES ****");
        for (const auto ent : entities) {
            if (strcmp(ent->cls->name, "SurvivorBase") == 0) {
            }
            std::println("{:X}", cast_ptr(ent));
        }
    }
};