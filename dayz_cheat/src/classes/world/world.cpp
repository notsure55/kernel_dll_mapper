#include "world.hpp"
#include "../../ui/toggles/toggles.hpp"
#include "../../enfusion/enfusion.hpp"
#include "../../globals/globals.hpp"

std::vector<Entity*> World::get_items() {
    std::vector<Entity*> items;
    const auto start{ reinterpret_cast<DWORD_PTR>(this->item_list) };
    size_t count{ 0 };
    const size_t size = this->item_count - 1;
    do {
        const auto in_render{ *reinterpret_cast<UINT8*>(start + 0x4 * count) };
        if (in_render == (UINT8)1) {
            const auto entity_value{ *reinterpret_cast<ULONGLONG*>(start + 0x4 * count + 0x8) };
            if (entity_value != NULL && entity_value > 0x1FFFFFFFF && entity_value < 0xFFFFFFFFF) {
                const auto item{ reinterpret_cast<Entity*>(entity_value) };
                if (item == nullptr) { count++; continue; };
                if (item->cls == nullptr) { count++;  continue; };
                if (item->cls->name == nullptr) { count++; continue; };
                if (item->get_type() == Type::INVALID) { count++; continue; };

                items.push_back(item);
                count += 5;
            }
        }
        else if (in_render == (UINT8)2) {
            // not in render
        }
        else {
        }
        count++;
    } while (size > items.size());

    return items;
}

std::vector<Entity*> World::get_entities() {
    std::vector<Entity*> entities;
    Globals::local_player = Enfusion::get_player();
    for (size_t i{ 0 }; i < this->entity_count; ++i) {
        const auto entity{ this->entity_list[i] };
        if (entity == nullptr) continue;
        if (entity->cls == nullptr) continue;
        if (entity->cls->name == nullptr) continue;
        if (entity->get_type() == Type::INVALID) continue;
        if (entity == Globals::local_player) { continue; }

        entities.push_back(entity);
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
        std::println("{:X}, {}", cast_ptr(item), item->cls->name);
    }
    std::println("**** DYNAMIC ENTITIES ****");
    for (const auto ent : entities) {
        std::println("{:X}, {}", cast_ptr(ent), ent->cls->name);
    }
    std::println("{:X}", cast_ptr(Globals::local_player));
}
