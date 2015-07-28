#include "server.h"

using namespace std;

/* 
	TODO:
		* Добавить возможность отправки ответов
*/

namespace SNS
{
	const int DNServer::BUFSIZE = 65000;

	DNServer::DNServer(const std::string& configName) : cfpars(configName)
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
			throw logic_error("DNServer: WSAStartup error");
	}

	DNServer::~DNServer()
	{
		WSACleanup();
	}

	DNServer& DNServer::getInstance(const std::string& configName)
	{
		static DNServer server(configName);
		return server;
	}

	void DNServer::mainLoop()
	{
		SOCKET servSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (servSock == INVALID_SOCKET)
			throw logic_error("DNServer: socket error");

		SOCKADDR_IN ssin = { 0 };
		ssin.sin_family = AF_INET;
		ssin.sin_addr.s_addr = inet_addr(cfpars["main"]["bindaddr"].c_str());
		ssin.sin_port = htons(stoi(cfpars["main"]["bindport"]));

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
				if (recvfrom(servSock, packet, sizeof(packet), 0, reinterpret_cast<SOCKADDR*>(&ssinf), &ssinsz) > 0)
				{
					DnsRequest req(packet);
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