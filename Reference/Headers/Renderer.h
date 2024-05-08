#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {
		RG_PRIORITY, // PS_OUT_DIFFUSE
		RG_SHADOW, // PS OUT X
		RG_TERRAIN, RG_OBJECT, RG_NONBLEND, RG_MTRAIL,
		RG_BLEND, 
		RG_TRAIL, RG_EFFECT, // PS_OUT_BWOIT
		RG_POSTEFFECT, // PS_OUT_DIFFUSE
		RG_PREVUI, RG_LATEUI, RG_POPUPUI, RG_FONT, RG_MOUSE,
		RG_END
	};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

private:
	list<class CGameObject*>	m_RenderObjects[RG_END];
	list<class CParticle*>		m_RenderParticles;
#ifdef _DEBUG
	_bool						m_bRenderRTV = { FALSE };
	_bool						m_bRenderDebugCom = { TRUE };
	list<class CComponent*>		m_DebugComponent;
#endif

#ifdef _DEBUG
public:
	void						Set_RenderRTV(_bool bRender) { m_bRenderRTV = bRender; }
	_bool						Get_RenderRTV() { return m_bRenderRTV; }

	void						Set_RenderDebugCom(_bool bRender) { m_bRenderDebugCom = bRender; }
	_bool						Set_RenderDebugCom() { return m_bRenderDebugCom; }
#endif

private:
	class CShader*				m_pShader = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix = { };
	_float4x4					m_ViewMatrix = { }, m_ProjMatrix = { };

private:
	ID3D11DepthStencilView*		m_pShadowDSV = { nullptr };
	ID3D11ShaderResourceView*	m_pShadowSRV = { nullptr };

	_float						m_fCascadeClip[3] = {};
	_float4x4					m_ShadowViewProj[3] = { };
#ifdef _DEBUG
	_float4x4					m_LightDepthMatrix = { };
#endif

private:
	_float						m_fFovy = { 60.f };
	_float						m_fAspect = { 16.f / 9.f };
	_float						m_fCamNear = { 0.1f };
	_float						m_fCamFar = { 100.f };

private:
	HRESULT Ready_ShadowDepthStencilView(_uint iSizeX, _uint iSizeY);
	void	Extract_FrustumBoundSphere(_float fNear, _float fFar, _fvector& vCenter, _float& fBoundRadius);

public:
	void	Set_ViewPort(_uint iSizeX, _uint iSizeY);

	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_RenderParticle(class CParticle* pParticle);
#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
#endif

	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Priority();			/* SkyBox, CharWIn의 BG 등 */
	HRESULT Render_Shadow();			/* Shadow */
	HRESULT Render_Terrain();			/* Terrain과 지형 Mesh 등 */
	HRESULT Render_NonBlend();			/* AnimMesh, Mesh 등 */
	HRESULT Render_MTrail();			/* 안드리우스 잔상 패턴 등 */
	HRESULT Render_LightAcc();			/* 점 광원, 방향성 광원 등 */
	HRESULT Render_Blend();				/* Alpha Mesh 등 */

	HRESULT Render_Trail();				/* Sword Trail */
	HRESULT Render_Particle();			/* Particle */

	HRESULT Render_SceneDeferred();		/* Scene Deferred */
	HRESULT	Render_EffectDeferred();	/* Trail, Effect Deferred */
	HRESULT	Render_PostEffect();		/* Non PostProcessing (Alpha Texture) */
	
	HRESULT	Render_UI();				/* PrevUI, LateUI, PopupUI */
	HRESULT Render_Font();
	HRESULT Render_Mouse();

#ifdef _DEBUG
	HRESULT Render_Debug();
	HRESULT Render_LightDepth();
#endif

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static	CRenderer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END