#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CDeciderObject abstract : public CGameObject
{
public:
	enum RENDER_MODE { RMODE_GAMEPLAY, RMODE_UI, RMODE_END };	/* 현재 게임의 RenderMode */
	enum RENDER_TYPE { RTYPE_GAMEPLAY, RTYPE_UI, RTYPE_END };	/* 현재 객체의 RenderType */

	enum RENDER_UI { RUI_MAIN, RUI_PAIMON, RUI_TEAM, RUI_QUEST, RUI_MAP, RUI_CHAR, RUI_INVEN, RUI_GACHA, RUI_DIALOG, RUI_END };

protected:
	CDeciderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeciderObject(const CDeciderObject& rhs);
	virtual ~CDeciderObject() = default;

protected:
	static RENDER_MODE	m_eRenderMode;
	RENDER_TYPE			m_eRenderType = { RTYPE_END };

	static RENDER_UI	m_eRenderUI;
	RENDER_UI			m_eUIState = { RUI_END };

public:
	static void			Set_RenderMode(RENDER_MODE eMode) { m_eRenderMode = eMode; }
	static RENDER_MODE	Get_RenderMode() { return m_eRenderMode; }

public:
	void	Invalidate_CameraMode(RENDER_MODE eMode, _bool bMove = TRUE);
	HRESULT	Verify_TickValidity();		/* 유효성 검증 */
	HRESULT	Verify_RenderValidity();	/* 유효성 검증 */

public:
	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual HRESULT PreviousTick(_float fTimeDelta) PURE;
	virtual HRESULT Tick(_float fTimeDelta) PURE;
	virtual HRESULT LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END