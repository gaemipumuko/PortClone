#include "stdafx.h"
#include "DeliveryQuests.h"
#include "GameInstance.h"
#include "Inventory.h"


CDelivery_Quest::CDelivery_Quest()
{
}

CDelivery_Quest::CDelivery_Quest(const CDelivery_Quest& rhs)
{
}

HRESULT CDelivery_Quest::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		DELIVERY_QUEST_DESC* DeliveryQuestDesc = (DELIVERY_QUEST_DESC*)pArg;
		m_StrItemName = DeliveryQuestDesc->strItemName;

	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}


HRESULT CDelivery_Quest::Update(_float fTimeDelta)
{
	CGameInstance* pGameInstace = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstace);

	CInventory* pInven = (CInventory*)pGameInstace->Find_Object(LEVEL_WORLD, TEXT("Layer_Inventory"));
	if (pInven->Search_Inventory(m_StrItemName)>0)
	{
		m_eQuest_State = { QUEST_CAN_COMPLETE };
		//if(m_eQuest_State ==QUEST_IN_PROGRESS)
		m_bIsUpdated = true;
	}

	if (m_eQuest_State == QUEST_CAN_COMPLETE)
	{

		m_strQuestDetail = TEXT("Äù½ºÆ® ¿Ï·á.");
	}

	Safe_Release(pGameInstace);

	return S_OK;
}


CQuest* CDelivery_Quest::Create(void* pArg)
{
	CDelivery_Quest* pQuest = new CDelivery_Quest();

	if (FAILED(pQuest->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialized : CDelivery_Quest"));
		Safe_Release(pQuest);
	}

	return pQuest;
}

void CDelivery_Quest::Free()
{
	__super::Free();
}
