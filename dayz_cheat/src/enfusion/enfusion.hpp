#pragma once
#include "../classes/classes.hpp"

namespace Enfusion {
	// types

	void cache();

	// functions
	using GetPlayer_t = Player * (*)();
	Player* get_player();

	using FindEntityById_t = Entity * (*)(LONGLONG, int);
	Entity* find_ent_by_id(int id);

	using GetDayZInfectedType_t = ULONGLONG * (*)(Entity*);
	ULONGLONG* get_dayz_infected_type(Entity* infected);

	using GetScreenPos_t = bool (__fastcall*)(ULONGLONG, float*, float*);
	bool get_screen_pos(ULONGLONG idk, float* out, float* in);

	using GetBoneIndex_t = ULONGLONG * (*)(Entity*);
	ULONGLONG* get_bone_object(Entity* entity);

	using GetHealth_t = ULONGLONG(*)(Entity*);
	ULONGLONG get_health(Entity* entity);

	using GetMaxHealth_t = ULONGLONG(*)(Entity*);
	ULONGLONG get_max_health(Entity* entity);

	using GetBonePos_t = void(*)(Entity*, int, float*);
	void get_bone_pos(Entity* entity, int index, float* out);

	using GetBoneIndexByName_t = int(*)(AnimationSystem*, const char*);
	int get_bone_index_by_name(AnimationSystem* anim_sys, const char* bone_name);

	using LookAt_t = void(*)(Entity*, float*);
	void look_at(Entity* entity, float* in);

	using GetCameraObject_t = CameraObject * (*)(void);
	CameraObject* get_camera_object();

	using GetLocalYawPitchRoll_t = float* (*)(float*, StaticCamera*);
	glm::vec3 get_yaw_pitch_roll(float* out, StaticCamera* static_camera);

	using SetAngles_t = void (*)(StaticCamera*, float*);
	void set_angles(StaticCamera* camera, float* angles);
}
