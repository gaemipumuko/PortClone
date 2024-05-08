#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GearOperator.h"
#include "DungeonPlatform.h"

BEGIN(Client)

class CGearOperator_Manager final : public CGameObject
{
public:
	typedef struct tagGearOperatorMgrDesc
	{
		LEVELID	eGearLevel = { LEVEL_END };
	} GEARMGR_DESC;

	typedef struct tagGearOperatorContainer
	{
		CGearOperator*	pGearOperator = { nullptr };
	} GEAROPER_CONTAINER;

private:
	CGearOperator_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGearOperator_Manager(const CGearOperator_Manager& rhs);
	virtual ~CGearOperator_Manager() = default;

private:
	LEVELID	m_eLevelID = { LEVEL_END };

private:
	CGearOperator*				m_pGearPrototype = { nullptr };
	CDungeonPlatform*			m_pDungeonPlatformPrototype = { nullptr };

	list<GEAROPER_CONTAINER>	m_GearOperators;

private:
	HRESULT	Ready_Dungeon_Temple();
	HRESULT	Ready_Dungeon_StrongHold();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CGearOperator_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END