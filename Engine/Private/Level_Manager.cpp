#include "Level_Manager.h"

#include "Level.h"

#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevel, CLevel* pLevel)
{
	if (m_pCurrentLevel != nullptr)
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pInstance);

		pInstance->Clear(m_iCurrentIndex);
		Safe_Release(m_pCurrentLevel);

		Safe_Release(pInstance);
	}

	m_pCurrentLevel = pLevel;
	m_iCurrentIndex = iLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Initialize(_uint iNumLevel)
{
	m_iNumLevel = iNumLevel;

	return S_OK;
}

HRESULT CLevel_Manager::PreviousTick(_float fTimeDelta)
{
	if (m_pCurrentLevel == nullptr)
		return E_FAIL;

	return m_pCurrentLevel->PreviousTick(fTimeDelta);
}

HRESULT CLevel_Manager::Tick(_float fTimeDelta)
{
	if (m_pCurrentLevel == nullptr)
		return E_FAIL;

	return m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT CLevel_Manager::LateTick(_float fTimeDelta)
{
	if (m_pCurrentLevel == nullptr)
		return E_FAIL;

	return m_pCurrentLevel->LateTick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (m_pCurrentLevel == nullptr)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

CLevel_Manager* CLevel_Manager::Create(_uint iNumLevel)
{
	CLevel_Manager* pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Initialize(iNumLevel)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentLevel);
}
