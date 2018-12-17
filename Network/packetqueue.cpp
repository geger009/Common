#include "stdafx.h"
#include "packetqueue.h"
#include "session.h"

using namespace network;


cPacketQueue::cPacketQueue()
	: m_memPoolPtr(NULL)
	, m_chunkBytes(0)
	, m_packetBytes(0)
	, m_totalChunkCount(0)
	, m_tempHeaderBuffer(NULL)
	, m_tempBuffer(NULL)
	, m_isStoreTempHeaderBuffer(false)
	, m_isLogIgnorePacket(false)
{
	InitializeCriticalSectionAndSpinCount(&m_criticalSection, 0x00000400);
}

cPacketQueue::~cPacketQueue()
{
	Clear();

	DeleteCriticalSection(&m_criticalSection);
}


bool cPacketQueue::Init(const int packetSize, const int maxPacketCount)
{
	Clear();

	// Init Memory pool
	m_packetBytes = packetSize + sizeof(sHeader);
	m_chunkBytes = packetSize;
	m_totalChunkCount = maxPacketCount;
	//m_memPoolPtr = new BYTE[(packetSize+sizeof(sHeader)) * maxPacketCount];
	m_memPool.reserve(maxPacketCount);
	for (int i = 0; i < maxPacketCount; ++i)
	{
		m_memPool.push_back({ false, new BYTE[ (packetSize + sizeof(sHeader)) ] });
		//m_memPool.push_back({ false, m_memPoolPtr + (i*(packetSize + sizeof(sHeader))) });
	}
	//

	m_queue.reserve(maxPacketCount);
	m_tempBuffer = new BYTE[packetSize + sizeof(sHeader)];
	m_tempBufferSize = packetSize + sizeof(sHeader);
	m_tempHeaderBuffer = new BYTE[sizeof(sHeader)];
	m_tempHeaderBufferSize = 0;

	return true;
}


// ��Ŷ�� ť�� �����Ѵ�.
// data�� ��Ȯ�� ��Ŷ ũ�⸸ŭ�� ������ �������Ѵ�.
// �ɰ��� ��Ŷ�� ��ġ�� �ڵ�� ����.
// ��Ʈ��ũ�� ���� �޾Ƽ� �����ϴ°� �ƴ϶�, ����ڰ� ���� ��Ŷ�� ť�� ���� ��
// ����ϴ� �Լ���.
void cPacketQueue::Push(const SOCKET sock, const char protocol[4]
	, const BYTE *data, const int len)
{
	cAutoCS cs(m_criticalSection);

	int totalLen = len;
	int offset = 0;
	while (offset < totalLen)
	{
		const BYTE *ptr = data + offset;
		const int size = totalLen - offset;
		if (sSockBuffer *buffer = FindSockBuffer(sock))
		{
			offset += CopySockBuffer(buffer, ptr, size);
		}
		else
		{
			int addLen = AddSockBuffer(sock, protocol, ptr, size);
			if (0 == addLen)
			{
				if (m_isLogIgnorePacket)
					common::dbg::ErrLog("packetqueue push Alloc error!! \n");
				break;
			}

			offset += addLen;
		}
	}
}


