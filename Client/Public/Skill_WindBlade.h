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

class CSkill_WindBlade final : public CSkillObj
{
private:
	CSkill_WindBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_WindBlade(const CSkill_WindBlade& rhs);
	virtual ~CSkill_WindBlade() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};

	// 제어
	SKILLOBJID			m_eSkill = { SKILLOBJID::SKILLOBJ_END };

	_float				m_fLifeTime = 0.f;
	_vector				m_vMoveDir = {};

	list<_int>			m_HitPlayerIndex;

	// Shader 관련
	_float2				m_vSpinUV = {};

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
	static CSkill_WindBlade*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END