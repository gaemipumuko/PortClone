#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CFX_EffigyFireTrsFish final : public CAssetFX
{
public:
	typedef struct tagTrsFishDesc : public ASFX_DESC
	{
		const _float4x4*	pBoneMatrix = { };
	} TRSFISH_DESC;

private:
	CFX_EffigyFireTrsFish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireTrsFish(const CFX_EffigyFireTrsFish& rhs);
	virtual ~CFX_EffigyFireTrsFish() = default;

private:
	CModel*				m_pModelCom = {nullptr};
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

	const _float4x4*	m_pBoneMatrix = { };

	_float4x4			m_TrsFishMatrix = { };	// Ω√¿€

private:
	_float4				m_vFrame = { };

	_float				m_fInitY = { };
	_bool				m_bInitEffect = { FALSE };
	_bool				m_bDissolveEffect = { FALSE };

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
	static CFX_EffigyFireTrsFish*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END