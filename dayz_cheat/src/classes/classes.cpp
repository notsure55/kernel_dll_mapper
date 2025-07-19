#include "classes.hpp"
#include "../globals/globals.hpp"
#include "../enfusion/enfusion.hpp"

std::vector<Entity*> World::get_items() {
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
                if (item->cls->name == nullptr) { count++; continue; };
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

std::vector<Entity*> World::get_entities() {
    std::vector<Entity*> entities;
    for (size_t i{ 0 }; i < this->entity_count; ++i) {
        if (this->entity_list[i] == nullptr) continue;
        if (this->entity_list[i]->cls == nullptr) continue;
        if (this->entity_list[i]->cls->name == nullptr) continue;
        if (this->entity_list[i]->get_type() == Type::INVALID) continue;
        if (i == 0) {
            Globals::local_player = this->entity_list[i];
        }
        else {
            entities.push_back(this->entity_list[i]);
        }
    }

    return entities;
}



std::vector<Entity*> World::get_all_entities() {
    auto entities{ this->get_entities() };
    const auto items{ this->get_items() };

    entities.insert(entities.end(), items.begin(), items.end());

    return entities;
}

void World::print_entities() {
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

Type Entity::get_type() const {
    const auto type{ reinterpret_cast<const char*>(cast_ptr(this->type->heap) + 0x10) };

    if (type == nullptr) {
        return Type::INVALID;
    }

    if (strstr(type, "gear") != nullptr) {
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