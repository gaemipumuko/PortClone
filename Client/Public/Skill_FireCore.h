#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CEff_Effigy_Recharge_Trail;

class CSkill_FireCore final : public CPart_Body
{
public:
	typedef struct tagFireCoreDesc
	{
		SKILLOBJID			eSkillObjID;
		_vector				vLinkPos;
		_vector				vSpawnPos;
		_vector				vScale;
	}FIRECORE_DESC;

private:
	CSkill_FireCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FireCore(const CSkill_FireCore& rhs);
	virtual ~CSkill_FireCore() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	/*CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };*/

private:
	CEff_Effigy_Recharge_Trail* m_pTrail = { nullptr };
	_float4x4			m_WorldMatrix = {};

	SKILLOBJID			m_eSkill = { SKILLOBJID::SKILLOBJ_END };

	_float				m_fLifeTime = 0.f;
	_float				m_fStopTime = 0.f;
	_float				m_fHp = 2.f;

	_bool				m_bIsCollGround = FALSE;
	_bool				m_bAttack = { FALSE };
	_bool				m_bIgniteSuccess = FALSE;

public:
	void SetIgniteSuccess(_bool _IgniteSuccess) { m_bIgniteSuccess = _IgniteSuccess; }

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
	static CSkill_FireCore*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END