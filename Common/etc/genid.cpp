
#include "stdafx.h"
#include "genid.h"


namespace common
{
	std::atomic_int g_seedId = 10000;
}

using namespace common;


// ���̵� ó������ �����Ѵ�.
void common::SetSeedId(const int seed)
{
	g_seedId = seed;
}


// �ߺ����� �ʴ� ���̵� ���� ������ �����Ѵ�.
// g_seedId ���� �ִ�ġ�� �Ѿ�� ������ �߻��� �� �ִ�.
// �̿� ���� ó���� ���� ����.
int common::GenerateId()
{
	return g_seedId++;
}
