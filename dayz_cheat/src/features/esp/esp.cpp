#include "esp.hpp"
#include <glm/vec3.hpp>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <algorithm>
#include "../../globals/globals.hpp"
#include "../../enfusion/enfusion.hpp"
#include "../../ui/toggles/toggles.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS

constexpr ImVec2 empty(0.0, 0.0);

using std::println;

namespace Esp {

    ImVec2 get_bone_pos_by_i(Entity* entity, int i) {
        auto index{ Enfusion::get_bone_index_by_name(Globals::anim->anim_system, Constants::BONE_ARRAY[i]) };
        if (index == -1) { return ImVec2(0.0, 0.0); };

        glm::vec3 pos;
        Enfusion::get_bone_pos(entity, index, &pos.x);

        glm::vec3 new_pos{};
        Enfusion::get_screen_pos(NULL, &new_pos.x, &pos.x);
        if (new_pos.x == 0 || new_pos.y == 0 || new_pos.z < 0.001) {
            return ImVec2(0.0, 0.0);
        }

        return ImVec2(new_pos.x, new_pos.y);
    }

    void skeletons() {
        for (const auto entity : Globals::world->get_entities()) {
            for (size_t i{ 0 }; i < std::size(Constants::BONE_ARRAY) - 1; ++i) {

                const auto p1{ get_bone_pos_by_i(entity, i) };

                if (p1.x == 0.0 && p1.y == 0.0) {
                    continue;
                }

                const auto p2{ get_bone_pos_by_i(entity, i + 1) };

                if (p2.x == 0.0 && p2.y == 0.0) {
                    continue;
                }
             
                ImGui::GetBackgroundDrawList()->AddLine(
                    p1,
                    p2,
                    IM_COL32(255, 0, 0, 255),
                    1.0
                );
            }
        }
    }

    bool check_type(Type type) {
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

        return true;
    }

    void run() {
        for (const auto entity : Globals::world->get_all_entities()) {
            if (!check_type(entity->get_type())) { continue; };

			glm::vec3 new_pos{};
            Enfusion::get_screen_pos(NULL, &new_pos.x, &entity->get_pos().x);
            if (new_pos.x == 0 || new_pos.y == 0 || new_pos.z < 0.001) {
                continue;
            }

            ImVec2 vec{ ImVec2(new_pos.x, new_pos.y) };

			ImGui::GetBackgroundDrawList()->AddText(
				vec,
				IM_COL32(255, 0, 0, 255),
				entity->cls->name
			);

            skeletons();
		}
    }
}