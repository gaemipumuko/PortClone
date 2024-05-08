#pragma once

#include "Base.h"

BEGIN(Engine)

class CScreen_Manager final : public CBase
{
private:
	CScreen_Manager();
	virtual ~CScreen_Manager() = default;

private:
	HWND	m_hWnd = { };

private:
	_int	m_iFixCount = { };
	_int	m_iClipCount = { };

public:
	_int	Get_ClipCount() { return m_iClipCount; }

public:
	void	Fix_Mouse() { m_iFixCount = 1; }
	void	FreeFix_Mouse() { m_iFixCount = 0; }

	void	Clip_Mouse() { m_iClipCount = 1; }
	void	FreeClip_Mouse() { m_iClipCount = 0; }

public:
	HRESULT Initialize(HWND hWnd);
	HRESULT PreviousTick(_float fTImeDelta);

public:
	static CScreen_Manager* Create(HWND hWnd);
	virtual void			Free();
};

END