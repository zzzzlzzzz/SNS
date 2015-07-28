#include "dnspacket.h"

using namespace std;

namespace SNS
{
	DnsHeader::DnsHeader(const char* rawPacket)
	{
		const DnsStruct* src = reinterpret_cast<const DnsStruct*>(rawPacket);
		header.ident = ntohs(src->ident);
		header.flags = ntohs(src->flags);
		header.qcount = ntohs(src->qcount);
		header.acount = ntohs(src->acount);
		header.lcount = ntohs(src->lcount);
		header.ocount = ntohs(src->ocount);
		headflags = reinterpret_cast<FlagStruct*>(&header.flags);
	}

	unsigned short DnsHeader::getHeaderId() const
	{
		return header.ident;
	}

	unsigned short DnsHeader::getQueryCount() const
	{
		return header.qcount;
	}

	unsigned short DnsHeader::getAnswerCount() const
	{
		return header.acount;
	}

	unsigned short DnsHeader::getAccessCount() const
	{
		return header.lcount;
	}

	unsigned short DnsHeader::getAdditionCount() const
	{
		return header.ocount;
	}

	MessageType DnsHeader::getMessageType() const
	{
		return MessageType(headflags->QR);
	}

	OperationCode DnsHeader::getOperationCode() const
	{
		return OperationCode(headflags->opcode);
	}

	bool DnsHeader::isAuthorityAnswer() const
	{
		return headflags->AA;
	}

	bool DnsHeader::isTruncated() const
	{
		return headflags->TC;
	}

	bool DnsHeader::needRecursion() const
	{
		return headflags->RD;
	}

	bool DnsHeader::allowRecursion() const
	{
		return headflags->RA;
	}

	ReturnCode DnsHeader::getReturnCode() const
	{
		return ReturnCode(headflags->rcode);
	}

	size_t DnsHeader::headerSize() const
	{
		return sizeof(DnsStruct);
	}

	DnsRequest::DnsRequest(const char* rawPacket) :DnsHeader(rawPacket)
	{
		const char* ptr = rawPacket + DnsHeader::headerSize();
		for (unsigned short i = 0; i < getQueryCount(); i++)
		{
			string domain("");
			for (++ptr; *ptr; ++ptr)
			{
				if (*ptr > 63)
					domain.append(1, *ptr);
				else
					domain.append(".");
			}
			const ReqFlag* rf = reinterpret_cast<const ReqFlag*>(++ptr);
			ptr += sizeof(ReqFlag);

			items.push_back(make_pair(domain, ReqFlag{ ntohs(rf->qtype), ntohs(rf->qclass) }));
		}
	}

	size_t DnsRequest::reqCount() const
	{
		return items.size();
	}

	const string& DnsRequest::getAddress(size_t i) const
	{
		return items[i].first;
	}

	RequestType DnsRequest::getType(size_t i) const
	{
		return RequestType(items[i].second.qtype);
	}

	unsigned short DnsRequest::getClass(size_t i) const
	{
		return items[i].second.qclass;
	}
}