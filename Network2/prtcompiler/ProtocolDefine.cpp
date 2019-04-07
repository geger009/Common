
#include "stdafx.h"
#include "ProtocolDefine.h"


using namespace network2;

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void network2::PrintToken( Tokentype token, char *szTokenString )
{
	switch( token )
	{
	case ID:
	case STRING:	printf( "string = %s\n", szTokenString ); break;

	case ASSIGN:	printf( "=" ); break;
	case TIMES:		printf( "*" ); break;
	case LPAREN:	printf( "(" ); break;
	case RPAREN:	printf( ")" ); break;
	case LBRACE:	printf( "{" ); break;
	case RBRACE:	printf( "}" ); break;
	case COMMA:	printf( "," ); break;
	case NUM:		printf( "number" ); break;
	case OR:		printf( "||" ); break;
	case AND:		printf( "&&" ); break;

	default: 		printf( "UnKnown token %d, %s\n", token, szTokenString ); break;
	}
}


//------------------------------------------------------------------------
// typeStr: sArg->var->type
// ��Ʈ���� Ÿ������ �����Ѵ�.
//------------------------------------------------------------------------
_variant_t network2::GetTypeStr2Type(const string &typeStr)
{
	if (typeStr == "std::string")
	{
		char *v=NULL;
		return _variant_t(v);
	}
	else if (typeStr == "string")
	{
		char *v=NULL;
		return _variant_t(v);
	}
	else if (typeStr == "float")
	{
		float v=0.f;
		return _variant_t(v);
	}
	else if (typeStr == "double")
	{
		double v=0.f;
		return _variant_t(v);
	}
	else if (typeStr == "int")
	{
		int v=0;
		return _variant_t(v);
	}
	else if (typeStr == "char")
	{
		char v='a';
		return _variant_t(v);
	}
	else if (typeStr == "short")
	{
		short v=0;
		return _variant_t(v);
	}
	else if (typeStr == "long")
	{
		long v=0;
		return _variant_t(v);
	}
	else if (typeStr == "bool")
	{
		bool v=true;
		return _variant_t(v);
	}
	else if (typeStr == "BOOL")
	{
		bool v=true;
		return _variant_t(v);
	}

	return _variant_t(1);
}


//------------------------------------------------------------------------
// packetID�� �����Ѵ�. ���⼭ Packet�̶� sProtocol protocol�� �ǹ��ϰ�, 
// sRmi �� �ڽ����� ������� ��ȣ�� �Ű��� ���� Packet ID�̴�.
//------------------------------------------------------------------------
int	network2::GetPacketID(sRmi *rmi, sProtocol *packet)
{
	if (!rmi) return 0;
	if (!packet) return rmi->number;

	int id = rmi->number + 1;
	sProtocol *p = rmi->protocol;
	while (p)
	{
		if (p == packet)
			break;
		++id;
		p = p->next;
	}
	return id;
}


//------------------------------------------------------------------------
// ��Ŷ������ ��Ʈ������ ��ȯ �Ѵ�.
// ���� �޸� �Ҵ��� ���� ������, ����� �ÿ��� ����
//------------------------------------------------------------------------
string network2::Packet2String(const cPacket &packet, sProtocol *protocol)
{
	if (!protocol)
	{
		return format( " protocol id = %d, packet id = %d"
			, packet.GetProtocolId(), packet.GetPacketId() );
	}

	std::stringstream ss;
	cPacket tempPacket = packet;
	tempPacket.InitRead();

	const int protocolID = tempPacket.GetProtocolId();
	const int packetID = tempPacket.GetPacketId();
	const bool isBinaryPacket = dynamic_cast<cPacketHeader*>(tempPacket.m_packetHeader)? true : false;

	ss << protocol->name << " sender = " << tempPacket.GetSenderId() << " ";

	sArg *arg = protocol->argList;
	while (arg)
	{
		// todo: ���� �Ϲ����� ó���� �ʿ��ϴ�.
		// vector<float>, vector<char> �� �������� �ʴ´�.
		if (arg->var->type == "vector<int>")
		{
			vector<int> vecs;
			if (isBinaryPacket)
			{
				using namespace marshalling;
				tempPacket >> vecs;
			}
			else
			{
				using namespace marshalling_ascii;
				tempPacket >> vecs;
			}

			ss << arg->var->var + " = ";
			for (auto &v : vecs)
				ss << v << ";";
		}
		else
		{
			_variant_t var = GetTypeStr2Type(arg->var->type);

			if (isBinaryPacket)
			{
				using namespace marshalling;
				tempPacket >> var;
			}
			else
			{
				using namespace marshalling_ascii;
				tempPacket >> var;
			}

			ss << arg->var->var + " = ";
			ss << common::variant2str(var);
		}

		//if ( arg->var->var == "errorCode")
		//	ss << "(" << ErrorCodeString((error::ERROR_CODE)(int)var) << ")";

		arg = arg->next;
		if (arg)
			ss << ", ";
	}

	return ss.str();
}


/**
 @brief 
 */
void network2::ReleaseRmi(sRmi *p)
{
	if (!p) return;
	ReleaseProtocol(p->protocol);
	ReleaseRmi(p->next);
	delete p;
}


/**
 @brief ReleaseRmiOnly
 */
void network2::ReleaseRmiOnly(sRmi *p)
{
	if (!p) return;
	ReleaseRmiOnly(p->next);
	delete p;
}

/**
 @brief 
 */
void network2::ReleaseProtocol(sProtocol *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	ReleaseProtocol(p->next);
	delete p;
}


/**
 @brief 
 */
void network2::ReleaseCurrentProtocol(sProtocol *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	delete p;
}


/**
 @brief 
 */
void network2::ReleaseArg(sArg *p)
{
	if (!p) return;
	if (p->var) delete p->var;
	ReleaseArg(p->next);
	delete p;
}