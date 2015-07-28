#ifndef DNSPACKET_H
#define DNSPACKET_H

// standart headers
#include <cstring>
#include <vector>
#include <string>

// system headers
#include <WinSock2.h>

namespace SNS
{
	enum MessageType{ QUERY = 0, ANSWER = 1 };
	enum OperationCode{ STANDART = 0, INVERSION = 1, STATUS = 2 };
	enum ReturnCode{ NOERR = 0, FORMATERR = 1, SERVERR = 2, NAMEERR = 3 };

	enum RequestType{ A = 1, NS = 2, CNAME = 5, SOA = 6, MB = 7, WKS = 11, 
						PTR = 12, HINFO = 13, MINFO = 14, MX = 15, TXTISDN = 16, 
						AXFR = 252, ANY = 255 };

	class DnsHeader
	{
		#pragma pack(push, 1)
		struct DnsStruct
		{
			// Идентификатор
			unsigned short ident;
			// Флаги
			unsigned short flags;
			// Количество вопросов
			unsigned short qcount;
			// Количество ответов
			unsigned short acount;
			// Количество прав доступа
			unsigned short lcount;
			// Количество дополнительных записей
			unsigned short ocount;
		}header;
		struct FlagStruct
		{
			// код возврата
			unsigned rcode : 4;
			// нулевое
			unsigned nulled : 3;
			// рекурсия возможна
			unsigned RA : 1;
			// требуется рекурсия
			unsigned RD : 1;
			// "обрезано"
			unsigned TC : 1;
			// авторитетный ответ
			unsigned AA : 1;
			// код операции
			unsigned opcode : 4;
			// тип сообщения
			unsigned QR : 1;
		}*headflags;
		#pragma pack(pop)
	public:
		DnsHeader(const char* rawPacket);
		size_t headerSize() const;
		unsigned short getHeaderId() const;
		unsigned short getQueryCount() const;
		unsigned short getAnswerCount() const;
		unsigned short getAccessCount() const;
		unsigned short getAdditionCount() const;
		MessageType getMessageType() const;
		OperationCode getOperationCode() const;
		bool isAuthorityAnswer() const;
		bool isTruncated() const;
		bool needRecursion() const;
		bool allowRecursion() const;
		ReturnCode getReturnCode() const;
	};

	class DnsRequest:public DnsHeader
	{
		#pragma pack(push,1)
		struct ReqFlag
		{
			unsigned short qtype;
			unsigned short qclass;
		};
		#pragma pack(pop)
		std::vector<std::pair<std::string, ReqFlag>> items;
	public:
		DnsRequest(const char* rawPacket);
		size_t reqCount() const;
		const std::string& getAddress(size_t i) const;
		RequestType getType(size_t i) const;
		unsigned short getClass(size_t i) const;
	};

	class DnsResponse:public DnsRequest
	{
		
	};
}

#endif // !DNSPACKET_H
