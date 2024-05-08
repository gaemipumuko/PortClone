#pragma once

#include "Process.h"
#include <iostream>

#include "Animation_Enum.h"
#include "Animation_Macro.h"

namespace AnimationTool
{
	enum LEVELID { LEVEL_STATIC, LEVEL_EDITOR, LEVEL_LOADING, LEVEL_END };

	static const unsigned int g_iWinSizeX = 1600;
	static const unsigned int g_iWinSizeY = 900;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace		std;
using namespace		AnimationTool;