#pragma once
#include "MacroEvent.h"

class PushEvent :
	public MacroEvent
{
private:
	BWAPI::UnitType type;
public:
	PushEvent(BWAPI::UnitType t)
		: type(t) {};
	~PushEvent();
	bool enact();
	bool resolved();
	bool resolve(PlayerState *state);
};

