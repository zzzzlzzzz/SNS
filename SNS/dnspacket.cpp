#include "dnspacket.h"

using namespace std;

namespace SNS
{
	//////////////////////////////////////////////////////////////////////////

	DnsHeader::DnsHeader(const unsigned char* rawPacket)
	{
		const DnsStruct* src = reinterpret_cast<const DnsStruct*>(rawPacket);
		header.ident = ntohs(src->ident);
		header.flags = ntohs(src->flags);
		header.queryCount = ntohs(src->queryCount);
		header.answerCount = ntohs(src->answerCount);
		header.authorityCount = ntohs(src->authorityCount);
		header.additionCount = ntohs(src->additionCount);
	}

	unsigned short DnsHeader::getHeaderId() const
	{
		return header.ident;
	}

	unsigned short DnsHeader::getQueryCount() const
	{
		return header.queryCount;
	}

	void DnsHeader::setQueryCount(unsigned short c)
	{
		header.queryCount = c;
	}

	unsigned short DnsHeader::getAnswerCount() const
	{
		return header.answerCount;
	}

	void DnsHeader::setAnswerCount(unsigned short c)
	{
		header.answerCount = c;
	}

	unsigned short DnsHeader::getAuthorityCount() const
	{
		return header.authorityCount;
	}

	void DnsHeader::setAuthorityCount(unsigned short c)
	{
		header.authorityCount = c;
	}

	unsigned short DnsHeader::getAdditionCount() const
	{
		return header.additionCount;
	}

	void DnsHeader::setAdditionCount(unsigned short c)
	{
		header.additionCount = c;
	}

	MessageType DnsHeader::getMessageType() const
	{
		return MessageType(header.flags >> 15);
	}

	void DnsHeader::setMessageType(MessageType t)
	{
		unsigned short x = t;
		header.flags &= ~(0x1 << 15);
		header.flags |= (x << 15);
	}

	OperationCode DnsHeader::getOperationCode() const
	{
		return OperationCode((header.flags >> 11) & 0xF);
	}

	void DnsHeader::setOperationCode(OperationCode t)
	{
		unsigned short x = t;
		header.flags &= ~(0xF << 11);
		header.flags |= (x << 11);
	}

	bool DnsHeader::isAuthorityAnswer() const
	{
		return ((header.flags >> 10) & 1);
	}

	void DnsHeader::setAuthorityAnswer(bool t)
	{
		unsigned short x = t;
		header.flags &= ~(0x1 << 10);
		header.flags |= (x << 10);
	}

	bool DnsHeader::isTruncated() const
	{
		return ((header.flags >> 9) & 1);
	}

	void DnsHeader::setTruncated(bool t)
	{
		unsigned short x = t;
		header.flags &= ~(0x1 << 9);
		header.flags |= (x << 9);
	}

	bool DnsHeader::needRecursion() const
	{
		return ((header.flags >> 8) & 1);
	}

	void DnsHeader::setNeedRecursion(bool t)
	{
		unsigned short x = t;
		header.flags &= ~(0x1 << 8);
		header.flags |= (x << 8);
	}

	bool DnsHeader::allowRecursion() const
	{
		return ((header.flags >> 7) & 1);
	}

	void DnsHeader::setAllowRecursion(bool t)
	{
		unsigned short x = t;
		header.flags &= ~(0x1 << 7);
		header.flags |= (x << 7);
	}

	ReturnCode DnsHeader::getReturnCode() const
	{
		return ReturnCode(header.flags & 0xF);
	}

	void DnsHeader::setReturnCode(ReturnCode t)
	{
		unsigned short x = t;
		header.flags &= ~(0xF);
		header.flags |= x;
	}

	size_t DnsHeader::size() const
	{
		return sizeof(DnsStruct);
	}

	unsigned char* DnsHeader::dump(unsigned char* out) const
	{
		DnsStruct* dst = reinterpret_cast<DnsStruct*>(out);
		dst->ident = htons(header.ident);
		dst->flags = htons(header.flags);
		dst->queryCount = htons(header.queryCount);
		dst->answerCount = htons(header.answerCount);
		dst->authorityCount = htons(header.authorityCount);
		dst->additionCount = htons(header.additionCount);
		return out + sizeof(DnsStruct);
	}

	//////////////////////////////////////////////////////////////////////////

	DnsRequest::DnsRequest(const unsigned char* rawPacket) :DnsHeader(rawPacket)
	{
		const unsigned char* ptr = rawPacket + DnsHeader::size();
		for (unsigned short i = 0; i < getQueryCount(); i++)
		{
			string domain(reinterpret_cast<const char*>(ptr));
			ptr += domain.length() + 1;
			const ReqFlag* rf = reinterpret_cast<const ReqFlag*>(ptr);
			ptr += sizeof(ReqFlag);

			items.push_back(make_pair(domain, ReqFlag{ ntohs(rf->qtype), ntohs(rf->qclass) }));
		}
	}

