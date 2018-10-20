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
			WAIT
			, RUN
			, STOP
		};
	};

	struct eActionType {
		enum Enum {
			NONE
			, ROOT
			
			, WAIT
			, MOVE
			, GROUP_MOVE
			, ATTACK
			, PATROL
			
			,MAX_ACTION_TYPE0,
		};
	};

	struct eMsgType {
		enum Enum {
			NONE

			, UNIT_SIDEMOVE
				// unit - unit collision message
		};
	};


	struct sMsg
	{
		cObject *receiver;
		cObject *sender;
		eMsgType::Enum msg;
		int param1;
		int param2;
		Vector3 v;
		int sendTime;
		//char comment[32];

		sMsg(
			cObject *rcv = NULL
			, cObject *snd = NULL
			, eMsgType::Enum msg0 = eMsgType::NONE
			, int parameter1 = 0
			, int parameter2 = 0
			, const Vector3 &v0 = Vector3(0,0,0)
			, int time = 0
		) 
			: receiver(rcv)
			, sender(snd)
			, msg(msg0)
			, param1(parameter1)
			, param2(parameter2)
			, v(v0)
			, sendTime(time)
		{
			//comment[0]=NULL;
		}
	};

}
