#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Quest.h"

BEGIN(Client)

class CKill_Quest final : public CQuest
{
public:
	typedef struct tagKillQuestDesc : public tagQuestDesc
	{
		wstring CreatureID = {};
		_int iKillQty;
		
	}KILL_QUEST_DESC;
private:
	CKill_Quest();
	CKill_Quest(const CKill_Quest& rhs);
	virtual ~CKill_Quest() = default;

private:
	_int	m_iKillQty;
	_int	m_iCurrentKills = {0};
	_int	m_iTargetID = {};

	wstring m_strMonsterID = {};

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(_float fTimeDelta)override;

public:
	static	CQuest* Create(void* pArg);
	virtual void	Free() override;

};

END