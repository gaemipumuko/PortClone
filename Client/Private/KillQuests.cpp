#include "stdafx.h"
#include "KillQuests.h"
#include "GameInstance.h"
#include "Quest_Manager.h"


CKill_Quest::CKill_Quest()
{
}

CKill_Quest::CKill_Quest(const CKill_Quest& rhs)
{
}

HRESULT CKill_Quest::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		CGameInstance* pGameInstace = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstace);

		KILL_QUEST_DESC* KillQuestDesc = (KILL_QUEST_DESC*)pArg;
		m_iKillQty = KillQuestDesc->iKillQty;
		m_strMonsterID = KillQuestDesc->CreatureID;

		pGameInstace->Register_Interaction(m_strMonsterID);
		Safe_Release(pGameInstace);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}


HRESULT CKill_Quest::Update(_float fTimeDelta)
{
	CGameInstance* pGameInstace = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstace);

	if (pGameInstace->Get_InteractionCount(m_strMonsterID) >= m_iKillQty && m_eQuest_State == QUEST_IN_PROGRESS)
	{

		m_bIsUpdated = true;
		m_eQuest_State = { QUEST_CAN_COMPLETE };
	}

	if (pGameInstace->Key_Down('5') && m_eQuest_State == QUEST_IN_PROGRESS)
	{
		m_bIsUpdated = true;
		m_eQuest_State = { QUEST_CAN_COMPLETE };
	}

	if (m_eQuest_State == QUEST_CAN_COMPLETE)
	{

		m_strQuestDetail = TEXT("캐서린에게 보상을 수령하러 가자.");
	}

	Safe_Release(pGameInstace);

	return S_OK;
}


CQuest* CKill_Quest::Create(void* pArg)
{

	QUEST_DESC* tQuest_Desc = static_cast<QUEST_DESC*>(pArg);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CQuest_Manager* Quest_Manager = (CQuest_Manager*)pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_QuestManager"), 0);
	Safe_Release(pGameInstance);

	CKill_Quest* pQuest = (CKill_Quest*)Quest_Manager->Check_Duplicates(tQuest_Desc->strQuestName);
	if(pQuest !=nullptr)
		return pQuest;

	CKill_Quest* pNewQuest = new CKill_Quest();

	if (FAILED(pNewQuest->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialized : CKill_Quest"));
		Safe_Release(pNewQuest);
	}

	return pNewQuest;
}

void CKill_Quest::Free()
{
	__super::Free();
}
