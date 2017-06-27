#include "GATerranAIModule.h"
#include <iostream>
#include <fstream>
#include "Production.h"
#include "PlayerState.h"
#include "GASocket.h"

using namespace BWAPI;
using namespace Filter;

Production producer;
PlayerState state;
bool GASOCKET = true;
GASocket *gaSocket = NULL;

void GATerranAIModule::onStart()
{
	Broodwar->setLocalSpeed(0);
	Broodwar->setGUI(false);

	if (GASOCKET) {// use GA Socket
		gaSocket = new GASocket();
		producer = Production(gaSocket->getBO());
	}
	else {
		producer = Production();
	}
  state = PlayerState();

  if (GASOCKET)
	Broodwar->sendText("Testing Build Order: %s", gaSocket->getID());
  

  // Enable the UserInput flag, which allows us to control the bot and type messages.
  Broodwar->enableFlag(Flag::UserInput);

  // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
  Broodwar->enableFlag(Flag::CompleteMapInformation);

  // Set the command optimization level so that common commands can be grouped
  // and reduce the bot's APM (Actions Per Minute).
  Broodwar->setCommandOptimizationLevel(2);

}

void GATerranAIModule::onEnd(bool isWinner)
{
  if (GASOCKET) {
	  gaSocket->sendResults(isWinner,
		  Broodwar->self()->gatheredMinerals(),
		  Broodwar->self()->gatheredGas(),
		  Broodwar->self()->getCustomScore(),
		  Broodwar->self()->getUnitScore(),
		  Broodwar->self()->getKillScore(),
		  Broodwar->self()->getBuildingScore(),
		  Broodwar->self()->getRazingScore(),
		  Broodwar->elapsedTime());
	  gaSocket->waitForRestart();
  }

  Sleep(750);
  Broodwar->restartGame();
}

void GATerranAIModule::unitControl(BWAPI::Unit u) {	

	// If the unit is a worker unit
	if (u->getType().isWorker())
	{
		// if our worker is idle
		if (u->isIdle())
		{
			// Order workers carrying a resource to return them to the center,
			// otherwise find a mineral patch to harvest.
			if (u->isCarryingGas() || u->isCarryingMinerals())
			{
				u->returnCargo();
			}
			else if (!u->getPowerUp())  // The worker cannot harvest anything if it
			{                             // is carrying a powerup such as a flag
										  // Harvest from the nearest mineral patch or gas refinery
				if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
				{
					// If the call fails, then print the last error message
					Broodwar << Broodwar->getLastError() << std::endl;
				}
			} // closure: has no powerup

		} // closure: if idle

	}
}

void GATerranAIModule::onFrame()
{
  // Called once every game frame

  // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
  Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
  //Broodwar->drawTextScreen(200, 35, "Rax: %d", state.getUnits(UnitTypes::Terran_Barracks).size());
  Broodwar->drawTextScreen(200, 45, "RsrvedMin: %d", state.getReserveMin());

  // Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
    return;

  // Iterate through all the units that we own
  for (auto &u : Broodwar->self()->getUnits())
  {
    // Ignore the unit if it no longer exists
    // Make sure to include this block when handling any Unit pointer!
    if ( !u->exists() )
      continue;

    // Ignore the unit if it has one of the following status ailments
    if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
      continue;

    // Ignore the unit if it is in one of the following states
    if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
      continue;

    // Ignore the unit if it is incomplete or busy constructing
    if ( !u->isCompleted() || u->isConstructing() )
      continue;

    // Finally make the unit do some stuff!
	
	unitControl(u);
	
	//state.updateUnit(u);
  } // closure: unit iterator

	//producer.showTask();
  
	state.update();  
	producer.predetermineTasks(&state);
	producer.update(&state);
	producer.showTask();
	  
}

void GATerranAIModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void GATerranAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void GATerranAIModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void GATerranAIModule::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void GATerranAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void GATerranAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void GATerranAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void GATerranAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void GATerranAIModule::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }

	state.updateUnit(unit);
 
}

void GATerranAIModule::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit)
		state.removeUnit(unit);
}

void GATerranAIModule::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void GATerranAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void GATerranAIModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void GATerranAIModule::onUnitComplete(BWAPI::Unit unit)
{
}
