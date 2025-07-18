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
    bool wts(const glm::vec3& in, ImVec2& out) {
        const auto& view_matrix{ Globals::world->get_view_matrix()};
        if (!ImGui::GetCurrentContext()) { return false; };

        const float z = view_matrix.v[3][0] * in.x + view_matrix.v[3][1] * in.z + view_matrix.v[3][2] * in.y + view_matrix.v[3][3];
        if (z < 0.001f) { return false; };

        out.x = ImGui::GetIO().DisplaySize.x * 0.5f;
        out.y = ImGui::GetIO().DisplaySize.y * 0.5f;

        /*glm::vec3 temp;
        temp = in - InvViewTranslation; // inv view is a vector 3
        float x = temp.dot(InvViewRight);// inv view is a vector 3
        float y = temp.dot(InvViewUp);// inv view is a vector 3
        float z = temp.dot(InvViewForward);// inv view is a vector 3

        out.x = ViewPortMatrix.x * (1 + (x / ProjD1.x / z));// proj d1 is a float
        out.y = ViewPortMatrix.z * (1 - (y / ProjD2.z / z)); // proj d2 is a float*/

        return true;
    }

    void run() {
        for (const auto entity : Globals::world->get_all_entities()) {
            glm::vec3 new_pos{};
            Enfusion::get_screen_pos(NULL, &new_pos.x, &entity->vis_state->pos.x);

            ImVec2 vec{ ImVec2(new_pos.x, new_pos.y) };

			ImGui::GetBackgroundDrawList()->AddText(
				vec,
				IM_COL32(255, 0, 0, 255),
				entity->cls->name
			);
		}
    }
}