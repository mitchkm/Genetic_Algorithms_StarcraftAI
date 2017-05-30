#pragma once
#include "MacroEvent.h"
#include "BWAPI.h"

class BuildEvent :
	public MacroEvent
{
private:
	BWAPI::Unit unit;
	BWAPI::UnitType type;
public:
	BuildEvent(BWAPI::Unit u, BWAPI::UnitType type)
		: unit(u)
		, type(type) {};
	~BuildEvent();
	bool enact();
	bool resolved();
	bool resolve(PlayerState *state);
	
};

