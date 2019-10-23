#include "stdafx.h"
#include "packetqueue.h"
#include "session.h"

using namespace network2;


cPacketQueue::cPacketQueue(cNetworkNode *node
	, const bool isPacketLog //= false
)
	: m_isPacketLog(isPacketLog)
	, m_isLogIgnorePacket(true)
	, m_netNode(node)
	, m_nextFrontIdx(0)
	, m_sockBufferSize(0)
{
	InitializeCriticalSectionAndSpinCount(&m_cs, 0x00000400);
}

cPacketQueue::~cPacketQueue()
{
	Clear();

	DeleteCriticalSection(&m_cs);
}


bool cPacketQueue::Init(const int packetSize, const int maxPacketCount)
{
	Clear();

	m_sockBufferSize = packetSize * maxPacketCount;
	return true;
}


// ��Ŷ�� ť�� �����Ѵ�.
// data�� ��Ȯ�� ��Ŷ ũ�⸸ŭ�� ������ �������Ѵ�.
// ��Ʈ��ũ�κ��� �޾Ƽ� �����ϴ°� �ƴ϶�, ����ڰ� ���� ��Ŷ�� ť�� ���� ��, ����ϴ� �Լ�.
// return true : ����
//		  false : ����
bool cPacketQueue::Push(const netid rcvId, const cPacket &packet)
{
	cAutoCS cs(m_cs);

	cSocketBuffer *sockBuff = NULL;
	auto it = m_sockBuffers.find(rcvId);
	if (m_sockBuffers.end() == it)
	{
		sockBuff = new cSocketBuffer(rcvId, m_sockBufferSize);
		m_sockBuffers.insert({ rcvId, sockBuff });
	}
	else
	{
		sockBuff = it->second;
	}

	const uint addLen = sockBuff->Push(packet.m_packetHeader, packet.m_data, packet.GetPacketSize());
	if (0 == addLen)
	{
		if (m_isLogIgnorePacket)
			common::dbg::ErrLog("packetqueue Push alloc error!! \n");
	}
	return 0 != addLen;
}


// ��Ʈ��ũ�κ��� �� ��Ŷ�� ��� ���.
// �������� ������ ��Ŷ�� ó���� �� ���.
// *data�� �ΰ� �̻��� ��Ŷ�� ������ ���� ��� ����.
// senderId : ��Ŷ�� �۽��� ������ network id
// return true : ����
//		  false : ����
bool cPacketQueue::PushFromNetwork(const netid senderId, const BYTE *data, const int len)
{
	cAutoCS cs(m_cs);

	cSocketBuffer *sockBuff = NULL;
	auto it = m_sockBuffers.find(senderId);
	if (m_sockBuffers.end() == it)
	{
		sockBuff = new cSocketBuffer(senderId, m_sockBufferSize);
		m_sockBuffers.insert({ senderId, sockBuff });
	}
	else
	{
		sockBuff = it->second;
	}

	const uint addLen = sockBuff->Push(m_netNode->GetPacketHeader(), data, len);
	if (0 == addLen)
	{
		if (m_isLogIgnorePacket)
			common::dbg::ErrLog("packetqueue PushFromNetwork alloc error!! \n");
	}

	return true;
}


bool cPacketQueue::Front(OUT cPacket &out)
{
	RETV(m_sockBuffers.empty(), false);

	cAutoCS cs(m_cs);

	if (m_nextFrontIdx >= (int)m_sockBuffers.m_seq.size())
		m_nextFrontIdx = 0;
	
	cSocketBuffer *sockBuff = m_sockBuffers.m_seq[m_nextFrontIdx++];
	out.m_sndId = sockBuff->m_netId;
	const bool result = sockBuff->Pop(out);
	
	// write packet log
	if (m_isPacketLog && result)
		network2::LogPacket(m_netNode->m_id, out);

	return result;
}


