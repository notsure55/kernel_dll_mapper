#include "entity.hpp"
#include "../../ui/toggles/toggles.hpp"
#include "../../enfusion/enfusion.hpp"
#include "../../globals/globals.hpp"

std::vector<Entity*> EntityInventory::get_equipment() {
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

Entity* EntityInventory::get_in_hand() {
    const auto in_hand{ *reinterpret_cast<Entity**>(
          *reinterpret_cast<DWORD_PTR*>(this) + 0x1B0) };

    return in_hand;
}

Type Entity::get_type() const {
    const auto type{ reinterpret_cast<const char*>(cast_ptr(this->type->heap) + 0x10) };

    if (type == nullptr) {
        return Type::INVALID;
    }

    if (this->cls->name != nullptr) {
        if (strncmp(this->cls->name, "SurvivorBase", 12) == 0) {
            return Type::PLAYER;
        }
    }

    if (strstr(type, "gear") != nullptr) {
        return Type::GEAR;
    }
    else if (strstr(type, "characters") != nullptr) {
        if (strstr(type, "zombie") != nullptr) {
            return Type::ZOMBIE;
        }
        else {
            return Type::CLOTHING;
        }
    }
    else if (strstr(type, "weapons") != nullptr) {
        return Type::WEAPONS;
    }
    else if (strstr(type, "animals") != nullptr) {
        return Type::ANIMAL;
    }
    else if (strstr(type, "vehicles") != nullptr) {
        if (strstr(type, "water") != nullptr) {
            return Type::BOAT;
        }
        else {
            return Type::VEHICLE;
        }
    }
    else {
        return Type::DEFAULT;
    }
}

glm::vec3& Entity::get_pos() const {
    auto& pos{ *reinterpret_cast<glm::vec3*>(*reinterpret_cast<DWORD_PTR*>(
        cast_ptr(this) + Offsets::Entity::VIS_STATE) + Offsets::Entity::VIS_STATE_POS) };

    return pos;
}

glm::vec3 Entity::get_bone_pos(const char* name) {
    auto index{ Enfusion::get_bone_index_by_name(Globals::anim->anim_system, name) };
    if (index < 0) {
        return glm::vec3{ 0.0, 0.0, 0.0 };
    }
    glm::vec3 pos;
    Enfusion::get_bone_pos(this, index, &pos.x);

    return pos;
}

bool Entity::check_type() {

    const auto type{ this->get_type() };

    if (!Toggles::Esp::clothing && type == Type::CLOTHING) {
        return false;
    }
    if (!Toggles::Esp::gear && type == Type::GEAR) {
        return false;
    }
    if (!Toggles::Esp::weapons && type == Type::WEAPONS) {
        return false;
    }
    if (!Toggles::Esp::def && type == Type::DEFAULT) {
        return false;
    }
    if (!Toggles::Esp::animals && type == Type::ANIMAL) {
        return false;
    }
    if (type == Type::INVALID) {
        return false;
    }

    return true;
}