// ��Ʈ��ũ�� ���� �� ��Ŷ�� ���,
// �������� ������ ��Ŷ�� ó���� ���� ȣ���� �� �ִ�.
// *data�� �ΰ� �̻��� ��Ŷ�� ������ ����, �̸� ó���Ѵ�.
void cPacketQueue::PushFromNetwork(const SOCKET sock, const BYTE *data, const int len)
{
	cAutoCS cs(m_criticalSection);

	int totalLen = len;

	// ���� ������ ���۰� ���� ��.. 
	// �ӽ÷� �����ص״� ������ ���ڷ� �Ѿ�� ������ ���ļ� ó���Ѵ�.
	if (m_isStoreTempHeaderBuffer)
	{
		if (m_tempBufferSize < (len + m_tempHeaderBufferSize))
		{
			SAFE_DELETEA(m_tempBuffer);
			m_tempBuffer = new BYTE[len + m_tempHeaderBufferSize];
			m_tempBufferSize = len + m_tempHeaderBufferSize;
		}

		memcpy(m_tempBuffer, m_tempHeaderBuffer, m_tempHeaderBufferSize);
		memcpy(m_tempBuffer + m_tempHeaderBufferSize, data, len);	
		m_isStoreTempHeaderBuffer = false;

		data = m_tempBuffer;
		totalLen = m_tempHeaderBufferSize + len;
	}

	int offset = 0;
	while (offset < totalLen)
	{
		const BYTE *ptr = data + offset;
		const int size = totalLen - offset;
		if (sSockBuffer *buffer = FindSockBuffer(sock))
		{
			offset += CopySockBuffer(buffer, ptr, size);
		}
		else
		{
			// ���� ������ ����Ÿ�� sHeader ���� ���� ��, �ӽ÷� 
			// ������ ��, ������ ��Ŷ�� ���� �� ó���Ѵ�.
			if (size < sizeof(sHeader))
			{
				m_tempHeaderBufferSize = size;
				m_isStoreTempHeaderBuffer = true;
				memcpy(m_tempHeaderBuffer, ptr, size);
				offset += size;
			}
			else
			{
				int addLen = AddSockBufferByNetwork(sock, ptr, size);
				if (0 == addLen)
				{
					if (m_isLogIgnorePacket)
						common::dbg::ErrLog("packetqueue push Alloc error!! \n");
					break;
				}

				offset += addLen;
			}
		}
	}
}


// ũ��Ƽ�ü����� ���� ������ �Ŀ� ȣ������.
// sock �� �ش��ϴ� ť�� �����Ѵ�. 
sSockBuffer* cPacketQueue::FindSockBuffer(const SOCKET sock)
{
	RETV(m_queue.empty(), NULL);

	// find specific packet by socket
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (sock != m_queue[i].sock)
			continue;
		// �ش� ������ ä������ ���� ���۶��, �� ���ۿ� ä��� �Ѵ�.
		if (m_queue[i].readLen < m_queue[i].totalLen)
			return &m_queue[i];
	}

	// fastmode?
	// �ִ� ��Ŷ ���� 1���� ������ ���, ���� �ϳ��� ������Ʈ�ϴ� ������� �۵��Ѵ�.
	if (m_totalChunkCount == 1)
	{
		m_queue.front().readLen = 0;
		return &m_queue.front();
	}

	return NULL;
}


// cSockBuffer *dst�� data �� len ��ŭ �����Ѵ�.
// �� ��, len�� SockBuffer�� ����ũ�� ��ŭ �����Ѵ�.
// ���� �� ũ�⸦ ����Ʈ ������ �����Ѵ�.
int cPacketQueue::CopySockBuffer(sSockBuffer *dst, const BYTE *data, const int len)
{
	RETV(!dst, 0);
	RETV(!dst->buffer, 0);

	const int copyLen = min(dst->totalLen - dst->readLen, len);
	memcpy(dst->buffer + dst->readLen, data, copyLen);
	dst->readLen += copyLen;

	if (dst->readLen == dst->totalLen)
		dst->full = true;

	return copyLen;
}


// �� ��Ŷ ���۸� �߰��Ѵ�.
int cPacketQueue::AddSockBuffer(const SOCKET sock, const char protocol[4]
	, const BYTE *data, const int len)
{
	// ���� �߰��� ������ ��Ŷ�̶��
	sSockBuffer sockBuffer;
	sockBuffer.protocol[0] = protocol[0];
	sockBuffer.protocol[1] = protocol[1];
	sockBuffer.protocol[2] = protocol[2];
	sockBuffer.protocol[3] = protocol[3];
	sockBuffer.sock = sock;
	sockBuffer.totalLen = 0;
	sockBuffer.readLen = 0;
	sockBuffer.full = false;

	// �۽��� ��Ŷ�� �߰��� ���, or ����� ���� ��Ŷ�� ���� ���.
	// ��Ŷ ����� �߰��Ѵ�.
	sockBuffer.totalLen = len + sizeof(sHeader);
	sockBuffer.actualLen = len;

	if ((sockBuffer.totalLen < 0) || (sockBuffer.actualLen < 0))
	{
		common::dbg::ErrLog("packetqueue push error!! canceled 2.\n");
		return len;
	}

	sockBuffer.buffer = Alloc();

	if (!sockBuffer.buffer)
		return 0; // Error!! 

	sHeader header = MakeHeader(protocol, len + sizeof(sHeader));
	CopySockBuffer(&sockBuffer, (BYTE*)&header, sizeof(sHeader));

	const int copyLen = CopySockBuffer(&sockBuffer, data, len);
	m_queue.push_back(sockBuffer);

	// ��Ŷ��� ũ��� ����(�����ϰ� data���� ����� ũ�⸦ �����Ѵ�.)
	return copyLen;
}


