#include "StatusEffect_Manager.h"

CStatusEffect_Manager::CStatusEffect_Manager()
{
}

HRESULT CStatusEffect_Manager::Add_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount, _float fDuration, _bool bCompareMax)
{
	auto& iter = m_StatusEffects[iLayer].find(strObjectID);
	if (iter != m_StatusEffects[iLayer].end())
	{
		iter->second->Set_Count_Compare(iCount, bCompareMax);
		if(fDuration != 0.f)
			iter->second->Set_Duration(fDuration);

		return S_OK;
	}

	CStatusEffect::STATUSEFFECT_DESC tDesc = {};
	tDesc.fDuration = fDuration;
	tDesc.iCount = iCount;
	CStatusEffect* pInstance = CStatusEffect::Create(&tDesc);
	if (pInstance == nullptr)
		return E_FAIL;

	m_StatusEffects[iLayer].emplace(strObjectID, pInstance);

	return S_OK;
}

HRESULT CStatusEffect_Manager::Release_StatusEffect(_uint iLayer, wstring strObjectID)
{
	auto& iter = m_StatusEffects[iLayer].find(strObjectID);
	if (iter == m_StatusEffects[iLayer].end())
		return E_FAIL;

	Safe_Release(iter->second);
	m_StatusEffects[iLayer].erase(strObjectID);

	return S_OK;
}

_int CStatusEffect_Manager::Get_Count_StatusEffect(_uint iLayer, wstring strObjectID)
{
	auto& iter = m_StatusEffects[iLayer].find(strObjectID);
	if (iter == m_StatusEffects[iLayer].end())
		return -1;

	return iter->second->Get_Count();
}

_bool CStatusEffect_Manager::IsProgress_StatusEffect(_uint iLayer, wstring strObjectID)
{
	auto& iter = m_StatusEffects[iLayer].find(strObjectID);
	if (iter == m_StatusEffects[iLayer].end())
		return FALSE;

	return TRUE;
}

HRESULT CStatusEffect_Manager::Add_Count_StatusEffect(_uint iLayer, wstring strObjectID, _int iCount)
{
	auto& iter = m_StatusEffects[iLayer].find(strObjectID);
	if (iter == m_StatusEffects[iLayer].end())
		return E_FAIL;

	iter->second->Add_Count(iCount);

	return S_OK;
}

HRESULT CStatusEffect_Manager::Initialize(_uint iLayerNum)
{
	m_StatusEffects = new map<wstring, CStatusEffect*>[iLayerNum];
	m_iLayerNum = iLayerNum;

	return S_OK;
}

HRESULT CStatusEffect_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iLayerNum; ++i)
	{
		for (auto iter = m_StatusEffects[i].begin(); iter != m_StatusEffects[i].end();)
		{
			auto& Pair = *iter;
			Pair.second->Update(fTimeDelta);
			if (Pair.second->Get_Destroy() == TRUE)
			{
				Safe_Release(Pair.second);
				iter = m_StatusEffects[i].erase(iter);
			}
			else
				++iter;

		}
	}

	return S_OK;
}

CStatusEffect_Manager* CStatusEffect_Manager::Create(_uint iNumLayer)
{
	CStatusEffect_Manager* pInstance = new CStatusEffect_Manager;

	if (FAILED(pInstance->Initialize(iNumLayer)))
	{
		MSG_BOX(TEXT("Failed to Create : CStatusEffect_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatusEffect_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iLayerNum; ++i)
	{
		for (auto& Pair : m_StatusEffects[i])
		{
			Safe_Release(Pair.second);
		}
		m_StatusEffects[i].clear();
	}
	Safe_Delete_Array(m_StatusEffects);
}
