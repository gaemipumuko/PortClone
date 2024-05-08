#include "Object_Manager.h"

#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Add_Object_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype, _bool bDuplication)
{
	if (Find_Object_Prototype(strPrototypeTag) != nullptr)
	{
		if (bDuplication == TRUE)
		{
			Safe_Release(pPrototype);
			return S_OK;
		}

		return E_FAIL;
	}

	m_Prototype.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_Object_Layer(_uint iLevel, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg, class CGameObject** ppObject)
{
	CGameObject* pPrototype = Find_Object_Prototype(strPrototypeTag);
	if (pPrototype == nullptr)
		return E_FAIL;

	CGameObject* pClone = pPrototype->Clone(pArg);
	if (pClone == nullptr)
		return E_FAIL;

	CLayer* pLayer = Find_Object_Layer(iLevel, strLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		if (pLayer == nullptr)
			return E_FAIL;

		m_pLayers[iLevel].emplace(strLayerTag, pLayer);
	}

	if (FAILED(pLayer->Add_GameObject(pClone)))
		return E_FAIL;

	if (ppObject != nullptr)
		*ppObject = pClone;

	return S_OK;
}

CGameObject* CObject_Manager::Find_Object_Prototype(const wstring& strPrototypeTag)
{
	auto& iter = m_Prototype.find(strPrototypeTag);

	if (iter == m_Prototype.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Object_Layer(_uint iLevel, const wstring& strLayerTag)
{
	auto& iter = m_pLayers[iLevel].find(strLayerTag);

	if (iter == m_pLayers[iLevel].end())
		return nullptr;

	return iter->second;
}

CGameObject* CObject_Manager::Find_Object(_uint iLevel, const wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Object_Layer(iLevel, strLayerTag);
	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Find_GameObject(iIndex);
}

CComponent* CObject_Manager::Find_Layer_Component(_uint iLevel, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Object_Layer(iLevel, strLayerTag);
	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Find_Layer_Component(strComponentTag, iIndex);
}

HRESULT CObject_Manager::Release_Object(_uint iLevel, const wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Object_Layer(iLevel, strLayerTag);
	if (pLayer == nullptr)
		return E_FAIL;

	if (FAILED(pLayer->Release_GameObject(pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Manager::Release_Object_Layer(_uint iLevel, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Object_Layer(iLevel, strLayerTag);
	if (pLayer == nullptr)
		return E_FAIL;

	m_pLayers[iLevel].erase(strLayerTag);
	Safe_Release(pLayer);

	return S_OK;
}

list<class CGameObject*>* CObject_Manager::Find_Objects(_uint iLevel, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Object_Layer(iLevel, strLayerTag);

	if (pLayer == nullptr)
		return nullptr;

	return pLayer->Get_GameObjects();
}

HRESULT CObject_Manager::Initialize(_uint iNumLevel)
{
	if (m_pLayers != nullptr)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevel];

	m_iNumLevel = iNumLevel;

	return S_OK;
}

HRESULT CObject_Manager::PreviousTick(_float fTimeDelta)
{
	for (size_t idx = 0; idx < m_iNumLevel; ++idx)
	{
		for (auto& Pair : m_pLayers[idx])
		{
			if (FAILED(Pair.second->PreviousTick(fTimeDelta)))
			{
				MSG_BOX(L"Failed to PreviousTick : CObject_Manager");
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CObject_Manager::Tick(_float fTimeDelta)
{
	for (size_t idx = 0; idx < m_iNumLevel; ++idx)
	{
		for (auto& Pair : m_pLayers[idx])
		{
			if (FAILED(Pair.second->Tick(fTimeDelta)))
			{
				MSG_BOX(L"Failed to Tick : CObject_Manager");
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CObject_Manager::LateTick(_float fTimeDelta)
{
	for (size_t idx = 0; idx < m_iNumLevel; ++idx)
	{
		for (auto& Pair : m_pLayers[idx])
		{
			if (FAILED(Pair.second->LateTick(fTimeDelta)))
			{
				MSG_BOX(L"Failed to LateTick : CObject_Manager");
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevel)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevel)))
	{
		MSG_BOX(TEXT("Failed to Create : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Clear(_uint iLevel)
{
	if (m_pLayers == nullptr)
		return;

	if (iLevel >= m_iNumLevel)
		return;

	for (auto& Pair : m_pLayers[iLevel])
	{
		if (Safe_Release(Pair.second) > 0)
			MSG_BOX(Pair.first.c_str());
	}
	m_pLayers[iLevel].clear();
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevel; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (Safe_Release(Pair.second) > 0)
				MSG_BOX(Pair.first.c_str());
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototype)
	{
		if (Safe_Release(Pair.second) > 0)
			MSG_BOX(Pair.first.c_str());
	}
	m_Prototype.clear();
}
