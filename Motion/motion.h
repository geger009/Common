//
// 2016-03-28, jjuiddong
// 
// Motion ó���� ���õ� ���̺귯������ �����Ѵ�.
// 

#pragma once


#include "../Common/common.h"
#include "../Network/network.h"
//#include "../CamCommon/camcommon.h"
using namespace common;


// BITCON Serial Communication Protocol
namespace VITCON_SER
{
	enum TYPE
	{
		START = 1,
		STOP = 2,
		ORIGIN = 5,
		EMERGENCY = 6,
		SERVOON = 7,
		SERVOOFF = 8,
	};
}



// Motion
#include "mixingconfig.h"
#include "modulator.h"
#include "motionwavemodulator.h"
#include "motionmonitorconfig.h"
#include "simplemodulator.h"
#include "motionwave.h"
//#include "joypad.h"


#include "script/script.h"

#include "udpinput.h"
#include "normalmixer.h"
#include "serialoutput.h"
#include "udpoutput.h"
#include "shmeminput.h"
#include "simplemodule.h"
#include "amplitudemodule.h"
#include "motioncontroller2.h"
#include "joystickinput.h"
