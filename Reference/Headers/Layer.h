#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

private:
	list<class CGameObject*>	m_GameObjects;

public:
	HRESULT				Add_GameObject(class CGameObject* pGameObject);
	class CGameObject*	Find_GameObject(_uint iIndex = 0);
	HRESULT				Release_GameObject(class CGameObject* pGameObject);

	class CComponent*	Find_Layer_Component(const wstring& strComponentTag, _uint iIndex);

	list<class CGameObject*>* Get_GameObjects() { return &m_GameObjects; }

public:
	HRESULT Initialize();
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);

public:
	static CLayer*	Create();
	virtual void	Free() override;
};

END