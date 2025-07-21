#include "toggles.hpp"

namespace Toggles {
	bool menu{false};
	namespace Esp {
		bool weapons{true};
		bool clothing{true};
		bool gear{true};
		bool def{true};
		bool animals{ true };
	}
	namespace Aimbot {
		float smooth{ 0.2f };
		bool prefer_zombies{false};
		bool prefer_animals{false};
		bool prefer_players{false};
	}
}