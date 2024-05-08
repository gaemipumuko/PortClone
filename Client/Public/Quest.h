#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CQuest abstract : public CBase
{
public:
	enum QUEST_TYPES { QUEST_NORMAL, QUEST_LEGACY, QUEST_LEGENDARY, QUEST_END};
	enum QUEST_STATE { QUEST_NOT_ACCEPTED, QUEST_ACCEPTED, QUEST_IN_PROGRESS, QUEST_CAN_COMPLETE, QUEST_COMPLETED, QUEST_STATE_END};

	typedef struct tagQuestDesc
	{
		wstring			strQuestName;
		wstring			strQuestDetail;
		wstring			strObjectiveTexts;
		QUEST_TYPES		eQuest_Type;
		QUEST_STATE		eQuest_State;
		
	}QUEST_DESC;

protected:
	CQuest();
	CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

protected:
	_bool					m_bIsUpdated = {false};

	wstring					m_strQuestName;
	wstring					m_strQuestDetail;
	wstring					m_strObjectiveTexts;

	QUEST_TYPES				m_eQuest_Type = { QUEST_END };
	QUEST_STATE				m_eQuest_State = { QUEST_STATE_END };

public:
	QUEST_STATE	Get_QuestState() { return m_eQuest_State; }
	QUEST_TYPES Get_QuestType() { return m_eQuest_Type; }
	wstring		Get_QuestName() { return m_strQuestName; }
	wstring		Get_QuestDetail() { return m_strQuestDetail; }
	_bool		Get_QuestUpdated() { return m_bIsUpdated; }
	void		Set_QuestUpdated(_bool bIsUpdated) { m_bIsUpdated = bIsUpdated; }
	void		Set_QuestState(QUEST_STATE eState) { m_eQuest_State = eState; }
public:
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Update(_float fTimeDelta) PURE;


public:
	virtual void	Free() override;

};

END