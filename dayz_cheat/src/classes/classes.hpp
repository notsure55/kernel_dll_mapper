#pragma once
#include <Windows.h>
#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>
#include <print>

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
    byte padding[0x8];
    EntityClass* cls;
    byte padding1[0x1B8];
    EntityVisualState* vis_state;
    // 1D0
};

struct Mat4x4 {
    float v[4][4];
};

class Camera {
public:
    byte padding[0x1C8];
    Mat4x4 view_matrix;
};

class Player {
public:

    byte padding[0x25F8];
    Mat4x4 matrix;
};

class World {
public:
    byte padding[0xF48];
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
                    items.push_back(*reinterpret_cast<Entity**>(start + 0x4 * count + 0x8));
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
            entities.push_back(this->entity_list[i]);
        }

        return entities;
    }

    void print_entities() {
        const auto items{ this->get_items() };
        const auto entities{ this->get_entities() };
        std::println("**** ITEMS ****");
        for (const auto item : items) {
            if (item == nullptr) continue;
            if (item->cls == nullptr) continue;
            if (item->cls->name == nullptr) continue;

            std::println("{}", item->cls->name);
        }
        std::println("**** DYNAMIC ENTITIES ****");
        for (const auto ent : entities) {
            if (ent == nullptr) continue;
            if (ent->cls == nullptr) continue;
            if (ent->cls->name == nullptr) continue;

            if (strcmp(ent->cls->name, "SurvivorBase") == 0) {
                const auto player{ reinterpret_cast<Player*>(ent) };

                for (size_t i{ 0 }; i < 4; ++i) {
                    for (size_t j{ 0 }; j < 4; ++j) {
                        std::print("{} ", player->matrix.v[i][j]);
                    }
                }
                std::println();
            }

            std::println("{}", ent->cls->name);
        }
    }
};