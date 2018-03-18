
#pragma once


namespace framework
{
	using namespace common;

	// ������ ����
	HWND InitWindow(HINSTANCE hInstance, 
		const wstring &windowName, 
		const sRectf &windowRect,
		int nCmdShow,
		WNDPROC WndProc,
		const bool dualMonitor = true
	);

}
