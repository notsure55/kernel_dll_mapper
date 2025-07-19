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

    Type get_type() const;

    bool check_type();

    glm::vec3& get_pos() const;

    glm::vec3 get_bone_pos(const char* name);
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