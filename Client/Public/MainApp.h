#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
#ifndef _DEBUGNUI
class CDebugTool;
#endif
class CElemental_Manager;
END

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };	/* 공간 할당 (Create Buffer) */
	ID3D11DeviceContext*	m_pContext = { nullptr };	/* 기능 (Draw, Bind, Etc...) */

private:
	CRenderer*				m_pRenderer = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
#ifndef _DEBUGNUI
	CDebugTool*				m_pDebugTool = { nullptr };
#endif

private:
	CElemental_Manager*		m_pElemental_Manager = { nullptr };

protected:
	_int					m_iFPS = { 0 };
	_float					m_fTimeAcc = { 0.f };

private:
	HRESULT Open_Level(LEVELID eLevel);

private:
	HRESULT Ready_Prototype();
	HRESULT Ready_Json();

	HRESULT Ready_Prototype_GameObject();				/* GameObject */

	HRESULT Ready_Prototype_Player();
	HRESULT Ready_Prototype_Camera();
	HRESULT Ready_Prototype_Monster();
	HRESULT Ready_Prototype_Effect();
	HRESULT Ready_Prototype_Skill();
	HRESULT Ready_Prototype_NPC();
	HRESULT Ready_Prototype_UI();
	HRESULT Ready_Prototype_Interaction();
	HRESULT Ready_Prototype_Map();
	HRESULT Ready_Prototype_PartObject_Body();
	HRESULT Ready_Prototype_PartObject_Weapon();
	HRESULT Ready_Prototype_PartObject_Glider();
	HRESULT Ready_Prototype_PartObject_Status();

	HRESULT Ready_Prototype_Component_For_Static();		/* Component */

	HRESULT Ready_ProtoType_Component_Basic();
	HRESULT Ready_ProtoType_Component_Shader();
	HRESULT Ready_ProtoType_Component_VIBuffer();
	HRESULT Ready_ProtoType_Component_Texture_UI();
	HRESULT Ready_ProtoType_Component_Texture_Effect();
	HRESULT Ready_ProtoType_Component_Texture_Map();
	HRESULT Ready_ProtoType_Component_Texture_Asset();
	HRESULT Ready_ProtoType_Component_Model_Character();
	HRESULT Ready_ProtoType_Component_Model_Weapon();
	HRESULT Ready_ProtoType_Component_Model_Glider();
	HRESULT Ready_ProtoType_Component_Model_Monster();
	HRESULT Ready_ProtoType_Component_Model_NPC();
	HRESULT Ready_ProtoType_Component_Model_Skill();
	HRESULT Ready_ProtoType_Component_Model_Effect();
	HRESULT Ready_ProtoType_Component_Model_Interaction();

	HRESULT Ready_Fonts();

	HRESULT Loading_WorldModel(LEVELID eLevel, wstring strDataPath);

public:
	HRESULT Initialize();
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);
	HRESULT Render();

public:
	static CMainApp*	Create();
	virtual void		Free() override;
};

END