// �� ��Ŷ ���۸� �߰��Ѵ�.
int cPacketQueue::AddSockBufferByNetwork(const SOCKET sock, const BYTE *data, const int len)
{
	// ��Ʈ��ũ�� ���� �� ��Ŷ�̶��, �̹� ��Ŷ����� ���Ե� ���´�.
	// ��ü ��Ŷ ũ�⸦ �����ؼ�, �и��� ��Ŷ������ �Ǵ��Ѵ�.
	int byteSize = 0;
	const sHeader header = GetHeader(data, byteSize);
	if (isalpha(header.protocol[0])
		&& isalpha(header.protocol[1])
		&& isalpha(header.protocol[2])
		&& isalpha(header.protocol[3])
		&& (byteSize > sizeof(sHeader)) && (byteSize <= m_packetBytes))
	{
		// nothing~ continue~
	}
	else
	{
		// error occur!!
		// ��Ŷ�� ���ۺΰ� �ƴѵ�, ���ۺη� ������.
		// ����ΰ� �����ų�, ���� ���۰� Pop �Ǿ���.
		// �����ϰ� �����Ѵ�.
		common::dbg::ErrLog("packetqueue push error!! packet header not found\n");
		return len;
	}

	// ����� ������ ����Ÿũ��
	const int actualDataSize = byteSize - sizeof(sHeader);

	// ��Ʈ��ũ�κ��� ���� ��Ŷ�� ����κ��� ������ ���� ����Ÿ�� �����Ѵ�.
	sSockBuffer sockBuffer;
	sockBuffer.sock = sock;
	sockBuffer.protocol[0] = header.protocol[0];
	sockBuffer.protocol[1] = header.protocol[1];
	sockBuffer.protocol[2] = header.protocol[2];
	sockBuffer.protocol[3] = header.protocol[3];
	sockBuffer.readLen = 0;
	sockBuffer.totalLen = actualDataSize;
	sockBuffer.actualLen = actualDataSize; 
	sockBuffer.buffer = Alloc();
	sockBuffer.full = false;

	if (!sockBuffer.buffer)
		return 0; // Error!! 

	if ((sockBuffer.totalLen < 0) || (sockBuffer.actualLen < 0))
	{
		common::dbg::ErrLog("packetqueue push error!! canceled 2.\n");
		return len;
	}

	// ��� ������ ����Ÿ�� �����Ѵ�.
	const int copyLen = CopySockBuffer(&sockBuffer, data + sizeof(sHeader)
		, min(len - (int)sizeof(sHeader), actualDataSize));
	m_queue.push_back(sockBuffer);

	// ��Ŷ��� ũ����� ����.
	return copyLen + sizeof(sHeader);
}


// �������ü�� �����Ѵ�.
cPacketQueue::sHeader cPacketQueue::MakeHeader(const char protocol[4], const int len)
{
	sHeader header;
	header.protocol[0] = protocol[0];
	header.protocol[1] = protocol[1];
	header.protocol[2] = protocol[2];
	header.protocol[3] = protocol[3];

	header.packetLength[0] = min(9, len / 1000) + '0';
	header.packetLength[1] = min(9, (len % 1000) / 100) + '0';
	header.packetLength[2] = min(9, (len % 100) / 10) + '0';
	header.packetLength[3] = min(9,  len % 10) + '0';

	return header;
}


// ����Ʈ ��Ʈ�����κ��� ��� ����ü�� �����Ѵ�.
cPacketQueue::sHeader cPacketQueue::GetHeader(const BYTE *data, OUT int &byteSize)
{
	sHeader header = *(sHeader*)data;

	byteSize = (header.packetLength[0] - '0') * 1000
		+ (header.packetLength[1] - '0') * 100
		+ (header.packetLength[2] - '0') * 10
		+ (header.packetLength[3] - '0') * 1;

	return header;
}


