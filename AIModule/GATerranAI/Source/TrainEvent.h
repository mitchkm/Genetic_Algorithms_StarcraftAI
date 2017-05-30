#pragma once
#include "MacroEvent.h"
#include "BWAPI.h"

class TrainEvent :
	public MacroEvent
{
private:
	BWAPI::Unit unit;
	BWAPI::UnitType type;

public:
	TrainEvent(BWAPI::Unit u, BWAPI::UnitType type)
		: unit(u)
		, type(type) {};
	~TrainEvent();
	bool enact();
	bool resolved();
	bool resolve(PlayerState *state);
	
};