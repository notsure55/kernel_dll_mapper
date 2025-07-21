#pragma once

#include "../entity/entity.hpp"

class Camera {
public:
    byte padding[0x8];
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
