#include "PlayerState.h"

using namespace BWAPI;

PlayerState::PlayerState()
{
	units = UnitMap();
	minerals = 0;
	gas = 0;
	reservedMinerals = 0;
	reservedGas = 0;
	maxSupply = 0;
	reservedSupply = 0;
	supply = 0;
	nexus = NULL;
	enemyLoc = Position(0, 0);

}

void PlayerState::update()
{

	minerals = Broodwar->self()->minerals();
	gas = Broodwar->self()->gas();
	maxSupply = Broodwar->self()->supplyTotal();
	supply = Broodwar->self()->supplyUsed();
	
	if (nexus && nexus->getClosestUnit(Filter::IsEnemy))
		enemyLoc = nexus->getClosestUnit(Filter::IsEnemy)->getPosition();
}

void PlayerState::updateUnit(BWAPI::Unit u)
{
	if (u) {
		if (!units[u->getType()].contains(u)) {
			units[u->getType()].insert(u);
		}
		if (!nexus && u->getType() == UnitTypes::Terran_Command_Center)
			nexus = u;
	}
	

}

void PlayerState::removeUnit(BWAPI::Unit u)
{
	if (u) {
		units[u->getType()].erase(u);
		if (nexus && u->getID() == nexus->getID()) {
			nexus = NULL;
		}
	}
	
}

int PlayerState::reserveGas(int gas)
{
	reservedGas += gas;
	return reservedGas;
}

int PlayerState::reserveMin(int min)
{
	reservedMinerals = reservedMinerals + min;
	return reservedMinerals; 
}

int PlayerState::gasUsed(int gas)
{
	reservedGas -= gas;
	return reservedGas;
}

int PlayerState::minUsed(int min)
{
	reservedMinerals = reservedMinerals - min;
	return reservedMinerals; 
}

bool PlayerState::enoughGas(int gas)
{
	return (PlayerState::gas - reservedGas) >= gas;
}

bool PlayerState::enoughMin(int min)
{
	return (minerals - reservedMinerals) >= min;
}

int PlayerState::getReserveMin() {
	return reservedMinerals;
}

int PlayerState::reserveSupply(int supply)
{
	reservedSupply += supply;
	return reservedSupply;
}

int PlayerState::supplyUsed(int supply)
{
	reservedSupply -= supply;
	return reservedSupply;
}

bool PlayerState::enoughSupply(int supply)
{
	return supplyLeft() >= supply;
}

int PlayerState::supplyLeft()
{
	return maxSupply - supply - reservedSupply;
}

BWAPI::Unitset PlayerState::getUnits(BWAPI::UnitType type)
{
	return units[type];
}

BWAPI::Unit PlayerState::getWorker() {
	if (nexus)
		return nexus->getClosestUnit(Filter::GetType == UnitTypes::Terran_SCV);
	else
		return NULL;
}

BWAPI::Position PlayerState::getEnemyLoc()
{
	return enemyLoc;
}
