#pragma once

#include "framework.h"
#include "Process.h"

namespace Tool
{
	static const unsigned int g_iWinSizeX = 1600;
	static const unsigned int g_iWinSizeY = 900;
}

#include "Tool_Enum.h"
#include "Tool_Struct.h"

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace		std;
using namespace		Tool;

#ifdef _DEBUG
#ifdef UNICODE    
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else                                                                                                    
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")   
#endif                                                                                                   
#endif
