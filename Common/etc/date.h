//
// Boost�� date ��ü�� �̿��ϸ� ��¥ ����� ���� �� �� �ִ�.
//
#pragma once


namespace common
{
	// year-month-day-hour-minutes-seconds-millseconds
	string GetCurrentDateTime();
	

	class cDateTime
	{
	public:
		cDateTime();
		cDateTime(const string &strDate);
		virtual ~cDateTime();


	public:
	};

}
