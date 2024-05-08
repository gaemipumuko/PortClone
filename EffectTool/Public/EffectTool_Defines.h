#pragma once

#include "Process.h"

#include "EffectTool_Enum.h"
#include "EffectTool_Macro.h"

namespace EffectTool
{
	static const unsigned int g_iWinSizeX = 1600;
	static const unsigned int g_iWinSizeY = 900;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace		std;
using namespace		EffectTool;