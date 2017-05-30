#include "Production.h"
#include "TrainEvent.h"
#include "BuildEvent.h"
#include <queue>
#include <vector>
#include <fstream>

using namespace BWAPI;
using namespace Filter;

enum Build {
	SCV,
	MRN,
	SPLY,
	RAX
};

std::queue<MacroEvent *> taskToDo; // start an event
std::queue<MacroEvent *> taskToRes; // resolve event
std::queue<MacroEvent *> taskToKeep; // keep events that are unresolved

std::vector<Build> buildOrder;
int buildOrderPos;

int SCVForFirstSPLY;
int SCVForRAX;
int maxRax;

Production::Production() {}

Production::Production(int firstSPLY, int rax, int mxRax) {
	SCVForFirstSPLY = firstSPLY;
	SCVForRAX = rax;
	maxRax = mxRax;
	taskToDo = std::queue<MacroEvent *>();
	taskToRes = std::queue<MacroEvent *>();
	taskToKeep = std::queue<MacroEvent *>();

	// Read Build Order from File
	std::ifstream bo;
	int num;
	bo.open("bwapi-data/read/BO.txt");
	if (bo.is_open()) {
		if (bo >> num) {
			buildOrder.resize(num);
			buildOrder.clear();
		}
		while (bo >> num) {
			buildOrder.push_back(static_cast<Build>(num));
		}
			bo.close();
	}
	buildOrderPos = 0;
}

void Production::showTask() {
	


}

void Production::predetermineTasks(PlayerState *state) {
	if (buildOrderPos >= buildOrder.size())
		return;

	switch (buildOrder.at(buildOrderPos)) {
	case SCV: 
		for (auto &c : state->getUnits(UnitTypes::Terran_Command_Center)) {
			if (c && (IsCompleted && IsIdle)(c) && state->enoughMin(UnitTypes::Terran_SCV.mineralPrice()) &&
				state->enoughSupply(UnitTypes::Terran_SCV.supplyRequired())) {
				taskToDo.push(new TrainEvent(c, UnitTypes::Terran_SCV));
				state->reserveMin(UnitTypes::Terran_SCV.mineralPrice());
				state->reserveSupply(UnitTypes::Terran_SCV.supplyRequired());
				buildOrderPos++;
			}
		}
		break;
	case MRN: 
		for (auto &b : state->getUnits(UnitTypes::Terran_Barracks)) {
			if (b && (IsCompleted && IsIdle)(b) && state->enoughMin(UnitTypes::Terran_Marine.mineralPrice()) &&
				state->enoughSupply(UnitTypes::Terran_Marine.supplyRequired())) {
				taskToDo.push(new TrainEvent(b, UnitTypes::Terran_Marine));
				state->reserveMin(UnitTypes::Terran_Marine.mineralPrice());
				state->reserveSupply(UnitTypes::Terran_Marine.supplyRequired());
				buildOrderPos++;
				break;
			}
		}
		break;
	case SPLY:
		if (state->enoughMin(UnitTypes::Terran_Supply_Depot.mineralPrice()) && state->getWorker()) {
			taskToDo.push(new BuildEvent(state->getWorker(), UnitTypes::Terran_Supply_Depot));
			state->reserveMin(UnitTypes::Terran_Supply_Depot.mineralPrice());
			buildOrderPos++;
		}
		break;
	case RAX: 
		if (state->enoughMin(UnitTypes::Terran_Barracks.mineralPrice()) && state->getWorker()) {
			taskToDo.push(new BuildEvent(state->getWorker(), UnitTypes::Terran_Barracks));
			state->reserveMin(UnitTypes::Terran_Barracks.mineralPrice());
			buildOrderPos++;
		}
		break;
	default:
		break;
	}

}

void Production::determineTasks(PlayerState *state) {
	if (state->enoughMin(UnitTypes::Terran_Supply_Depot.mineralPrice())) {
		taskToDo.push(new BuildEvent(state->getWorker(), UnitTypes::Terran_Supply_Depot));
		state->reserveMin(UnitTypes::Terran_Supply_Depot.mineralPrice());
	}

	for (auto &c : state->getUnits(UnitTypes::Terran_Command_Center)) {
		if ((IsCompleted && IsIdle)(c) && state->enoughMin(UnitTypes::Terran_SCV.mineralPrice()) &&
			state->enoughSupply(UnitTypes::Terran_SCV.supplyRequired())) {
			taskToDo.push(new TrainEvent(c, UnitTypes::Terran_SCV));
			state->reserveMin(UnitTypes::Terran_SCV.mineralPrice());
			state->reserveSupply(UnitTypes::Terran_SCV.supplyRequired());
		}
	}
}

void Production::update(PlayerState *state) {
	while (!taskToDo.empty()) {
		taskToDo.front()->enact();
		taskToRes.push(taskToDo.front());
		taskToDo.pop();
	}

	while (!taskToRes.empty()) {
		if (taskToRes.front()->resolved())
			taskToRes.front()->resolve(state);
		else
			taskToKeep.push(taskToRes.front());
		taskToRes.pop();
	}

	while (!taskToKeep.empty()) {
		taskToRes.push(taskToKeep.front());
		taskToKeep.pop();
	}
}
