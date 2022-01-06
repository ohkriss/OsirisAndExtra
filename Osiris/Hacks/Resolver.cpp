
#include "Resolver.h"

#include "../Config.h"

#include "../SDK/UserCmd.h"
#include "../SDK/Entity.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/Client.h"

namespace Resolver
{
	void frameStageUpdate(Entity* entity) noexcept
	{
		auto activeWeapon = localPlayer->getActiveWeapon();
		if (!activeWeapon || !activeWeapon->clip())
			return;

		auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
		if (!weaponIndex)
			return;

		bool reset = config->ragebot[weaponIndex].resolver || !entity->isOtherEnemy(localPlayer.get());
		
		if (reset) {
			entity->clientSideAnimation() = true;
			
		}

	}

	void updateAnimations(Backtrack::Record bRecord) noexcept
	{

	}
}