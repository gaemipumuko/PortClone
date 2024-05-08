#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CSkill_Phoenix final : public CSkillObj
{
private:
	CSkill_Phoenix(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Phoenix(const CSkill_Phoenix& rhs);
	virtual ~CSkill_Phoenix() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};


	_float				m_fLifeTime = { 0.f };
	_float				m_fHitDuration = { 0.f };

	list<_int>			m_HitMonsterIDList;

private:
	_bool				m_bParticle = { FALSE };
	_bool				m_bAttackStop = { FALSE };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

	void	Collision_Monster();
	void	Release_Collision();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CSkill_Phoenix*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END