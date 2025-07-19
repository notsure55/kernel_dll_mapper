#include "aimbot.hpp"
#include "../../enfusion/enfusion.hpp"
#include "../../globals/globals.hpp"
#include <Windows.h>
#include <imgui.h>

namespace Aimbot {

	ImVec2 get_mouse_pos() {
		ImGuiIO& io{ ImGui::GetIO() };
		ImVec2 screen_size{ io.DisplaySize };
		ImVec2 mouse_pos{ screen_size.x / 2.0f, screen_size.y / 2.0f };
		return mouse_pos;
	}
	
	void run() {
		glm::vec3 closest_pos{};
		float closest_distance{ 999999.9};
		static LONG last_input_x{0};
		static LONG last_input_y{0};

		for (const auto entity : Globals::world->get_entities()) {

			const auto difference = Globals::local_player->get_pos() - entity->get_pos();
			const auto distance = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;

			if (distance < closest_distance) {
				closest_distance = distance;
				closest_pos = entity->get_bone_pos("Head");
			}
		}

		if (closest_distance == 999999.9) { return; }

		const auto& mouse_pos{ get_mouse_pos() };

		glm::vec3 pos{};
		if (!Enfusion::get_screen_pos(NULL, &pos.x, &closest_pos.x)) {
			return;
		}

		float smooth = 0.2f; // smooth so doesnt go buck wild on a nga
		LONG dx = static_cast<LONG>((pos.x - mouse_pos.x) * smooth);
		LONG dy = static_cast<LONG>((pos.y - mouse_pos.y) * smooth);

		INPUT input = {
			.type = INPUT_MOUSE,
			.mi = {
				.dx = dx,
				.dy = dy,
                .dwFlags = MOUSEEVENTF_MOVE,
            },
		};

		SendInput(1, &input, sizeof(INPUT));

		// by fps maybe get global fps from func and put here instead of 60?
		Sleep(1000 / 60);
	}
}