#include "..\Public\Timer_Manager.h"

#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTimer_Manager::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	m_Timers.emplace(strTimerTag, CTimer::Create());

	return S_OK;
}

CTimer * CTimer_Manager::Find_Timer(const wstring & strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

_float CTimer_Manager::Compute_TimeDelta(const wstring & strTimerTag)
{
	CTimer*	pTimer = Find_Timer(strTimerTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Compute_TimeDelta();
}

CTimer_Manager * CTimer_Manager::Create()
{
	CTimer_Manager* pInstance = new CTimer_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CTimer_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}
