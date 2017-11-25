
#include "stdafx.h"
#include "dbgmonitor.h"


using namespace framework;


cDbgMonitor::cDbgMonitor()
	: m_sharedData(NULL)
	, m_mutex("DebugMonitorMutex::jjuiddong")
{
}

cDbgMonitor::~cDbgMonitor()
{
}


bool cDbgMonitor::Create(graphic::cRenderer &renderer, const StrId &sharedMemoryName)
{
	if (!m_shmem.Init(sharedMemoryName.c_str(), sizeof(sSharedData)))
	{
		//assert(0);
		return false;
	}

	m_mutex.Lock();
	m_sharedData = (sSharedData *)m_shmem.m_memPtr;
	m_sharedData->fps = 0;
	m_sharedData->dtVal = 1; // ��ũ�� Ƣ��� ����
	m_sharedData->isDbgRender = renderer.m_isDbgRender;
	m_sharedData->dbgRenderStyle = renderer.m_dbgRenderStyle;
	m_mutex.Unlock();

	return true;
}


// ���ø����̼ǿ��� ȣ���ϴ� ������Ʈ �Լ�
void cDbgMonitor::UpdateApp(graphic::cRenderer &renderer)
{
	m_mutex.Lock();
	m_sharedData->fps = ImGui::GetIO().Framerate;
	renderer.m_isDbgRender = m_sharedData->isDbgRender;
	renderer.m_dbgRenderStyle = m_sharedData->dbgRenderStyle;
	m_mutex.Unlock();
}


// DebugMonitor ���� ȣ���ϴ� ������Ʈ �Լ�
void cDbgMonitor::UpdateTool(graphic::cRenderer &renderer)
{
}


void cDbgMonitor::Lock()
{
	m_mutex.Lock();
}


void cDbgMonitor::Unlock()
{
	m_mutex.Unlock();
}
