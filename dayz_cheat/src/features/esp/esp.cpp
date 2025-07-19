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

    bool get_bone_pos_by_i(Entity* entity, int i, ImVec2& out) {
        auto index{ Enfusion::get_bone_index_by_name(Globals::anim->anim_system, Constants::BONE_ARRAY[i]) };
        if (index < 1) { return false; };

        glm::vec3 pos;
        Enfusion::get_bone_pos(entity, index, &pos.x);

        glm::vec3 new_pos{};
        if (!Enfusion::get_screen_pos(NULL, &new_pos.x, &pos.x)) {
            return false;
        }
        else {
            out = ImVec2(new_pos.x, new_pos.y);
            return true;
        }
    }

    void skeletons() {
        for (const auto entity : Globals::world->get_entities()) {
            if (!entity->check_type()) { continue; };
            for (size_t i{ 0 }; i < std::size(Constants::BONE_ARRAY) - 1; ++i) {
                ImVec2 p1{};
                if (!get_bone_pos_by_i(entity, i, p1)) { continue; }
                ImVec2 p2{};
                if (!get_bone_pos_by_i(entity, i + 1, p2)) { continue; }
             
                ImGui::GetBackgroundDrawList()->AddLine(
                    p1,
                    p2,
                    IM_COL32(255, 0, 0, 255),
                    1.0
                );
            }
        }
    }

    void run() {
        for (const auto entity : Globals::world->get_all_entities()) {
            if (!entity->check_type()) { continue; };

			glm::vec3 new_pos{};
            if (!Enfusion::get_screen_pos(NULL, &new_pos.x, &entity->get_pos().x)) {
                continue;
            }
            
			ImGui::GetBackgroundDrawList()->AddText(
                ImVec2(new_pos.x, new_pos.y),
				IM_COL32(255, 0, 0, 255),
				entity->cls->name
			);            
		}

        skeletons();
    }
}