void cPacketQueue::SendAll(
	const map<netid, SOCKET> &socks
	, OUT set<netid> *outErrSocks //= NULL
)
{
	RET(m_sockBuffers.empty());

	cAutoCS cs(m_cs);

	bool isSendError = false;
	for (u_int i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];

		SOCKET sock = INVALID_SOCKET;
		if (ALL_NETID != sockBuffer->m_netId)
		{
			auto it = socks.find(sockBuffer->m_netId);
			if (it == socks.end())
			{
				// Already Close Socket, no problem
			}
			else
			{
				sock = it->second;
			}
		}

		while (1)
		{
			cPacket packet(m_netNode->GetPacketHeader());
			if (!sockBuffer->PopNoRemove(packet))
				break;

			if (ALL_NETID == sockBuffer->m_netId)
			{
				const bool result = SendBroadcast(socks, packet, outErrSocks);
				if (!result)
				{
					// error!!, no remove buffer, resend
					dbg::Logc(2, "Error Send Packet\n");
					isSendError = true;
					break;
				}

				// write packet log
				if (m_isPacketLog)
					network2::LogPacket(m_netNode->m_id, packet);
			}
			else
			{
				int result = 0;
				if (INVALID_SOCKET != sock)
				{
					result = send(sock, (const char*)packet.m_data, packet.GetPacketSize(), 0);
					if (result != packet.GetPacketSize())
					{
						// error!!, no remove buffer, resend
						dbg::Logc(2, "Error Send Packet\n");
						isSendError = true;
						break;
					}

					// write packet log
					if (m_isPacketLog && (result != SOCKET_ERROR))
						network2::LogPacket(m_netNode->m_id, packet);
				}

				if (outErrSocks && (result == SOCKET_ERROR))
				{
					// error!!, close socket
					outErrSocks->insert(sockBuffer->m_netId);
				}
			} //~else ALL_NETID

			sockBuffer->Pop(packet.m_writeIdx);

		} // ~while
	} // ~for

	if (isSendError)
	{
		// send ������ �߻��ϸ�, �����̸� �༭ �ٸ� ������� ����Ī�� �� �ְ��Ѵ�.
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


// udpclient send all
void cPacketQueue::SendAll(const sockaddr_in &sockAddr)
{
	RET(m_sockBuffers.empty());

	cAutoCS cs(m_cs);
	for (u_int i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];
		const SOCKET sock = m_netNode->GetSocket(sockBuffer->m_netId);
		while (1)
		{
			cPacket packet(m_netNode->GetPacketHeader());
			if (!sockBuffer->Pop(packet))
				break;

			if (INVALID_SOCKET == sock)
			{
				assert(0);
			}
			else
			{
				sendto(sock, (const char*)packet.m_data, packet.GetPacketSize()
					, 0, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
			}
		}
	}
}


// exceptOwner �� true �϶�, ��Ŷ�� ���� Ŭ���̾�Ʈ�� ������ ������ Ŭ���̾�Ʈ�鿡�� ���
// ��Ŷ�� ������.
bool cPacketQueue::SendBroadcast(const vector<cSession*> &sessions, const bool exceptOwner)
{
	cAutoCS cs(m_cs);

	for (uint i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];
		while (1)
		{
			cPacket packet(m_netNode->GetPacketHeader());
			if (!sockBuffer->Pop(packet))
				break;

			for (u_int k = 0; k < sessions.size(); ++k)
			{
				// exceptOwner�� true�� ��, ��Ŷ�� �� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
				const bool isSend = !exceptOwner 
					|| (exceptOwner && (sockBuffer->m_netId != sessions[k]->m_id));
				if (isSend)
					send(sessions[k]->m_socket, (const char*)packet.m_data, packet.GetPacketSize(), 0);
			}
		}
	}

	return true;
}


// return false if send error
bool cPacketQueue::SendBroadcast(const map<netid, SOCKET> &socks
	, const cPacket &packet
	, OUT set<netid> *outErrSocks //= NULL
)
{
	bool isSendError = false;
	for (auto &kv : socks)
	{
		if (kv.first == SERVER_NETID)
			continue;

		const SOCKET sock = kv.second;
		const int result = send(sock, (const char*)packet.m_data, packet.GetPacketSize(), 0);

		if (outErrSocks && (result == SOCKET_ERROR))
		{
			isSendError = true;
			outErrSocks->insert(kv.first);
		}
	}
	return !isSendError;
}


void cPacketQueue::Lock()
{
	EnterCriticalSection(&m_cs);
}


void cPacketQueue::Unlock()
{
	LeaveCriticalSection(&m_cs);
}


void cPacketQueue::Clear()
{
	cAutoCS cs(m_cs);
	for (auto sock : m_sockBuffers.m_seq)
		delete sock;
	m_sockBuffers.clear();
	m_nextFrontIdx = 0;
}
