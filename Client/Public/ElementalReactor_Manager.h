#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "DungeonDoor.h"
#include "DungeonPlatform.h"
#include "ElementalReactor.h"

BEGIN(Client)

class CElementalReactor_Manager final : public CGameObject
{
public:
	typedef struct tagReactorMgrDesc
	{
		LEVELID	eReactorLevel = { LEVEL_END };
	} REACTORMGR_DESC;

	typedef struct tagReactorContainer
	{
		enum RCTYPE { RC_DOOR = 0x0001, RC_PLATFORM = 0x0010, RC_END = 0x0000 };
		_uint					iType = { RC_END };
		CElementalReactor*		pReactor = { nullptr };

		CDungeonDoor*			pDungeonDoor = { nullptr };
		list<CDungeonPlatform*>	pDungeonPlatform;
	} REACTOR_CONTAINER;

private:
	CElementalReactor_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElementalReactor_Manager(const CElementalReactor_Manager& rhs);
	virtual ~CElementalReactor_Manager() = default;

private:
	LEVELID						m_eLevelID = { LEVEL_END };

private:
	CElementalReactor*			m_pReactorPrototype = { nullptr };
	CDungeonDoor*				m_pDungeonDoorPrototype = { nullptr };
	CDungeonPlatform*			m_pDungeonPlatformPrototype = { nullptr };

	list<REACTOR_CONTAINER>		m_ElementalReactors;

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
	static CElementalReactor_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg);
	virtual void						Free() override;
};

END