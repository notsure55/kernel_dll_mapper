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

namespace Esp {
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
            if (new_pos.x == 0 || new_pos.y == 0) {
                continue;
            }

            ImVec2 vec{ ImVec2(new_pos.x, new_pos.y) };

			ImGui::GetBackgroundDrawList()->AddText(
				vec,
				IM_COL32(255, 0, 0, 255),
				entity->cls->name
			);
		}
    }
}