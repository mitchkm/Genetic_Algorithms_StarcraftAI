#include "BuildEvent.h"

using namespace BWAPI;

BuildEvent::~BuildEvent()
{
}

// Called to begin an event
bool BuildEvent::enact() {

	TilePosition targetBuildLocation = Broodwar->getBuildLocation(type, unit->getTilePosition());
	
	return unit->build(type, targetBuildLocation);
}
// Called to check if the event is resolved then 
// Resolve method is called to clean up anything
bool BuildEvent::resolved() {
	return unit->getBuildUnit();
}
bool BuildEvent::resolve(PlayerState *state) {
	state->minUsed(type.mineralPrice());
	state->gasUsed(type.gasPrice());
	
	return true;
}
