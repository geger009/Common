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
		enum class eState {
			ReadyConnect,  // Ŀ�ؼ� ��, ������ �����ϱ� ����.
			TryConnect,  // ������ ������ ���� Ŀ�ؼ� ��û ����
			Connect,  // Ŀ�ؼ� ����
			Disconnect, // Ŀ�ؼ� ����
			ConnectError, // Ŀ�ؼ� ����, �ַ� ����ȭ �������� �߻�
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
