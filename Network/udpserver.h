//
// v.1
// udp server, receive ���
//
// v.2
// close(), waitforsingleobject
// isConnect()
// add variable m_isReceivData
// thread restart bug fix
//
// 2016-08-30
//		- packetqueue �߰�
//
// 2019-01-04
//		- update iProtocol
//
#pragma once


namespace network
{

	class cUDPServer
	{
	public:
		cUDPServer(iProtocol *protocol = new cProtocol());
		virtual ~cUDPServer();

		bool Init(const int id, const int port 
			,const int packetSize = 1024, const int maxPacketCount = 512, const int sleepMillis = 1);

		int GetRecvData(OUT BYTE *dst, const int maxSize);
		void SetMaxBufferLength(const int length);
		void Close(const bool isWait = false);
		bool IsConnect() const;


	public:
		int m_id;
		SOCKET m_socket;
		int m_port;
		bool m_isConnect;
		iProtocol *m_protocol;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		bool m_threadLoop;
		int m_sleepMillis; // default = 1
	};


	inline bool cUDPServer::IsConnect() const { return m_isConnect; }
}
