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

class CSkill_FireCube final : public CSkillObj
{
private:
	CSkill_FireCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FireCube(const CSkill_FireCube& rhs);
	virtual ~CSkill_FireCube() = default;

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
	_float				m_fMoveTimer = 0.f;
	_vector				m_vDir = {};
	_bool				m_bIsCollwithSomething = FALSE;
	_bool				m_bIsPlayerNearby = FALSE;

	_vector				m_vPrevDirToPlayer = {};

	_bool				m_bAttack = { FALSE };

	list<wstring>		m_ParticleTags;

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
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

public:
	static CSkill_FireCube*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END