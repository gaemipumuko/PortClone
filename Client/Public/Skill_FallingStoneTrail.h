#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CSkill_FallingStoneTrail final : public CSkillObj
{
private:
	CSkill_FallingStoneTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FallingStoneTrail(const CSkill_FallingStoneTrail& rhs);
	virtual ~CSkill_FallingStoneTrail() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};

	_float				m_fTimeAcc = {};
	_float				m_fLifeTime = { 0.f };
	_bool				m_bAttack = { FALSE };

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
	static CSkill_FallingStoneTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END