
#include "stdafx.h"
#include "BufferedSerial.h"

using namespace common;


cBufferedSerial::cBufferedSerial()
{
}

cBufferedSerial::~cBufferedSerial()
{
}


// ch���ڰ� ���� ������ �ø��������� �о �����Ѵ�.
// ch���ڰ� ���ٸ� ���ڿ��� �����Ѵ�.
bool cBufferedSerial::ReadStringUntil(const char ch, OUT char *out
	, OUT int &outLen, const int maxSize)
{
	if (m_q.full())
	{
		m_q.frontPop(out, maxSize);
		outLen = maxSize;
		return true;
	}

	char buffer[MAX_BUFFERSIZE];
	const int readBytes = ReadData(buffer, sizeof(buffer));

	if (readBytes <= 0)
		return false;

	m_q.push(buffer, readBytes);

	bool isFind = false;
	const uint cnt = m_q.size();
	int front = m_q.m_front;
	uint i = 0;
	while (i++ < cnt)
	{
		if (ch == m_q.m_datas[front])
		{
			isFind = true;
			break;
		}
		front = (front + 1) % m_q.SIZE;
	}

	if (!isFind)
		return false;

	m_q.frontPop(out, i);
	outLen = (int)i;
	return true;
}


// ������ �ʱ�ȭ.
void cBufferedSerial::ClearBuffer()
{
	m_q.clear();
}
