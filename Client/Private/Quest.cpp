#include "stdafx.h"
#include "Quest.h"

#include "Quest_Manager.h"
#include "GameInstance.h"

CQuest::CQuest()
{
}

CQuest::CQuest(const CQuest& rhs)
{
}

HRESULT CQuest::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		QUEST_DESC* tQuest_Desc = static_cast<QUEST_DESC*>(pArg);


		m_strQuestName = tQuest_Desc->strQuestName;
		m_strQuestDetail = tQuest_Desc->strQuestDetail;
		m_strObjectiveTexts = tQuest_Desc->strObjectiveTexts;
		m_eQuest_Type = tQuest_Desc->eQuest_Type;
		m_eQuest_State = tQuest_Desc->eQuest_State;

	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CQuest_Manager* Quest_Manager = (CQuest_Manager*)pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_QuestManager"), 0);
	Quest_Manager->Add_Quest(this);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CQuest::Free()
{
}
