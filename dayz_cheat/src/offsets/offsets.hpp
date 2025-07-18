#pragma once
#include <Windows.h>

namespace Offsets {
	namespace Entity {
		constexpr DWORD VIS_STATE{ 0x1C8 };
		constexpr DWORD VIS_STATE_POS{ 0x2C };
	}
	namespace Skeletons {
		constexpr const char* HEAD{ "Head" };
		constexpr const char* SPINE2{ "Spine2" };
	}
}

namespace Constants {
	constexpr const char* BONE_ARRAY[] = {
		// head neck
		"Head", // 0
		"Neck", // 1 
		// torse / pelvis
		"RightArm",	// 7 	
		"RightForeArm", // 8
		"RightHand", // 9
        "RightForeArm", // 8
		"RightArm",	// 7
		"Neck",
		// left arm
		"LeftArm", // 11
		"LeftForeArm", // 12
        "LeftHand", // 13
		"LeftForeArm", // 12
		"LeftArm", // 11
		"Neck",
		"Spine3",
		"Spine1", // 4
		"Pelvis", // 5
		// left leg
        "LeftUpLeg", // 14
        "LeftLeg", // 15
        "LeftFoot", // 16
		"LeftLeg",
		"LeftUpLeg",
		"Pelvis",
        "RightUpLeg", // 17
		"RightLeg", // 18
		"RightFoot", // 19
	};
}
