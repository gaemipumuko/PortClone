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

class CSkill_WindRange final : public CSkillObj
{
private:
	CSkill_WindRange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_WindRange(const CSkill_WindRange& rhs);
	virtual ~CSkill_WindRange() = default;

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
	_vector				m_vColor = {};
	
	_float				m_fLifeTime = 0.f;

	// Shader 관련
	_float2				m_vScaleUV = {};
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
	static CSkill_WindRange*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END