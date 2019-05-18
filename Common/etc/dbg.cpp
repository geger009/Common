
#include "stdafx.h"
#include "dbg.h"
#include <chrono>


namespace common {
	namespace dbg {

		// Log Thread
		common::cWQSemaphore g_logThread;
		struct sLogData
		{
			int type; // 0:log, 1:error log, 2:log + error log
			Str256 str;
		};

		StrPath g_logPath = "log.txt";
		StrPath g_errLogPath = "errlog.txt";
	}
}

using namespace common;
using namespace dbg;


//------------------------------------------------------------------------
// LogThread Task
class cLogTask : public cTask
				, public common::cMemoryPool4<cLogTask>
{
public:
	sLogData m_logData;
	cLogTask() : cTask(0, "cLogTask") {}
	cLogTask(const sLogData &logData) 
		: cTask(0, "cLogTask"), m_logData(logData) {
	}
	virtual ~cLogTask() {
	}

	virtual eRunResult::Enum Run(const double deltaSeconds) override
	{
		const string timeStr = common::GetCurrentDateTime();

		std::ofstream ofs1(g_logPath.c_str(), std::ios::app);
		std::ofstream ofs2(g_errLogPath.c_str(), std::ios::app);
		switch (m_logData.type)
		{
		case 0:
			if (ofs1.is_open())
			{
				ofs1 << timeStr << " : ";
				ofs1 << m_logData.str.c_str();
			}
			break;
		case 1:
			if (ofs2.is_open())
			{
				ofs2 << timeStr << " : ";
				ofs2 << m_logData.str.c_str();
			}
			break;
		case 2:
			if (ofs1.is_open())
			{
				ofs1 << timeStr << " : ";
				ofs1 << m_logData.str.c_str();
			}
			if (ofs2.is_open())
			{
				ofs2 << timeStr << " : ";
				ofs2 << m_logData.str.c_str();
			}
			break;
		}
		return eRunResult::END;
	}
};
//-----------------------------------------------------------------------


//------------------------------------------------------------------------
// Set Log File Path
// ex) "./server/"  --> "./server/log.txt"
// ex) "./client/" --> "./client/log.txt"
//------------------------------------------------------------------------
void dbg::SetLogPath(const char *path)
{
	g_logPath = StrPath(path) + g_logPath;
}


//------------------------------------------------------------------------
// Set ErrLog File Path
// ex) "./server/"  --> "./server/errlog.txt"
// ex) "./client/" --> "./client/errlog.txt"
//------------------------------------------------------------------------
void dbg::SetErrLogPath(const char *path)
{
	g_errLogPath = StrPath(path) + g_errLogPath;
}


//------------------------------------------------------------------------
// ���â�� ��Ʈ���� ����Ѵ�.
//------------------------------------------------------------------------
void dbg::Print( const std::string &str)
{
	OutputDebugStringA(str.c_str());
	OutputDebugStringA("\n");
}


//------------------------------------------------------------------------
// ���â�� ��Ʈ���� ����Ѵ�. (�������� ���)
//------------------------------------------------------------------------
void dbg::Print( const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );
	OutputDebugStringA(textString);
	//OutputDebugStringA("\n");
}


void dbg::Log(const char* fmt, ...)
{
	char textString[256];
	ZeroMemory(textString, sizeof(textString));

	va_list args;
	va_start(args, fmt);
	vsnprintf_s(textString, sizeof(textString)-1, _TRUNCATE, fmt, args);
	va_end(args);

	std::ofstream ofs(g_logPath.c_str(), std::ios::app);
	if (ofs.is_open())
		ofs << textString;
}


// log parallel thread
void dbg::Logp(const char* fmt, ...)
{
	sLogData data;
	data.type = 0;
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(data.str.m_str, sizeof(data.str.m_str) - 1, _TRUNCATE, fmt, args);
	va_end(args);

	//------------------------------------------------------------------------
	// add string to log thread
	{
		cLogTask *task = new cLogTask();
		task->m_logData = data;
		g_logThread.PushTask(task);
	}
}


// fileName �� ���Ͽ� �α׸� �����.
void dbg::Log2(const char *fileName, const char* fmt, ...)
{
	char textString[256] = { '\0' };
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(textString, sizeof(textString), _TRUNCATE, fmt, args);
	va_end(args);

	FILE *fp = fopen(fileName, "a+");
	if (fp)
	{
		fputs(textString, fp);
		fclose(fp);
	}
}


void dbg::ErrLog(const char* fmt, ...)
{
	char textString[256] = { '\0' };
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(textString, sizeof(textString), _TRUNCATE, fmt, args);
	va_end(args);

	FILE *fp = fopen(g_errLogPath.c_str(), "a+");
	if (fp)
	{
		fputs(textString, fp);
		fclose(fp);
	}

	// �α����Ͽ��� ���� �޼����� �����Ѵ�.
	Log( "Error : %s", textString);
}


// errlog parallel 
void dbg::ErrLogp(const char* fmt, ...)
{
	sLogData data;
	data.type = 1;
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(data.str.m_str, sizeof(data.str.m_str), _TRUNCATE, fmt, args);
	va_end(args);

	//------------------------------------------------------------------------
	// add string to log thread
	{
		cLogTask *task = new cLogTask();
		task->m_logData = data;
		g_logThread.PushTask(task);
	}

	// �α����Ͽ��� ���� �޼����� �����Ѵ�.
	Logp("Error : %s", data.str.m_str);
}


void dbg::RemoveErrLog()
{
	FILE *fp = fopen(g_errLogPath.c_str(), "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}


void dbg::RemoveLog2(const char *fileName)
{
	FILE *fp = fopen(fileName, "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}


void dbg::RemoveLog()
{
	FILE *fp = fopen(g_logPath.c_str(), "w");
	if (fp)
	{
		fputs("", fp);
		fclose(fp);
	}
}


void dbg::TerminateLogThread()
{
	g_logThread.Clear();
}


// log classfy
// print/log/errlog, multithread
//
// level 0 : printf
//		 1 : printf + log
//		 2 : printf + log + err log
//		 3 : printf + log + err log + assertion
void dbg::Logc(const int level, const char* fmt, ...)
{
	sLogData data;
	data.type = -1;
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(data.str.m_str, sizeof(data.str.m_str), _TRUNCATE, fmt, args);
	va_end(args);

	switch (level)
	{
	case 3:
		//DebugBreak();
		assert(!"dbg::Logc()");
	case 2:
		data.type = 2;
	case 1:
		if (level == 1)
			data.type = 0;
	case 0:
		// cout �� ȭ���� Freeze �������� ����� �ֱ� ������ ���ܵ�
		//std::cout << data.str.m_str;
		break;
	default:
		assert(0);
		break;
	}

	//------------------------------------------------------------------------
	// add string to log thread
	if (data.type >= 0)
	{
		cLogTask *task = new cLogTask();
		task->m_logData = data;
		g_logThread.PushTask(task);
	}
}