bool cPacketQueue::Front(OUT sSockBuffer &out)
{
	RETV(m_queue.empty(), false);

	cAutoCS cs(m_criticalSection);
	RETV(!m_queue[0].full, false);

	out = m_queue[0];
	out.buffer = m_queue[0].buffer; // ��Ʈ��ũ�κ��� ���� ��Ŷ�̶��, ����δ� ������ ����Ÿ�θ� �����Ѵ�.

	return true;
}


void cPacketQueue::Pop()
{
	cAutoCS cs(m_criticalSection);
	RET(m_queue.empty());

	Free(m_queue.front().buffer);
	common::rotatepopvector(m_queue, 0);
}


void cPacketQueue::SendAll(
	OUT vector<SOCKET> *outErrSocks //= NULL
)
{
	RET(m_queue.empty());

	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		const int result = send(m_queue[i].sock, (const char*)m_queue[i].buffer, m_queue[i].totalLen, 0);
		Free(m_queue[i].buffer);

		if (outErrSocks && (result == SOCKET_ERROR))
			outErrSocks->push_back(m_queue[i].sock);
	}
	m_queue.clear();
}


void cPacketQueue::SendAll(const sockaddr_in &sockAddr)
{
	RET(m_queue.empty());

	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		sendto(m_queue[i].sock, (const char*)m_queue[i].buffer, m_queue[i].totalLen,
			0, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
		Free(m_queue[i].buffer);
	}
	m_queue.clear();
}


// ť�� ����� ������ �� ũ�� (����Ʈ ����)
int cPacketQueue::GetSize()
{
	RETV(m_queue.empty(), 0);

	int size = 0;
	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
		size += m_queue[i].totalLen;
	return size;
}


// exceptOwner �� true �϶�, ��Ŷ�� ���� Ŭ���̾�Ʈ�� ������ ������ Ŭ���̾�Ʈ�鿡�� ���
// ��Ŷ�� ������.
void cPacketQueue::SendBroadcast(vector<sSession> &sessions, const bool exceptOwner)
{
	cAutoCS cs(m_criticalSection);

	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (!m_queue[i].full)
			continue; // �� ä������ ���� ��Ŷ�� ����

		for (u_int k = 0; k < sessions.size(); ++k)
		{
			// exceptOwner�� true�� ��, ��Ŷ�� �� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
			const bool isSend = !exceptOwner || (exceptOwner && (m_queue[i].sock != sessions[k].socket));
			if (isSend)
				send(sessions[k].socket, (const char*)m_queue[i].buffer, m_queue[i].totalLen, 0);
		}
	}

	// ��� ������ �� ť�� ����.
	for (u_int i = 0; i < m_queue.size(); ++i)
		Free(m_queue[i].buffer);
	m_queue.clear();

	ClearMemPool();
}


void cPacketQueue::Lock()
{
	EnterCriticalSection(&m_criticalSection);
}


void cPacketQueue::Unlock()
{
	LeaveCriticalSection(&m_criticalSection);
}


BYTE* cPacketQueue::Alloc()
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
	{
		if (!m_memPool[i].used)
		{
			m_memPool[i].used = true;
			return m_memPool[i].p;
		}
	}
	return NULL;
}


void cPacketQueue::Free(BYTE*ptr)
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
	{
		if (ptr == m_memPool[i].p)
		{
			m_memPool[i].used = false;
			break;
		}
	}
}


void cPacketQueue::Clear()
{
	SAFE_DELETEA(m_memPoolPtr);
	SAFE_DELETEA(m_tempBuffer);
	SAFE_DELETEA(m_tempHeaderBuffer);
	
	for (auto &pool : m_memPool)
	{
		SAFE_DELETEA(pool.p);
	}
	m_memPool.clear();


	m_queue.clear();
}


// �޸� Ǯ�� �ʱ�ȭ�ؼ�, ��¼�� ������ �� ���׸� �����Ѵ�.
void cPacketQueue::ClearMemPool()
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
		m_memPool[i].used = false;
}
