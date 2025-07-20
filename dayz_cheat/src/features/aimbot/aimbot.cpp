#include "aimbot.hpp"
#include "../../enfusion/enfusion.hpp"
#include "../../globals/globals.hpp"
#include <Windows.h>
#include <imgui.h>

namespace Aimbot {

	glm::vec3 get_mouse_pos() {
		ImGuiIO& io{ ImGui::GetIO() };
		ImVec2 screen_size{ io.DisplaySize };
		glm::vec3 mouse_pos{ screen_size.x / 2.0f, screen_size.y / 2.0f, 0.0f };
		return mouse_pos;
	}

	void get_distance() {

	}
	
	void run() {
		glm::vec3 closest_pos{};
		float closest_distance{ 999999.9};

		for (const auto entity : Globals::world->get_entities()) {

			const auto& mouse_pos{ get_mouse_pos() };

			auto entity_pos{ (entity->check_type()) ? entity->get_bone_pos("Head") : entity->get_pos() };

			glm::vec3 pos{};
			if (!Enfusion::get_screen_pos(NULL, &pos.x, &entity_pos.x)) {
				return;
			}

			const auto difference = pos - mouse_pos;
			const auto distance = abs(difference.x) + abs(difference.y);

			if (distance < closest_distance) {
				closest_distance = distance;
				closest_pos = difference;
			}
		}

		if (closest_distance == 999999.9f) { return; }

		float smooth = 0.2f; // smooth so doesnt go buck wild on a nga
		LONG dx = static_cast<LONG>((closest_pos.x) * smooth);
		LONG dy = static_cast<LONG>((closest_pos.y) * smooth);

		INPUT input = {
			.type = INPUT_MOUSE,
			.mi = {
				.dx = dx,
				.dy = dy,
                .dwFlags = MOUSEEVENTF_MOVE,
            },
		};

		SendInput(1, &input, sizeof(INPUT));

		Sleep(1000 / 60);
	}
}