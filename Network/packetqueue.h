//
// 2015-11-29, jjuiddong
//
// - ��Ŷ�� �����Ѵ�. Thread Safe, �ִ��� �����ϰ� �������.
// - ��Ʈ�������� �� ����Ÿ�� ������ ��Ŷ ũ�⸸ŭ ������ �����Ѵ�.
// - ť�� ��������, ���� ������ ��Ŷ�� �����ϰ�, �߰��Ѵ�.
// - �� ���ϸ��� packetSize ũ�⸸ŭ ä�� ������ �������� �Ѿ�� �ʴ´�.
// - ��Ŷ�� ť�� ������ ��, ���(sHeader)�� �߰��ȴ�.
// - ���ϸ��� �ϳ� �̻��� ť�� ���� �� �ִ�.
//
// 2016-06-29, jjuiddong
//		- sHeader�� ���� ��Ŷ�� ���� �� �ְ���
//		- Init(),  isIgnoreHeader �÷��� �߰�
//
// 2016-05-26, jjuiddong
//		- ū ��Ŷ�� �������� �� ��� sHeader ������ ����, ��Ŷ�� �������� �״´�.
//		- ��Ʈ��ũ�� ���� �� ��Ŷ�� 2�� �̻��� ���������� ������ ���, ������ ó���Ѵ�.
//
// 2016-09-24, jjuiddong
//		- �޸�Ǯ�� ��� ����, ������ ��Ŷ�� �����Ѵ�.
//
// 2018-11-07, jjuiddong
//		- header ����
//		- [0 ~ 3] : protocol ascii code
//		- [4 ~ 7] : packet bytes length by ascii (ex 0085, 85 bytes)
//
//
#pragma once


namespace network
{
	// �� ��Ŷ�� ������ �����Ѵ�.
	// �ϳ��� ��Ŷ�� �������� �������� ��, �� sSockBuffer �� ��� ����ȴ�.
	struct sSockBuffer
	{
		SOCKET sock; // ���� ����
		char protocol[4];
		BYTE *buffer;
		int totalLen; // = sizeof(sHeader) + buffer size
		bool full; // ���۰� �� ä������ true�� �ȴ�.
		int readLen;
		int actualLen; // ���� ��Ŷ�� ũ�⸦ ��Ÿ����. buffer size(bytes) {= totalLen - sizeof(sHeader)}
	};

	struct sSession;
	class cPacketQueue
	{
	public:
		cPacketQueue();
		virtual ~cPacketQueue();

		struct sHeader
		{
			BYTE protocol[4];
			BYTE packetLength[4]; // ascii packet length, ex) 0085
		};

		bool Init(const int packetSize, const int maxPacketCount);
		void Push(const SOCKET sock, const char protocol[4], const BYTE *data, const int len);
		void PushFromNetwork(const SOCKET sock, const BYTE *data, const int len);
		bool Front(OUT sSockBuffer &out);
		void Pop();
		void SendAll();
		void SendAll(const sockaddr_in &sockAddr);
		void SendBroadcast(vector<sSession> &sessions, const bool exceptOwner = true);
		void Lock();
		void Unlock();
		int GetSize();
		int GetPacketSize();
		int GetMaxPacketCount();

		vector<sSockBuffer> m_queue;

		// �ӽ� ����
		BYTE *m_tempHeaderBuffer; // �ӽ÷� Header �����ϴ� ����
		int m_tempHeaderBufferSize;
		bool m_isStoreTempHeaderBuffer; // �ӽ÷� �����ϰ� ���� �� true
		BYTE *m_tempBuffer; // �ӽ÷� ����� ����
		int m_tempBufferSize;
		bool m_isLogIgnorePacket; // ������ ��Ŷ �α׸� ������ ����, default = false


	protected:
		sSockBuffer* FindSockBuffer(const SOCKET sock);
		int CopySockBuffer(sSockBuffer *dst, const BYTE *data, const int len);
		int AddSockBuffer(const SOCKET sock, const char protocol[4]
			, const BYTE *data, const int len);
		int AddSockBufferByNetwork(const SOCKET sock, const BYTE *data, const int len);
		sHeader MakeHeader(const char protocol[4], const int len);
		sHeader GetHeader(const BYTE *data, OUT int &byteSize);

		//---------------------------------------------------------------------
		// Simple Queue Memory Pool
		BYTE* Alloc();
		void Free(BYTE*ptr);
		void ClearMemPool();
		void Clear();

		struct sChunk
		{
			bool used;
			BYTE *p;
		};
		vector<sChunk> m_memPool;
		BYTE *m_memPoolPtr;
		int m_packetBytes; // sHeader ����� ������ ��Ŷ ũ��
		int m_chunkBytes; // ������ ��Ŷ ũ�� (actual size)
		int m_totalChunkCount;
		CRITICAL_SECTION m_criticalSection;
	};


	inline int cPacketQueue::GetPacketSize() { return m_packetBytes;  }
	inline int cPacketQueue::GetMaxPacketCount() { return m_totalChunkCount; }
}
