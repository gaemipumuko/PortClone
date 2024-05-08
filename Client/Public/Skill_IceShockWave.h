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

class CSkill_IceShockWave final : public CSkillObj
{
private:
	CSkill_IceShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_IceShockWave(const CSkill_IceShockWave& rhs);
	virtual ~CSkill_IceShockWave() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};

	// Á¦¾î
	SKILLOBJID			m_eSkill = { SKILLOBJID::SKILLOBJ_END };
	_float3				m_vDir = {};
	_float				m_fLifeTime = 0.f;

	list<_int>			m_HitPlayerIndex;
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
	static CSkill_IceShockWave*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END