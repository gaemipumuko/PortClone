#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Quest.h"

BEGIN(Client)

class CQuest_Manager : public CGameObject
{
private:
	CQuest_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Manager(const CQuest_Manager& rhs);
	virtual ~CQuest_Manager() = default;

private:
	vector<CQuest*> m_vecQuests;
	vector<CQuest*>	m_vecCompletedQuests;
//should have been maps
	class CUI_Manager* m_pUI_Manager = { nullptr };
public:
    void Add_Quest(CQuest* pQuest);
	CQuest* Check_Duplicates(wstring strQuestName);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT PreviousTick(_float fTimeDelta);
	virtual HRESULT Tick(_float fTimeDelta);
	virtual HRESULT LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	static	CQuest_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Manager* Clone(void* pArg);
	virtual void	Free() override;

};

END