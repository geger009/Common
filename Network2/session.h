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
		cSession(const netid id, const StrId &name, const bool isPacketLog = false);
		virtual ~cSession();

		virtual void Close();
		int GetId() const;
		bool IsConnect() const;
		bool IsReadyConnect() const;
		bool IsFailConnection() const;


	public:
		enum State {
			READYCONNECT,  // Ŀ�ؼ� ��, ������ �����ϱ� ����.
			TRYCONNECT,  // ������ ������ ���� Ŀ�ؼ� ��û ����
			CONNECT,  // Ŀ�ؼ� ����
			DISCONNECT, // Ŀ�ؼ� ����
			CONNECT_ERROR, // Ŀ�ؼ� ����, �ַ� ����ȭ �������� �߻�
		};

		netid m_id;
		SOCKET m_socket;
		State m_state;
		StrId m_name;
		Str16 m_ip;
		int m_port;
		bool m_isPacketLog;
	};


	// Listener Change Session
	class iSessionListener
	{
	public:
		virtual void RemoveSession(cSession &session) = 0;
		virtual void AddSession(cSession &session) = 0;
	};

}
