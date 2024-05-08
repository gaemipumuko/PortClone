#pragma once

#include "Animation_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(AnimationTool)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };	
	ID3D11DeviceContext*	m_pContext = { nullptr };	

private:
	CRenderer*				m_pRenderer = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	HRESULT Open_Level(LEVELID eLevel);

private:
	HRESULT Ready_Prototype();

	HRESULT Ready_Prototype_GameObject();				/* GameObject */
	HRESULT Ready_Prototype_Component_For_Static();		/* Component */

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