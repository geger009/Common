//
// 2016-07-05, jjuiddong
// 
// cTCPClient �� ��ü�ϱ����� ���������.
// Connection �� ������� ó���Ѵ�.
//
// 2018-12-09, jjuiddong
//	- send all error, close connection
//
// 2019-01-04
//		- update iProtocol
//
#pragma once

#include "packetqueue.h"

namespace network
{

	class cTCPClient2
	{
	public:
		cTCPClient2(iProtocol *protocol = new cProtocol());
		virtual ~cTCPClient2();

		bool Init(const string &ip, const int port
			, const int packetSize = 512, const int maxPacketCount = 10, const int sleepMillis = 30
			, const int clientSidePort = -1);
		void Send(iProtocol *protocol, const BYTE *buff, const int len);
		bool ReConnect();
		void Close();
		bool IsConnect() const;
		bool IsReadyConnect() const;
		bool IsFailConnection() const;

		static void TCPClient2ThreadFunction(cTCPClient2 *client);


	public:
		enum STATE {
			READYCONNECT,  // Ŀ�ؼ� ��, ������ �����ϱ� ����.
			TRYCONNECT,  // ������ ������ ���� Ŀ�ؼ� ��û ����
			CONNECT,  // Ŀ�ؼ� ����
			DISCONNECT, // Ŀ�ؼ� ����
			CONNECT_ERROR, // Ŀ�ؼ� ����, �ַ� ����ȭ �������� �߻�
		};
		 
		STATE m_state;
		string m_ip;
		int m_port;
		int m_clientSidePort;
		SOCKET m_socket; // server socket
		int m_maxBuffLen;
		iProtocol *m_protocol;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		int m_sleepMillis;
		int m_recvBytes; // debug
	};


	inline bool cTCPClient2::IsConnect() const { return m_state == CONNECT; }
	inline bool cTCPClient2::IsReadyConnect() const { return (m_state == READYCONNECT) || (m_state == TRYCONNECT); }
	inline bool cTCPClient2::IsFailConnection() const {return m_state == DISCONNECT; }
}
