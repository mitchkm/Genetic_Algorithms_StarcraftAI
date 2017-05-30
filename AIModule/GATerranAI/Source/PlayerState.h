#pragma once
#include <map>
#include <vector>
#include "BWAPI.h"

typedef std::map<BWAPI::UnitType, BWAPI::Unitset> UnitMap;
class PlayerState
{
private:
	UnitMap units; 
	int minerals;
	int gas;
	int reservedMinerals;
	int reservedGas;
	int maxSupply;
	int reservedSupply;
	int supply;
	BWAPI::Unit nexus;
	BWAPI::Position enemyLoc;

public:
	PlayerState();

	void update();
	void updateUnit(BWAPI::Unit u);
	void removeUnit(BWAPI::Unit u);
	
	int reserveGas(int gas);
	int reserveMin(int min);
	int gasUsed(int gas);
	int minUsed(int min);
	bool enoughGas(int gas);
	bool enoughMin(int min);
	int getReserveMin();

	int reserveSupply(int supply);
	int supplyUsed(int supply);
	bool enoughSupply(int supply);
	int supplyLeft();

	BWAPI::Unitset getUnits(BWAPI::UnitType type);
	BWAPI::Unit getWorker();

	BWAPI::Position getEnemyLoc();
};

