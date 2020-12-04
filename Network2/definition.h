//
// 2019-08-31, jjuiddong
// definition
//
#pragma once


namespace network2
{

	enum {
		// netid definition
		INVALID_NETID = -1 // �߸��� NetId�� ���Ѵ�. ���������� ����
		, SERVER_NETID = 0 // ����� Server�� NetId�� ���� (Ŭ���̾�Ʈ �� ��)
		, CLIENT_NETID = 0 // ����� Client�� NetId�� ���� (UDP ���� �� ��)
		, ALL_NETID = ~1 // ����� Connection ��θ� ���� (����/Ŭ���̾�Ʈ)

		// constant definition
		, RECV_BUFFER_LENGTH = 1024 * 2
		, DEFAULT_PACKETSIZE = 1024 * 1
		, DEFAULT_MAX_PACKETCOUNT = 10
		, DEFAULT_SLEEPMILLIS = 30
		, DEFAULT_SOCKETBUFFER_SIZE = 1024 * 10
	};
	
	struct eProtocolType {
		enum Enum {
			TCPIP, UDPIP
		};
	};

	// packet data format
	enum class ePacketFormat {
		BINARY, // binary packet
		ASCII, // ascii packet
		JSON, // json string or binary packet
		FREE, // no format, no packet header
	};

}