	size_t DnsRequest::size() const
	{
		size_t sz = DnsHeader::size();
		for (const auto& x : items)
			sz += (x.first.length() + 1) + sizeof(ReqFlag);
		return sz;
	}

	size_t DnsRequest::reqCount() const
	{
		return items.size();
	}

	string DnsRequest::getAddress(size_t i) const
	{
		string outstr;
		for (auto it = items[i].first.cbegin() + 1; it != items[i].first.cend(); it++)
			if (*it < 64)
				outstr += ".";
			else
				outstr += *it;
		return outstr;
	}

	RequestType DnsRequest::getType(size_t i) const
	{
		return RequestType(items[i].second.qtype);
	}

	unsigned short DnsRequest::getClass(size_t i) const
	{
		return items[i].second.qclass;
	}

	const pair<string, DnsRequest::ReqFlag>& DnsRequest::getRaw(size_t i) const
	{
		return items[i];
	}

	unsigned char* DnsRequest::dump(unsigned char* out) const
	{
		unsigned char* pout = DnsHeader::dump(out);
		for (const auto& x : items)
		{
			memcpy(pout, x.first.c_str(), x.first.length() + 1);
			pout += (x.first.length() + 1);
			ReqFlag* rf = reinterpret_cast<ReqFlag*>(pout);
			rf->qtype = htons(x.second.qtype);
			rf->qclass = htons(x.second.qclass);
			pout += sizeof(ReqFlag);
		}
		return pout;
	}

	//////////////////////////////////////////////////////////////////////////

	DnsResponse::DnsResponse(const unsigned char* rawPacket) :DnsRequest(rawPacket)
	{

	}

	size_t DnsResponse::size() const
	{
		size_t sz = DnsRequest::size();
		if (!answer.empty())
			sz += sectionSize(answer);
		if (!legacy.empty())
			sz += sectionSize(legacy);
		if (!addition.empty())
			sz += sectionSize(addition);
		return sz;
	}

	size_t DnsResponse::sectionSize(const std::vector<std::pair<std::pair<std::string, ReqFlag>, std::pair<RespFlag, std::vector<unsigned char>>>>& section) const
	{
		size_t sz = 0;
		for (const auto& x : section)
		{
			sz += x.first.first.length() + 1;
			sz += sizeof(ReqFlag);
			sz += sizeof(RespFlag);
			sz += x.second.second.size();
		}
		return sz;
	}

	unsigned char* DnsResponse::appSection(const vector<pair<pair<string, ReqFlag>, std::pair<RespFlag, std::vector<unsigned char>>>>& section, unsigned char* out) const
	{
		unsigned char* pout = out;
		for (const auto& x : section)
		{
			memcpy(pout, x.first.first.c_str(), x.first.first.length() + 1);
			pout += x.first.first.length() + 1;
			ReqFlag* reqf = reinterpret_cast<ReqFlag*>(pout);
			reqf->qtype = htons(x.first.second.qtype);
			reqf->qclass = htons(x.first.second.qclass);
			pout += sizeof(ReqFlag);
			RespFlag* resf = reinterpret_cast<RespFlag*>(pout);
			resf->TTL = htonl(x.second.first.TTL);
			resf->len = htons(x.second.first.len);
			pout += sizeof(RespFlag);
			for (auto c : x.second.second)
				*(pout++) = c;
		}
		return pout;
	}

	unsigned char* DnsResponse::dump(unsigned char* out) const
	{
		unsigned char* pout = DnsRequest::dump(out);
		if (!answer.empty())
			pout = appSection(answer, pout);
		if (!legacy.empty())
			pout = appSection(legacy, pout);
		if (!addition.empty())
			pout = appSection(addition, pout);
		return pout;
	}

	void DnsResponse::addRawAnswer(const pair<string, ReqFlag>& query, unsigned long TTL, const vector<unsigned char>& raw)
	{
        RespFlag resp{ TTL, static_cast<unsigned short>(raw.size()) };
		answer.push_back(make_pair(query, make_pair(resp, raw)));
	}

	void DnsResponse::addRawAuthority(const pair<string, ReqFlag>& query, unsigned long TTL, const vector<unsigned char>& raw)
	{
        RespFlag resp{ TTL, static_cast<unsigned short>(raw.size()) };
		legacy.push_back(make_pair(query, make_pair(resp, raw)));
	}

	void DnsResponse::addRawAddition(const pair<string, ReqFlag>& query, unsigned long TTL, const vector<unsigned char>& raw)
	{
        RespFlag resp{ TTL, static_cast<unsigned short>(raw.size()) };
		addition.push_back(make_pair(query, make_pair(resp, raw)));
	}
}
