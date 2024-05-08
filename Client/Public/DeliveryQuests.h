#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Quest.h"

BEGIN(Client)

class CDelivery_Quest final : public CQuest
{
public:
	typedef struct tagInputQuestDesc : public tagQuestDesc
	{
		wstring strItemName;
		
	}DELIVERY_QUEST_DESC;
private:
	CDelivery_Quest();
	CDelivery_Quest(const CDelivery_Quest& rhs);
	virtual ~CDelivery_Quest() = default;

private:
	wstring	m_StrItemName;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(_float fTimeDelta)override;

public:
	static	CQuest* Create(void* pArg);
	virtual void	Free() override;

};

END