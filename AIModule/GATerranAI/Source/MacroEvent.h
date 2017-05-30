#pragma once
#include "PlayerState.h"

class MacroEvent {

public:
	MacroEvent();

	// Called to begin an event
	virtual bool enact();
	// Called to check if the event is resolved then 
	// Resolve method is called to clean up anything
	virtual bool resolved();
	virtual bool resolve(PlayerState *state);
};