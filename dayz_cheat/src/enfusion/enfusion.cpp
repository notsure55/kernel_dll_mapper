#include "enfusion.hpp"
#define cast_ptr(ptr) reinterpret_cast<DWORD_PTR>(ptr)

namespace Enfusion {
	GetPlayer_t GetPlayer_p{ nullptr };
	FindEntityById_t FindEntityById_p{ nullptr };
	GetDayZInfectedType_t GetDayZInfectedType_p{ nullptr };
	GetScreenPos_t GetScreenPos_p{ nullptr };
	
	// caches api func pointers
	void cache() {
		const auto base{ cast_ptr(GetModuleHandle(nullptr)) };
		GetPlayer_p = reinterpret_cast<GetPlayer_t>(base + 0x572F20);
		FindEntityById_p = reinterpret_cast<FindEntityById_t>(base + 0x29A490);
		GetDayZInfectedType_p = reinterpret_cast<GetDayZInfectedType_t>(base + 0x4775D0);
		GetScreenPos_p = reinterpret_cast<GetScreenPos_t>(base + 0x578660);
	}

	Player* get_player() {
		return GetPlayer_p();
	}

	Entity* find_ent_by_id(int id) {
		return FindEntityById_p(1, id);
	}

	ULONGLONG* get_dayz_infected_type(Entity* infected) {
		return GetDayZInfectedType_p(infected);
	}

	float* get_screen_pos(ULONGLONG idk, float* out, float* in) {
		return GetScreenPos_p(idk, out, in);
	}
}