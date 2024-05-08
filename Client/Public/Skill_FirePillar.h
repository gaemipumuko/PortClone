#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CSkill_FirePillar final : public CSkillObj
{
private:
	CSkill_FirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FirePillar(const CSkill_FirePillar& rhs);
	virtual ~CSkill_FirePillar() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};

	SKILLOBJID			m_eSkill = { SKILLOBJID::SKILLOBJ_END };

	_float				m_fLifeTime = 0.f;
	_float				m_fStopTime = 0.f;
	_float				m_fDissapearSpeed = 0.f;

	_bool				m_bIsReachTop = FALSE;
	_bool				m_bIsCollGround = FALSE;
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
	static CSkill_FirePillar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END