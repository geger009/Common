//
// 2019-01-08, jjuiddong
// Network Session class
//
#pragma once


namespace network2
{

	class cSession
	{
	public:
		cSession();
		cSession(const netid id, const StrId &name, const int logId = -1);
		virtual ~cSession();

		virtual void Close();
		int GetId() const;
		bool IsConnect() const;
		bool IsReadyConnect() const;
		bool IsFailConnection() const;


	public:
		enum eState {
			READYCONNECT,  // Ŀ�ؼ� ��, ������ �����ϱ� ����.
			TRYCONNECT,  // ������ ������ ���� Ŀ�ؼ� ��û ����
			CONNECT,  // Ŀ�ؼ� ����
			DISCONNECT, // Ŀ�ؼ� ����
			CONNECT_ERROR, // Ŀ�ؼ� ����, �ַ� ����ȭ �������� �߻�
		};

		netid m_id;
		SOCKET m_socket;
		eState m_state;
		StrId m_name;
		Str16 m_ip;
		int m_port;
		int m_logId; // packet log id, default: -1
	};


	// Change Session State Listener
	// server side: accept, disconnect session
	// client side: connect, disconnect session
	class iSessionListener
	{
	public:
		virtual void RemoveSession(network2::cSession &session) = 0;
		virtual void AddSession(network2::cSession &session) = 0;
	};

}
