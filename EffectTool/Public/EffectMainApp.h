#pragma once
#include "EffectTool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(EffectTool)

class CEffectMainApp final : public CBase
{
private:
	CEffectMainApp();
	virtual ~CEffectMainApp() = default;

private:
	HRESULT Open_Level(LEVELID eLevel);

private:
	HRESULT Ready_Prototype();
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Prototype_For_Loading();

public:
	HRESULT Initialize();
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	class CToolWindow* m_pTool = { nullptr };

private:
	_int m_iFPS = { 0 };
	_float m_fTimeAcc = { 0.f };

public:
	static CEffectMainApp* Create();
	virtual void	 Free() override;
};

END