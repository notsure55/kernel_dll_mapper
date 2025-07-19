#pragma once
#include "../classes/classes.hpp"

namespace Globals {
	extern World* world;
	extern AnimCore* anim;
	extern Entity* local_player;

	void cache();
}