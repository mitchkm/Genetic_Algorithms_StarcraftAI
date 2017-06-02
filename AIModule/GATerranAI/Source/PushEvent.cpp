#include "PushEvent.h"

PushEvent::~PushEvent()
{
}

// Called to begin an event
bool PushEvent::enact() {
	return true;
}
// Called to check if the event is resolved then 
// Resolve method is called to clean up anything
bool PushEvent::resolved() {
	return true;
}
bool PushEvent::resolve(PlayerState *state) {
	for (BWAPI::Unit u : state->getUnits(type)) {
		if (u->exists() && u->isIdle())
			u->attack(state->getEnemyLoc());
	}
	return true;
}
