#include "aimbot.hpp"
#include "../../enfusion/enfusion.hpp"
#include "../../globals/globals.hpp"
#include <Windows.h>
#include <imgui.h>
#include "../../ui/toggles/toggles.hpp"

namespace Aimbot {

	glm::vec3 get_mouse_pos() {
		ImGuiIO& io{ ImGui::GetIO() };
		ImVec2 screen_size{ io.DisplaySize };
		glm::vec3 mouse_pos{ screen_size.x / 2.0f, screen_size.y / 2.0f, 0.0f };
		return mouse_pos;
	}

	glm::vec3 calc_pos(Entity* entity) {
		const auto type{ entity->get_type() };
		if (type == Type::PLAYER || type == Type::ANIMAL || type == Type::ZOMBIE) {
			return entity->get_bone_pos("Head");
		}
		else {
			return entity->get_pos();
		}
	}

	bool prefered(Type type) {
		if (!Toggles::Aimbot::prefer_animals && type == Type::ANIMAL) {
			return false;
		}
		if (!Toggles::Aimbot::prefer_zombies && type == Type::ZOMBIE) {
			return false;
		}
		if (!Toggles::Aimbot::prefer_players && type == Type::PLAYER) {
			return false;
		}

		return true;
	}
	
	void run() {
		glm::vec3 closest_pos{};
		float closest_distance{ 999999.9f };
		Type type = Type::INVALID;

		for (const auto entity : Globals::world->get_entities()) {

			const auto mouse_pos{ get_mouse_pos() };
			// get_bone_pos("Head")
			auto entity_pos{ calc_pos(entity) };

			glm::vec3 pos{};
			if (!Enfusion::get_screen_pos(NULL, &pos.x, &entity_pos.x)) {
				continue;
			}

			const auto difference = pos - mouse_pos;
			const auto distance = abs(difference.x) + abs(difference.y);

			if (distance < closest_distance || prefered(entity->get_type()) && entity->get_type() != type) {
				closest_distance = distance;
				closest_pos = difference;
				type = entity->get_type();
			}
		}

		if (closest_distance == 999999.9f) { return; }

		LONG dx = static_cast<LONG>((closest_pos.x) * Toggles::Aimbot::smooth);
		LONG dy = static_cast<LONG>((closest_pos.y) * Toggles::Aimbot::smooth);

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