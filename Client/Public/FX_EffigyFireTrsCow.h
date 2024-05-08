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

class CFX_EffigyFireTrsCow final : public CAssetFX
{
public:
	typedef struct tagTrsCowDesc : public ASFX_DESC
	{
		const _float4x4*	pBoneMatrix = { };
	} TRSCOW_DESC;

private:
	CFX_EffigyFireTrsCow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireTrsCow(const CFX_EffigyFireTrsCow& rhs);
	virtual ~CFX_EffigyFireTrsCow() = default;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

	const _float4x4*	m_pBoneMatrix = { };

private:
	_float4				m_vFrame = { };
	_bool				m_bFireAura = { FALSE };
	wstring				m_strConvParticle = { };

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
	static CFX_EffigyFireTrsCow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END