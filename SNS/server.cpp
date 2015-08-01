#include "server.h"

using namespace std;

namespace SNS
{
	const int DNServer::BUFSIZE = 65000;

	DNServer::DNServer(const std::string& configName) : cfpars(configName), filter(cfpars)
	{
		try
		{
			// Здесь необходимо добавить модули для работы с различными типами записей
			if (cfpars["main"]["aenabled"] == "1")
				modules[A] = AModule(cfpars["files"]["afile"], stol(cfpars["main"]["answerttl"]), stoi(cfpars["main"]["cachesize"]));
			if (cfpars["main"]["soaenabled"] == "1")
				modules[SOA] = SOAModule(cfpars["files"]["soafile"], stol(cfpars["main"]["answerttl"]));
			if (cfpars["main"]["cnameenabled"] == "1")
				modules[CNAME] = CNAMEModule(cfpars["files"]["cnamefile"], stol(cfpars["main"]["answerttl"]));
		}
		catch (const exception& exp)
		{
			throw logic_error(string("DNServer: Module Load Error: ") + exp.what());
		}

		#ifdef _WIN32
			if (WSAStartup(MAKEWORD(2, 2), &wsaData))
				throw logic_error("DNServer: WSAStartup error");
		#endif
	}

	DNServer::~DNServer()
	{
		#ifdef _WIN32
			WSACleanup();
		#endif
	}

	DNServer& DNServer::getInstance(const std::string& configName)
	{
		static DNServer server(configName);
		return server;
	}

	void DNServer::mainLoop()
	{
        #ifdef __linux
            typedef int SOCKET;
            typedef sockaddr SOCKADDR;
            typedef sockaddr_in SOCKADDR_IN;
            const int INVALID_SOCKET = -1;
            #define closesocket(X) close(X)
        #endif
		SOCKET servSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (servSock == INVALID_SOCKET)
			throw logic_error("DNServer: socket error");

		SOCKET cliSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (cliSock == INVALID_SOCKET)
		{
			closesocket(servSock);
			throw logic_error("DNServer: socket error");
		}

		try
		{
			SOCKADDR_IN ssin = { 0 };
			ssin.sin_family = AF_INET;
			ssin.sin_addr.s_addr = inet_addr(cfpars["main"]["bindaddr"].c_str());
			ssin.sin_port = htons(stoi(cfpars["main"]["bindport"]));

			SOCKADDR_IN assin = { 0 };
			assin.sin_family = AF_INET;
			assin.sin_addr.s_addr = inet_addr(cfpars["main"]["rootdns"].c_str());
			assin.sin_port = htons(stoi(cfpars["main"]["rootdnsport"]));

			if (::bind(servSock, reinterpret_cast<SOCKADDR*>(&ssin), sizeof(ssin)))
					throw logic_error("DNServer: bind error");

			unsigned char packet[BUFSIZE];
			while (1)
			{
				memset(packet, 0, sizeof(packet));
				size_t recSize;
				SOCKADDR_IN ssinf = { 0 };
                #ifdef _WIN32
                    int ssinsz = sizeof(ssinf);
                #elif __linux
                    socklen_t ssinsz = sizeof(ssinf);
                #endif
				if ((recSize = recvfrom(servSock, reinterpret_cast<char*>(packet), sizeof(packet), 0, reinterpret_cast<SOCKADDR*>(&ssinf), &ssinsz)) > 0)
				{
					DnsResponse response(packet);
					if (filter(inet_ntoa(ssinf.sin_addr)) && processClient(response))
					{
						// обработать запрос и отдать ответ
						size_t respsize = response.size();

						response.setTruncated(respsize >= 512);

						if (respsize > sizeof(packet))
						{
							unique_ptr<unsigned char[]> uptr(new unsigned char[respsize]);
							response.dump(uptr.get());
							sendto(servSock, reinterpret_cast<char*>(uptr.get()), respsize, 0, reinterpret_cast<SOCKADDR*>(&ssinf), sizeof(ssinf));
						}
						else
						{
							memset(packet, 0, sizeof(packet));
							response.dump(packet);
							sendto(servSock, reinterpret_cast<char*>(packet), respsize, 0, reinterpret_cast<SOCKADDR*>(&ssinf), sizeof(ssinf));
						}
					}
					else
					{
						// данный тип запроса не поддерживается
						// отправить запрос основному серверу
						sendto(cliSock, reinterpret_cast<char*>(packet), recSize, 0, reinterpret_cast<SOCKADDR*>(&assin), sizeof(assin));
						memset(packet, 0, sizeof(packet));
						int rfms = recvfrom(cliSock, reinterpret_cast<char*>(packet), sizeof(packet), 0, NULL, NULL);
						sendto(servSock, reinterpret_cast<char*>(packet), rfms, 0, reinterpret_cast<SOCKADDR*>(&ssinf), sizeof(ssinf));
					}
				}
			}
		}
		catch (...)
		{
			closesocket(servSock);
			closesocket(cliSock);
			throw;
		}
		
		closesocket(servSock);
		closesocket(cliSock);
	}

	bool DNServer::processClient(DnsResponse& resp)
	{
		bool status = false;
		if (resp.getMessageType() == QUERY && resp.getOperationCode() == STANDART)
		{
			resp.setMessageType(ANSWER);
			resp.setAuthorityAnswer(true);
			resp.setAllowRecursion(false);
			resp.setReturnCode(NOERR);

			for (int i = 0; i < resp.getQueryCount(); i++)
			{
				auto mit = modules.find(resp.getType(i));
				if (mit != modules.end() && mit->second(resp, i))
					status = true;	// хотя бы один модуль может ответить
			}
		}
		return status;
	}
}
