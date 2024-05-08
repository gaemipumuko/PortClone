#pragma once

#include "Base.h"

#define VK_MAX	0xff

BEGIN(Engine)

class CInput_Manager final : public CBase
{
private:
	CInput_Manager();
	virtual ~CInput_Manager() = default;

private:
	LPDIRECTINPUT8				m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8		m_pMouse = { nullptr };

private:
	_bool						m_bKeyState[VK_MAX];
	map<_int, _bool>			m_mapReadyState;

	/* 이전 프레임 기준으로 이동 거리(픽셀) (lX, lY : Mouse) (lZ : Wheel) */
	DIMOUSESTATE				m_tMouseState = { };

public:	  
	_bool Key_Pressing(_int iKey);
	_bool Key_Down(_int iKey);
	_bool Key_Up(_int iKey);

	_long Mouse_Move(MOUSEMOVESTATE eMouseMoveID);

public:
	HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
	HRESULT PreviousTick(_float fTImeDelta);
	HRESULT LateTick(_float fTImeDelta);

public:
	static CInput_Manager*	Create(HINSTANCE hInstance, HWND hWnd);
	virtual void			Free();
};

END