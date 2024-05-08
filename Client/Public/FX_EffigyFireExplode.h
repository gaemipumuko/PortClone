#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

#define NUM_SMOKE 8

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFX_EffigyFireExplode final : public CAssetFX
{
public:
	typedef struct tagExplodeDesc : public ASFX_DESC
	{
		_float				fExplodeExpand = { 1.f };
		_float				fScale = { 1.f };
		const _float4x4*	pBoneMatrix = { nullptr };
	} EXPLODE_DESC;

private:
	CFX_EffigyFireExplode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireExplode(const CFX_EffigyFireExplode& rhs);
	virtual ~CFX_EffigyFireExplode() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	_bool				m_bGlow = { TRUE };
	_bool				m_bFlare = { TRUE };
	_bool				m_bSmoke = { TRUE };
	_bool				m_bThorn = { FALSE };

	_float4x4			m_GlowMatrix = { };
	_float4x4			m_FlareMatrix = { };
	_float4x4			m_SmokeMatrix = { };

	_float4				m_SmokeCorrection[NUM_SMOKE] = { };
	_float4				m_DSmokeCorrection[NUM_SMOKE] = { };

private:
	_float				m_fFrame = { };
	_float4				m_vUVRate = { };

	_float				m_fExplodeScale = { 1.f };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CFX_EffigyFireExplode*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END