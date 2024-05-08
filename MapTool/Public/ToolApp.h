#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Tool)

class CToolApp final : public CBase
{
private:
	CToolApp();
	virtual ~CToolApp() = default;

private:
	HRESULT Open_Level(LEVELID eLevel);

private:
	HRESULT Ready_Prototype();
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Prototype_For_Loading();
	HRESULT Ready_Fonts();

public:
	HRESULT Initialize();
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);
	HRESULT Render();

private:
	CRenderer* m_pRenderer = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	class CTool_Manager* m_pTool_Manager = { nullptr };

private:
	_int m_iFPS = { 0 };
	_float m_fTimeAcc = { 0.f };

public:
	static CToolApp* Create();
	virtual void	 Free() override;
};

END
