//
// Boost�� date ��ü�� �̿��ϸ� ��¥ ����� ���� �� �� �ִ�.
//
#pragma once


namespace common
{
	// year-month-day-hour-minutes-seconds-millseconds
	string GetCurrentDateTime();
	string GetCurrentDateTime2();


	class cDateTime
	{
	public:
		cDateTime();
		cDateTime(const string &strDate);
		virtual ~cDateTime();

		bool Parse(const char *str);


	public:
		BYTE m_year;
		BYTE m_month;
		BYTE m_day;
		BYTE m_hours;
		BYTE m_minutes;
		BYTE m_seconds;
	};

}
