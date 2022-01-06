#pragma once
#include "Backtrack.h"

class Entity;

namespace Resolver
{
	struct Record {

	};
	void frameStageUpdate(Entity* entity) noexcept;
	void updateAnimations(Backtrack::Record bRecord) noexcept;
}