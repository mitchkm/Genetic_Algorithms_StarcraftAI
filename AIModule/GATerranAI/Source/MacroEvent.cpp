#include "MacroEvent.h"

MacroEvent::MacroEvent() {
	
	}

bool MacroEvent::enact()
{
	return true;
}

bool MacroEvent::resolved()
{
	return true;
}

bool MacroEvent::resolve(PlayerState *state)
{
	return true;
}

	