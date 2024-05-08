#include "Screen_Manager.h"

CScreen_Manager::CScreen_Manager()
{
}

HRESULT CScreen_Manager::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

    return S_OK;
}

HRESULT CScreen_Manager::PreviousTick(_float fTImeDelta)
{
	if (m_iFixCount > 0)
	{
		HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO tMonitorInfo;

		tMonitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &tMonitorInfo);

		_int iCenterX = (tMonitorInfo.rcMonitor.left + tMonitorInfo.rcMonitor.right) / 2;
		_int iCenterY = (tMonitorInfo.rcMonitor.top + tMonitorInfo.rcMonitor.bottom) / 2;

		SetCursorPos(iCenterX, iCenterY);
	}

	if (m_iClipCount > 0)
	{
		RECT	rcRect = { };
		POINT	ptLeftTop = { }, ptRightBottom = { };

		GetClientRect(m_hWnd, &rcRect);

		ptLeftTop.x = rcRect.left;		ptLeftTop.y = rcRect.top;
		ptRightBottom.x = rcRect.right;	ptRightBottom.y = rcRect.bottom;
		
		ClientToScreen(m_hWnd, &ptLeftTop);
		ClientToScreen(m_hWnd, &ptRightBottom);

		rcRect.left = ptLeftTop.x;		rcRect.top = ptLeftTop.y;
		rcRect.right = ptRightBottom.x;	rcRect.bottom = ptRightBottom.y;

		ClipCursor(&rcRect);
	}
	else
	{
		ClipCursor(NULL);
	}

	return S_OK;
}

CScreen_Manager* CScreen_Manager::Create(HWND hWnd)
{
	CScreen_Manager* pInstance = new CScreen_Manager();

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Create : CScreen_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScreen_Manager::Free()
{
    __super::Free();
}
