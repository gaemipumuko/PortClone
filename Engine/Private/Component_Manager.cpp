#include "Component_Manager.h"

#include "Component.h"
#include "GameInstance.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Add_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, CComponent* pPrototype, _bool bDuplication)
{
	if (nullptr != Find_Component_Prototype(iLevel, strPrototypeTag))
	{
		if (bDuplication == TRUE)
		{
			Safe_Release(pPrototype);
			return S_OK;
		}

		return E_FAIL;
	}

	m_Prototypes[iLevel].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CComponent_Manager::Release_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag)
{
	auto iter = Find_Component_Prototype(iLevel, strPrototypeTag);

	if (iter == nullptr)
		return E_FAIL;

	Safe_Release(iter);
	m_Prototypes[iLevel].erase(strPrototypeTag);

	return S_OK;
}

CComponent* CComponent_Manager::Find_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag)
{
 	auto& iter = m_Prototypes[iLevel].find(strPrototypeTag);

	if (iter == m_Prototypes[iLevel].end())
		return nullptr;

	return iter->second;
}

CComponent* CComponent_Manager::Clone_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Component_Prototype(iLevel, strPrototypeTag);

	if (pPrototype == nullptr)
		return nullptr;

	return pPrototype->Clone(pArg);
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevel)
{
	if (m_Prototypes != nullptr)
		return E_FAIL;

	m_Prototypes = new PROTOTYPES[iNumLevel];
	m_iNumLevel = iNumLevel;

	return S_OK;
}

CComponent_Manager* CComponent_Manager::Create(_uint iNumLevel)
{
	CComponent_Manager* pInstance = new CComponent_Manager();

	if (FAILED(pInstance->Initialize(iNumLevel)))
	{
		MSG_BOX(TEXT("Failed to Create : CComponent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComponent_Manager::Clear(_uint iLevel)
{
	if (m_Prototypes == nullptr)
		return;

	if (iLevel >= m_iNumLevel)
		return;

	for (auto& Pair : m_Prototypes[iLevel])
	{
		Safe_Release(Pair.second);
	}

	m_Prototypes[iLevel].clear();
}

void CComponent_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevel; i++)
	{
		for (auto& Pair : m_Prototypes[i])
		{
			Safe_Release(Pair.second);
		}

		m_Prototypes[i].clear();
	}

	Safe_Delete_Array(m_Prototypes);
}
