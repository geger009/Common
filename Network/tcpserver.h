//
// 2015-11-28, jjuiddong
//
// TCP/IP ���������� �̿��ؼ� ����ϴ� ��ü��.
// �ִ��� �����ϰ� �������.
//
#pragma once

#include "session.h"
#include "packetqueue.h"

namespace network
{
	class iSessionListener
	{
	public:
		virtual void RemoveSession(const sSession &session) = 0;
		virtual void AddSession(const sSession &session) = 0;
	};

	class cTCPServer
	{
	public:
		cTCPServer();
		virtual ~cTCPServer();

		bool Init(const int port, const int packetSize = 512
			, const int maxPacketCount = 10, const int sleepMillis = 30);
		void SetListener(iSessionListener *listener);
		void Close();
		bool IsConnect() const;
		void MakeFdSet(OUT fd_set &out);
		bool AddSession(const SOCKET remoteSock, const string &ip, const int port);
		void RemoveSession(const SOCKET remoteSock);
		bool IsExistSession();


	public:
		SOCKET m_svrSocket;
		vector<sSession> m_sessions;
		iSessionListener *m_listener;
		int m_port;
		bool m_isConnect;
 		int m_maxBuffLen;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		common::CriticalSection m_criticalSection;
		bool m_threadLoop;
		int m_sleepMillis;
		int m_sendBytes; // debug
	};


	inline bool cTCPServer::IsConnect() const { return m_isConnect; }
}
