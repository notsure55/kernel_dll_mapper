#pragma once
#include "../classes/classes.hpp"

namespace Enfusion {
	// types
	using GetPlayer_t = Player * (*)();
	using FindEntityById_t = Entity * (*)(LONGLONG, int);
	using GetDayZInfectedType_t = ULONGLONG* (*)(Entity*);
	using GetScreenPos_t = float* (__fastcall*)(ULONGLONG, float*, float*);

	void cache();

	// functions
	Player* get_player();

	Entity* find_ent_by_id(int id);

	ULONGLONG* get_dayz_infected_type(Entity* infected);

	float* get_screen_pos(ULONGLONG idk, float* out, float* in);
}