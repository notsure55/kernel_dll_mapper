#include "esp.hpp"
#include <glm/vec3.hpp>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <algorithm>
#include "../../globals/globals.hpp"
#include "../../enfusion/enfusion.hpp"

namespace Esp {
    void run() {
        for (const auto entity : Globals::world->get_all_entities()) {
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