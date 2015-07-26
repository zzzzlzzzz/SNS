#include "server.h"

using namespace std;

namespace SNS
{
	const int DNServer::BUFSIZE = 65000;

	DNServer::DNServer()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
			throw logic_error("DNServer: WSAStartup error");
	}

	DNServer::~DNServer()
	{
		WSACleanup();
	}

	DNServer& DNServer::getInstance()
	{
		static DNServer server;
		return server;
	}

	void DNServer::mainLoop()
	{
		SOCKET servSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (servSock == INVALID_SOCKET)
			throw logic_error("DNServer: socket error");

		SOCKADDR_IN ssin = { 0 };
		ssin.sin_family = AF_INET;
		ssin.sin_addr.s_addr = inet_addr("127.0.0.1");
		ssin.sin_port = htons(53);

		try
		{
			if (bind(servSock, reinterpret_cast<SOCKADDR*>(&ssin), sizeof(ssin)))
				throw logic_error("DNServer: bind error");

			char packet[BUFSIZE];
			while (1)
			{
				memset(packet, 0, sizeof(packet));
				SOCKADDR_IN ssinf = { 0 };
				int ssinsz = sizeof(ssinf);
				if (int rsize = recvfrom(servSock, packet, sizeof(packet), 0, reinterpret_cast<SOCKADDR*>(&ssinf), &ssinsz))
				{

				}
			}
		}
		catch (...)
		{
			closesocket(servSock);
			throw;
		}
		
		closesocket(servSock);
	}
}