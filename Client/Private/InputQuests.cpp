#include "stdafx.h"
#include "InputQuests.h"
#include "GameInstance.h"


CInput_Quest::CInput_Quest()
{
}

CInput_Quest::CInput_Quest(const CInput_Quest& rhs)
{
}

HRESULT CInput_Quest::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		IN_QUEST_DESC* InputQuestDesc = (IN_QUEST_DESC*)pArg;
		m_iKey = InputQuestDesc->iKey;

	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}


HRESULT CInput_Quest::Update(_float fTimeDelta)
{
	CGameInstance* pGameInstace = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstace);

	if (pGameInstace->Key_Down(m_iKey))
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


CQuest* CInput_Quest::Create(void* pArg)
{
	CInput_Quest* pQuest = new CInput_Quest();

	if (FAILED(pQuest->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialized : CInput_Quest"));
		Safe_Release(pQuest);
	}

	return pQuest;
}

void CInput_Quest::Free()
{
	__super::Free();
}
