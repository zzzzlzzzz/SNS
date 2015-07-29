#include "server.h"

using namespace std;

/* 
	http://kunegin.narod.ru/ref3/dns/format.htm
	http://citforum.ru/internet/dns/dns/
	http://book.itep.ru/4/44/dns_4412.htm
	http://cdo.bseu.by/library/ibs1/net_l/tcp_ip/dns/dns.htm
	http://www.zytrax.com/books/dns/ch15/
	http://www.soslan.ru/tcp/tcp14.html
	https://xakep.ru/2001/07/03/12975/

	TODO:
		* ƒобавить анализ записей типа A
		* добавить возможность редиректа в случае неподдерживаемого запроса
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

			if (bind(servSock, reinterpret_cast<SOCKADDR*>(&ssin), sizeof(ssin)))
					throw logic_error("DNServer: bind error");

			char packet[BUFSIZE];
			while (1)
			{
				size_t recSize;
				memset(packet, 0, sizeof(packet));
				SOCKADDR_IN ssinf = { 0 };
				int ssinsz = sizeof(ssinf);
				if ((recSize = recvfrom(servSock, packet, sizeof(packet), 0, reinterpret_cast<SOCKADDR*>(&ssinf), &ssinsz)) > 0)
				{
					DnsResponse response(packet);
					if (processClient(response))
					{
						// обработать запрос и отдать ответ
						ssinsz = sizeof(ssinf);
						size_t respsize = response.size();

						response.setTruncated(respsize >= 512);

						if (respsize > sizeof(packet))
						{
							unique_ptr<char[]> uptr(new char[respsize]);
							response.dump(uptr.get());
							sendto(servSock, uptr.get(), respsize, 0, reinterpret_cast<SOCKADDR*>(&ssinf), ssinsz);
						}
						else
						{
							memset(packet, 0, sizeof(packet));
							response.dump(packet);
							sendto(servSock, packet, respsize, 0, reinterpret_cast<SOCKADDR*>(&ssinf), ssinsz);
						}
					}
					else
					{
						// данный тип запроса не поддерживаетс€
						// отправить запрос основному серверу
						int assinsz = sizeof(assin);
						sendto(cliSock, packet, recSize, 0, reinterpret_cast<SOCKADDR*>(&assin), assinsz);
						memset(packet, 0, sizeof(packet));
						int rfms = recvfrom(cliSock, packet, sizeof(packet), 0, NULL, NULL);
						ssinsz = sizeof(ssinf);
						sendto(servSock, packet, rfms, 0, reinterpret_cast<SOCKADDR*>(&ssinf), ssinsz);
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
		return false;
	}
}