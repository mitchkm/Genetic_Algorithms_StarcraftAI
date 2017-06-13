#include "GASocket.h"
/*
Author: Pindrought
Date: 11/13/2015
This is the solution for the client that you should have at the end of tutorial 1.
*/

#include <string>


int GASocket::connectToGA()
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Address to be binded to our Connection socket
	int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Address = localhost (this pc)
	addr.sin_port = htons(1771); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket

	connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	if (connect(connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //If we are unable to connect...
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return 0; //Failed to Connect
	}
	recv(connection, BO, sizeof(BO), NULL); // Revieve string versiong of build order
	char* rcvd = "Recieved\n";
	send(connection, rcvd, strlen(rcvd) + 1, 0);
	recv(connection, ID, sizeof(ID), NULL);
}


GASocket::GASocket()
{
	connectToGA();

}

GASocket::~GASocket()
{
	closesocket(connection);
}

char *GASocket::getBO() {

	return BO;
}

char *GASocket::getID() {
	return ID;
}

void GASocket::sendResults(bool winner, int totalMin, int totalGas, int customScore, int unitScore, int killScore, int buildingScore, int razingScore, int elapsedTime)
{
	// This string is created in a particular order matching the one expected by the R script:
	// Win, , Total_Minerals, Total_Gas, Custom_score, Unit_Score, Kill_Score, Building_Score, Razing_Score, Elapsed_Time
	std::string res("");
	res += std::to_string(winner);
	res += ", " + std::to_string(totalMin);
	res += ", " + std::to_string(totalGas);
	res += ", " + std::to_string(customScore);
	res += ", " + std::to_string(unitScore);
	res += ", " + std::to_string(killScore);
	res += ", " + std::to_string(buildingScore);
	res += ", " + std::to_string(razingScore);
	res += ", " + std::to_string(elapsedTime);
	res += '\n';
	send(connection, res.c_str(), res.length() + 1, 0);
}

void GASocket::waitForRestart() {	
	char temp[100];
	recv(connection, temp, sizeof(temp), NULL);
}