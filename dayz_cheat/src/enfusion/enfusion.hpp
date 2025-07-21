#pragma once
#include "../classes/classes.hpp"

namespace Enfusion {
	// types

	void cache();

	// functions
	using GetPlayer_t = Entity* (*)();
	Entity* get_player();

	using GetScreenPos_t = bool (__fastcall*)(ULONGLONG, float*, float*);
	bool get_screen_pos(ULONGLONG idk, float* out, float* in);

	using GetBonePos_t = void(*)(Entity*, int, float*);
	void get_bone_pos(Entity* entity, int index, float* out);

	using GetBoneIndexByName_t = int(*)(AnimationSystem*, const char*);
	int get_bone_index_by_name(AnimationSystem* anim_sys, const char* bone_name);
}
