#pragma once

#include "Process.h"
#include <iostream>

namespace Client
{
	static const unsigned int g_iWinSizeX = 1600;
	static const unsigned int g_iWinSizeY = 900;

	static const unsigned int g_iMouseSizeX = 32;
	static const unsigned int g_iMouseSizeY = 32;
}

/* Headers */
#include "Client_Enum.h"
#include "Client_Macro.h"
#include "Client_Struct.h"

#include <Vfw.h>
#pragma comment (lib, "Vfw32.lib")

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance; // DInput¿¡ ÇÊ¿ä

/* Namespace */
using namespace		std;
using namespace		Client;

/* Show Console */
#ifdef _DEBUG
#ifdef UNICODE    
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else                                                                                                    
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")   
#endif                                                                                                   
#endif