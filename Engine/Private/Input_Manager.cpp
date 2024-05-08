#include "Input_Manager.h"

CInput_Manager::CInput_Manager()
	: m_bKeyState { FALSE }
{
}

_bool CInput_Manager::Key_Pressing(_int iKey)
{
	if (GetAsyncKeyState(iKey) & 0x8000)
		return TRUE;

	return FALSE;
}

_bool CInput_Manager::Key_Down(_int iKey)
{
	if (!m_bKeyState[iKey] && (GetAsyncKeyState(iKey) & 0x8000))
	{
		m_mapReadyState.emplace(iKey, !m_bKeyState[iKey]);
		return TRUE;
	}

	if (m_bKeyState[iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
		m_mapReadyState.emplace(iKey, !m_bKeyState[iKey]);

	return FALSE;
}

_bool CInput_Manager::Key_Up(_int iKey)
{
	if (m_bKeyState[iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
	{
		m_mapReadyState.emplace(iKey, !m_bKeyState[iKey]);
		return TRUE;
	}

	if (!m_bKeyState[iKey] && (GetAsyncKeyState(iKey) & 0x8000))
		m_mapReadyState.emplace(iKey, !m_bKeyState[iKey]);

	return FALSE;
}

_long CInput_Manager::Mouse_Move(MOUSEMOVESTATE eMouseMoveID)
{
	return *(((_long*)&m_tMouseState) + eMouseMoveID);
}

HRESULT CInput_Manager::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	/* DINPUT 컴객체 생성 */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	/* 마우스 객체 생성 */
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	/* 생성된 마우스 객체의 정보를 컴객체에게 전달 */
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	/* 장치에 대한 독점권 설정 (클라이언트 백그라운드 시 입력) */
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	/* 장치 Access 버전을 받음 */
	m_pMouse->Acquire();

	return S_OK;
}

HRESULT CInput_Manager::PreviousTick(_float fTImeDelta)
{
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	return S_OK;
}

HRESULT CInput_Manager::LateTick(_float fTImeDelta)
{
	for (auto& iter : m_mapReadyState)
		m_bKeyState[iter.first] = iter.second;

	m_mapReadyState.clear();

	return S_OK;
}

CInput_Manager* CInput_Manager::Create(HINSTANCE hInstance, HWND hWnd)
{
	CInput_Manager* pInstance = new CInput_Manager();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Create : CInput_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
