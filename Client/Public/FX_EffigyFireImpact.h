#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
END

BEGIN(Client)

class CFX_EffigyFireImpact final : public CAssetFX
{
	enum STEP		{ STEP_1, STEP_2, STEP_3, STEP_4, STEP_5, STEP_6, STEP_END };
	enum FIREIMPACT { FIRE_CRACK, FIRE_IMPACT_1, FIRE_IMPACT_2, FIRE_IMPACT_3, FIRE_END };

private:
	CFX_EffigyFireImpact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireImpact(const CFX_EffigyFireImpact& rhs);
	virtual ~CFX_EffigyFireImpact() = default;

private:
	CModel*				m_pModelCom[FIRE_END] = {nullptr};
	CShader*			m_pShaderCom = {nullptr};
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom[FIRE_END] = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	STEP				m_eStep = { STEP_END };
	_float				m_fFrame = { };
	_float4				m_vFrame = { };

	_int				m_iColliderID = {};
	_bool				m_bAttack = { FALSE };
	_bool				m_bInitThorn = { FALSE };

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
	static CFX_EffigyFireImpact*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END