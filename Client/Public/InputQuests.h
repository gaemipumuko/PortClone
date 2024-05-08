#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Quest.h"

BEGIN(Client)

class CInput_Quest final : public CQuest
{
public:
	typedef struct tagInputQuestDesc : public tagQuestDesc
	{
		_int iKey;
		
	}IN_QUEST_DESC;
private:
	CInput_Quest();
	CInput_Quest(const CInput_Quest& rhs);
	virtual ~CInput_Quest() = default;

private:
	_int	m_iKey;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(_float fTimeDelta)override;

public:
	static	CQuest* Create(void* pArg);
	virtual void	Free() override;

};

END