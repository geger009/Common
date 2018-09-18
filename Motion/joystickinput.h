//
// 2016-06-23, jjuiddong
// joystick input
//
// @xaxis, @yaxis, @zaxis,  @xrot, @yrot, @zrot, @slider �� ���̽�ƽ �Է��� ����ȴ�.
// @roll_mod, @pitch_mod, @yawl_mod, @heavel_mod �� ��� ���� ����ȴ�.
//
#pragma once

#include "input.h"


namespace motion
{

	class cJoyStickInput : public cInput
	{
	public:
		cJoyStickInput();
		virtual ~cJoyStickInput();

		enum DEVICE {JOYSTICK, JOYPAD};
		bool Init(const DEVICE device, const HWND hWnd, const string &mathStr, const string &modulatorScript);
		bool Init2(const DEVICE device, const HWND hWnd, const string &mathScriptFileName, const string &modulatorScriptFileName);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	public:
		DEVICE m_device;
		HWND m_hWnd;
		//cJoyPad m_joyPad;
		mathscript::cMathParser m_mathParser;
		mathscript::cMathInterpreter m_matInterpreter;
		cMotionWaveModulator m_modulator;
	};

}
