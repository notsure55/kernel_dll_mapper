#include "enfusion.hpp"
#define cast_ptr(ptr) reinterpret_cast<DWORD_PTR>(ptr)

namespace Enfusion {
	GetPlayer_t GetPlayer_p{ nullptr };
	FindEntityById_t FindEntityById_p{ nullptr };
	GetDayZInfectedType_t GetDayZInfectedType_p{ nullptr };
	GetScreenPos_t GetScreenPos_p{ nullptr };
	GetBoneIndex_t GetBoneIndex_p{ nullptr };
	GetHealth_t GetHealth_p{ nullptr };
	GetHealth_t GetMaxEntityValue_p{ nullptr };
	GetBonePos_t GetBonePos_p{ nullptr };
	GetBoneIndexByName_t GetBoneIndexByName_p{ nullptr };
	LookAt_t LookAt_p{ nullptr };
	GetCameraObject_t GetCameraObject_p{ nullptr };
	GetLocalYawPitchRoll_t GetLocalYawPitchRoll_p{ nullptr };
	SetAngles_t SetAngles_p{ nullptr };
	fill_out_0x188_t fill_out_0x188_p{ nullptr };
	
	// caches api func pointers
	// TODO: make these SIGS
	void cache() {
		const auto base{ cast_ptr(GetModuleHandle(nullptr)) };
		GetPlayer_p = reinterpret_cast<GetPlayer_t>(base + 0x572F20);
		FindEntityById_p = reinterpret_cast<FindEntityById_t>(base + 0x29A490);
		GetDayZInfectedType_p = reinterpret_cast<GetDayZInfectedType_t>(base + 0x4775D0);
		GetScreenPos_p = reinterpret_cast<GetScreenPos_t>(base + 0x578660);
		GetBoneIndex_p = reinterpret_cast<GetBoneIndex_t>(base + 0x433490);
		GetHealth_p = reinterpret_cast<GetHealth_t>(base + 0x8ADEA0);
		GetMaxEntityValue_p = reinterpret_cast<GetHealth_t>(base + 0x8ADF70);
		GetBonePos_p = reinterpret_cast<GetBonePos_t>(base + 0x8AA040);
		GetBoneIndexByName_p = reinterpret_cast<GetBoneIndexByName_t>(base + 0xC11F0);
		LookAt_p = reinterpret_cast<LookAt_t>(base + 0x480FA0);
		GetCameraObject_p = reinterpret_cast<GetCameraObject_t>(base + 0x4823B0);
		GetLocalYawPitchRoll_p = reinterpret_cast<GetLocalYawPitchRoll_t>(base + 0x29C2D0);
		SetAngles_p = reinterpret_cast<SetAngles_t>(base + 0x29C370);
		fill_out_0x188_p = reinterpret_cast<fill_out_0x188_t>(base + 0x3F8AC0);
	}

	Entity* get_player() {
		return GetPlayer_p();
	}

	Entity* find_ent_by_id(int id) {
		return FindEntityById_p(1, id);
	}

	ULONGLONG* get_dayz_infected_type(Entity* infected) {
		return GetDayZInfectedType_p(infected);
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

	ULONGLONG* get_bone_index(Entity* entity) {
		return GetBoneIndex_p(entity);
	}

	float get_health(Entity* entity, const char* first_string, const char* second_string) {
		if (reinterpret_cast<void*>(cast_ptr(entity) + 0x188) == nullptr) {
		}
		fill_out_0x188(entity);
		return GetHealth_p(entity, first_string, second_string);
	}

	float get_max_entity_value(Entity* entity, const char* first_string, const char* second_string) {
		return GetMaxEntityValue_p(entity, first_string, second_string);
	}

	void get_bone_pos(Entity* entity, int index, float* out) {
		GetBonePos_p(entity, index, out);
	}

	int get_bone_index_by_name(AnimationSystem* anim_sys, const char* bone_name) {
		return GetBoneIndexByName_p(anim_sys, bone_name);
	}

	void look_at(Entity* entity, float* in) {
		LookAt_p(entity, in);
	}

	CameraObject* get_camera_object() {
		return GetCameraObject_p();
	}

	glm::vec3 get_yaw_pitch_roll(float* out, StaticCamera* static_camera) {
		return *reinterpret_cast<glm::vec3*>(GetLocalYawPitchRoll_p(out, static_camera));
	}

	void set_angles(StaticCamera* camera, float* angles) {
		SetAngles_p(camera, angles);
	}

	void fill_out_0x188(Entity* entity) {
		fill_out_0x188_p(entity);
	}
}