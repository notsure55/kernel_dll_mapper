#include "enfusion.hpp"
#define cast_ptr(ptr) reinterpret_cast<DWORD_PTR>(ptr)

namespace Enfusion {
	GetPlayer_t GetPlayer_p{ nullptr };
	GetScreenPos_t GetScreenPos_p{ nullptr };
	GetBonePos_t GetBonePos_p{ nullptr };
	GetBoneIndexByName_t GetBoneIndexByName_p{ nullptr };
	
	// caches api func pointers
	// TODO: make these SIGS
	void cache() {
		const auto base{ cast_ptr(GetModuleHandle(nullptr)) };
		GetPlayer_p = reinterpret_cast<GetPlayer_t>(base + 0x572F20);
		GetScreenPos_p = reinterpret_cast<GetScreenPos_t>(base + 0x578660);
		GetBonePos_p = reinterpret_cast<GetBonePos_t>(base + 0x8AA040);
		GetBoneIndexByName_p = reinterpret_cast<GetBoneIndexByName_t>(base + 0xC11F0);
	}

	Entity* get_player() {
		return GetPlayer_p();
	}

	bool get_screen_pos(ULONGLONG idk, float* out, float* in) {
		GetScreenPos_p(idk, out, in);
		if ((UINT32)(out[0]) == 0 && (UINT32)(out[1]) == 0 || out[2] < 0.001f) {
			return false;
		}
		else {
			return true;
		}
	}

	void get_bone_pos(Entity* entity, int index, float* out) {
		GetBonePos_p(entity, index, out);
	}

	int get_bone_index_by_name(AnimationSystem* anim_sys, const char* bone_name) {
		return GetBoneIndexByName_p(anim_sys, bone_name);
	}
}