
#include "stdafx.h"
#include "protocol_basic_dispatcher.h"

using namespace network2;
using namespace basic_protocol;


// ������ ���õ� �⺻���� �۾��� ó���Ѵ�.
// Connect, Disconnect, Accept�� ��Ŷȭ �ؼ� ó���Ѵ�.
void basic_protocol::ServerDispatcher::Dispatch(cPacket &packet, cTcpServer *svr)
{
	RET(!svr);

	switch (packet.GetPacketId())
	{
	case PACKETID_DISCONNECT:
	{
		netid disconnectId = INVALID_NETID;
		packet >> disconnectId;
		if (svr->GetId() == disconnectId)
		{
			svr->Close();
		}
		else
		{
			svr->RemoveSession(disconnectId);
		}
	}
	break;

	case PACKETID_CLIENT_DISCONNECT:
	{
		netid disconnectId = INVALID_NETID;
		packet >> disconnectId;
		svr->RemoveSession(disconnectId);
	}
	break;

	case PACKETID_ACCEPT:
	{
		SOCKET remoteClientSock;
		packet >> remoteClientSock;
		std::string ip;
		packet >> ip;
		int port;
		packet >> port;
		svr->AddSession(remoteClientSock, ip, port);
	}
	break;
	}
}


// Ŭ���̾�Ʈ�� ���õ� �⺻���� �۾��� ó���Ѵ�.
// Disconnect �� ��Ŷȭ �ؼ� ó���Ѵ�.
void basic_protocol::ClientDispatcher::Dispatch(cPacket &packet, cTcpClient *client)
{
	RET(!client);

	switch (packet.GetPacketId())
	{
	case PACKETID_DISCONNECT:
	{
		client->Close();
	}
	break;
	}
}

