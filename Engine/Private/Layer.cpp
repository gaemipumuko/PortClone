#include "..\Public\Layer.h"

#include "../Public/GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

CGameObject* CLayer::Find_GameObject(_uint iIndex)
{
	if (m_GameObjects.size() - 1 < iIndex)
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter);
}

HRESULT CLayer::Release_GameObject(CGameObject* pGameObject)
{
	for (auto& iter : m_GameObjects)
	{
		if (iter == pGameObject)
		{
			Safe_Release(iter);
			m_GameObjects.remove(iter);

			return S_OK;
		}
	}

	return E_FAIL;
}

CComponent* CLayer::Find_Layer_Component(const wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t idx = 0; idx < iIndex; idx++)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Find_Component(strComponentTag);
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::PreviousTick(_float fTimeDelta)
{
	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if ((*iter) == nullptr)
		{
			MSG_BOX(TEXT("Failed to PreviousTick : CLayer"));
			return E_FAIL;
		}

		if (FAILED((*iter)->PreviousTick(fTimeDelta)))
		{
			MSG_BOX(TEXT("Failed to PreviousTick : CLayer"));
			return E_FAIL;
		}

		++iter;
	}

	return S_OK;
}

HRESULT CLayer::Tick(_float fTimeDelta)
{
	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if ((*iter) == nullptr)
		{
			MSG_BOX(TEXT("Failed to Tick : CLayer"));
			return E_FAIL;
		}

		if (FAILED((*iter)->Tick(fTimeDelta)))
		{
			MSG_BOX(TEXT("Failed to Tick : CLayer"));
			return E_FAIL;
		}

		++iter;
	}

	return S_OK;
}

HRESULT CLayer::LateTick(_float fTimeDelta)
{
	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if ((*iter) == nullptr)
		{
			MSG_BOX(TEXT("Failed to LateTick : CLayer"));
			return E_FAIL;
		}

		if ((*iter)->Get_Destroy() == TRUE)
		{
			Safe_Release(*iter);
			iter = m_GameObjects.erase(iter);
		}
		else
		{
			if (FAILED((*iter)->LateTick(fTimeDelta)))
			{
				MSG_BOX(TEXT("Failed to LateTick : CLayer"));
				return E_FAIL;
			}

			++iter;
		}
	}

	return S_OK;
}

CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer();

	if (FAILED(pLayer->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Initialized : CLayer"));
		Safe_Release(pLayer);
	}

	return pLayer;
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
