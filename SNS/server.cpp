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
	http://tools.ietf.org/html/rfc1035#page-12
	https://technet.microsoft.com/en-us/library/dd197470%28v=ws.10%29.aspx

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

			unsigned char packet[BUFSIZE];
			while (1)
			{
				memset(packet, 0, sizeof(packet));
				SOCKADDR_IN ssinf = { 0 };
				size_t recSize;
				int ssinsz = sizeof(ssinf);
				if ((recSize = recvfrom(servSock, reinterpret_cast<char*>(packet), sizeof(packet), 0, reinterpret_cast<SOCKADDR*>(&ssinf), &ssinsz)) > 0)
				{
					DnsResponse response(packet);
					if (processClient(response))
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
						// данный тип запроса не поддерживаетс€
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
		if (resp.getMessageType() == QUERY && resp.getOperationCode() == STANDART)
		{
			for (int i = 0; i < resp.getQueryCount(); i++)
			{
				/*if (resp.getType(i) == A && resp.getAddress(i) == "sysadmins.ru")
				{
					resp.setMessageType(ANSWER);
					resp.setAuthorityAnswer(true);
					resp.setAllowRecursion(false);
					resp.setReturnCode(NOERR);

					resp.setAnswerCount(1);

					vector<unsigned char> rd = { 217, 69, 139, 200 };
					resp.addRawAnswer(resp.getRaw(i), 0, rd);
					return true;
				}
				else */if (resp.getType(i) == SOA && resp.getAddress(i) == "sysadmins.ru")
				{
					resp.setMessageType(ANSWER);
					resp.setAuthorityAnswer(true);
					resp.setAllowRecursion(false);
					resp.setReturnCode(NOERR);

					resp.setAnswerCount(1);

					string mname = "mname.ru";
					string rname = "rname.ru";
					struct SOAS
					{
						unsigned long serial;
						unsigned long refresh;
						unsigned long retry;
						unsigned long expire;
						unsigned long minimum;
					};
					SOAS t;
					t.serial = htonl(1111);
					t.refresh = htonl(2222);
					t.retry = htonl(3333);
					t.expire = htonl(4444);
					t.minimum = htonl(5555);

					size_t cnt = 0;
					string mnameo("");
					for (auto it = mname.rbegin(); it != mname.rend(); it++)
					{
						if (*it == '.')
						{
							mnameo = string(1,cnt) + mnameo;
							cnt = 0;
						}
						else
						{
							mnameo = *it + mnameo;
							++cnt;
						}
					}
					mnameo = string(1, cnt) + mnameo;

					cnt = 0;
					string rnameo("");
					for (auto it = rname.rbegin(); it != rname.rend(); it++)
					{
						if (*it == '.')
						{
							rnameo = string(1, cnt) + rnameo;
							cnt = 0;
						}
						else
						{
							rnameo = *it + rnameo;
							++cnt;
						}
					}
					rnameo = string(1, cnt) + rnameo;

					vector<unsigned char> rd(mnameo.length() + 1 + rnameo.length() + 1 + sizeof(SOAS));
					memcpy(&rd[0], mnameo.c_str(), mnameo.length() + 1);
					memcpy(&rd[mnameo.length() + 1], rnameo.c_str(), rnameo.length() + 1);
					memcpy(&rd[mnameo.length() + 1 + rnameo.length() + 1], &t, sizeof(SOAS));
					resp.addRawAnswer(resp.getRaw(i), 0, rd);
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}
}