#include "TrainEvent.h"

using namespace BWAPI;

TrainEvent::~TrainEvent()
{
}

// Called to begin an event
bool TrainEvent::enact() {
	return unit->train(type);
}
// Called to check if the event is resolved then 
// Resolve method is called to clean up anything
bool TrainEvent::resolved() {
	return unit->getBuildUnit();
}
bool TrainEvent::resolve(PlayerState *state) {
	state->minUsed(type.mineralPrice());
	state->gasUsed(type.gasPrice());
	state->supplyUsed(type.supplyRequired());

	return true;
}