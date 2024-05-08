#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 26495)

/* Console Debug */
#ifdef _DEBUG
#include <iostream>
#endif

/* DX */
/* D3DX를 제외한 D3D 헤더들은 WindowsSDK 안에 설치되어 있기 때문에 포함 가능 */
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> // XM으로 시작하는 MathType 사용 가능
#include <DirectXCollision.h>

/* Shader (FX11) */
#include <FX11/d3dx11effect.h>

#ifdef _DEBUG
#pragma comment(lib, "FX11/Effects11d.lib")
#else
#pragma comment(lib, "FX11/Effects11.lib")
#endif

/* DInput */
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")

/* Texture (DirectXTK) */
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/PrimitiveBatch.h>		/* 충돌체 */
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>			/* 폰트 */
#include <DirectXTK/SpriteFont.h>			/* 폰트 */
#include <DirectXTK\ScreenGrab.h>
#include <DirectXTK/Effects.h>

#define D3DCOLOR_ABGR(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/DirectXTKd.lib")
#else
#pragma comment(lib, "DirectXTK/DirectXTK.lib")
#endif

using namespace DirectX;

/* STL Container */
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <tuple>
#include <list>
#include <map>
#include <set>

#include <string>

/* Random */
#include <chrono>
#include <random>

/* FMOD */
#include <io.h>
#include "FMOD/fmod.h"
#include "FMOD/fmod.hpp"
#pragma comment(lib, "FMOD/fmodL_vc.lib")

/* Json */
#include <locale>
#include <codecvt>
#include "Json/json.h"

#ifdef _DEBUG
#pragma comment(lib, "Json/jsoncppd.lib")
#else
#pragma comment(lib, "Json/jsoncpp.lib")
#endif

/* PhysX */
#include "PhysX/PxPhysics.h"
#include "PhysX/PxPhysicsAPI.h"
#include "PhysX/cooking/PxCooking.h"
#include "PhysX/characterkinematic/PxControllerManager.h"

#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES	128

#ifdef _DEBUG
#pragma comment(lib, "PhysX/Debug/PhysX_64.lib")
#pragma comment(lib, "PhysX/Debug/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX/Debug/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX/Debug/PhysXCooking_64.lib")
#pragma comment(lib, "PhysX/Debug/PhysXCommon_64.lib")
#pragma comment(lib, "PhysX/Debug/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysX/Debug/PhysXCharacterKinematic_static_64.lib")
#else
#pragma comment(lib, "PhysX/Release/PhysX_64.lib")
#pragma comment(lib, "PhysX/Release/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX/Release/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX/Release/PhysXCooking_64.lib")
#pragma comment(lib, "PhysX/Release/PhysXCommon_64.lib")
#pragma comment(lib, "PhysX/Release/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysX/Release/PhysXCharacterKinematic_static_64.lib")
#endif

using namespace physx;

#include <wincodec.h>

/* File IO */
#include <fstream>
#include <sstream>

/* Headers */
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

/* Namespace */
using namespace std;
using namespace Engine;

/* Leak Debug */
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif
#endif
