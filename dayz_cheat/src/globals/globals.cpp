#include "globals.hpp"
#include "../sig_scanner/sig_scanner.hpp"
#include "../utility/utility.hpp"

namespace Globals {
	World* world{ nullptr };
	AnimCore* anim{ nullptr };
	Entity* local_player{ nullptr };

	void cache() {
		world = *Scanner::get<World**>("world");
		anim = *Scanner::get<AnimCore**>("anim");
	}
}