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
	}
}