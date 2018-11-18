#pragma once


namespace network
{
	
	namespace SESSION_STATE {
		enum TYPE {
			DISCONNECT,
			LOGIN_WAIT,
			LOGIN,					// ������ �� ����
			LOGOUT_WAIT,	// ���� ��� ��Ͽ� �ִ� ����
		};}


	struct sSession
	{
		SESSION_STATE::TYPE state;
		SOCKET socket;
		char ip[16];
		int port;
	};

}