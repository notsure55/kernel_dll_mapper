#include "globals.hpp"
#include "../sig_scanner/sig_scanner.hpp"
#include "../utility/utility.hpp"

namespace Globals {
	World* world{ nullptr };

	void cache() {
		world = *Scanner::get<World**>("world");
		print_ptr(world);
	}
}