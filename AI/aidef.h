// �ΰ����ɿ� �ʿ��� Ÿ�Ե��� �����Ѵ�.
// ���� ���� ����Ǵ� �����̱� ������ �ΰ����ɿ��� ���̴� ��� Ŭ������
// ���⿡ ���ǵ� Ÿ�Ե��� ������ �� �� �ִ�.
#pragma once


namespace ai
{
	using namespace common;

	class cObject;

	struct eActionState {
		enum Enum {
			WAIT,
			RUN,
			STOP,
		};
	};

	struct eActionType {
		enum Enum {
			NONE,
			ROOT,

			MOVE, 
			GROUP_MOVE,
			ATTACK,
			PATROL,

			MAX_ACTION_TYPE0,
		};
	};


	struct sMsg
	{
		cObject *receiver;
		cObject *sender;
		int msg;
		int param1;
		int param2;
		int sendTime;
		char comment[ 32];

		sMsg() : param1(0) 
		{
			comment[0]=NULL;
		}

		sMsg(
			cObject *rcv, cObject *snd, int msg0,
			int parameter1, int parameter2, int time) 
			: receiver(rcv), sender(snd), msg(msg0), 
			param1(parameter1), param2(parameter2), 
			sendTime(time)
		{
			comment[0]=NULL;
		}
	};

}
