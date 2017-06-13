#include "BWAPI.h"
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
class GASocket
{

private:
	SOCKET connection;
	int connectToGA();
	char BO[1500];
	char ID[100];
public:
	GASocket();
	~GASocket();
	char *getBO();
	char *getID();
	void sendResults(bool winner, 
		int totalMin,
		int totalGas,
		int customScore,
		int unitScore,
		int killScore,
		int buildingScore,
		int razingScore,
		int elapsedTime);
	void waitForRestart();
};

