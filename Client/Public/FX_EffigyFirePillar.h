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

class CFX_EffigyFirePillar final : public CAssetFX
{
public:
	enum STEP		{ STEP_1, STEP_2, STEP_3, STEP_4, STEP_END };
	enum FIREPILLAR { FP_CONE, FP_CYLINDER, FP_CRACK, FP_END };

private:
	CFX_EffigyFirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFirePillar(const CFX_EffigyFirePillar& rhs);
	virtual ~CFX_EffigyFirePillar() = default;

private:
	CModel*				m_pModelCom[FP_END] = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom[FP_END] = { nullptr };

private:
	STEP				m_eStep = { STEP_END };
	_float4				m_vFrame = { };

	_float4				m_vConeScale = { 3.f, 1.f, 3.f, 0.f };
	_float4				m_vCylinderScale = { 4.5f, 2.f, 4.5f, 0.f };
	_float4				m_vCircleScale = { 0.015f, 1.f, 0.015f, 0.f };

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
	static CFX_EffigyFirePillar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END