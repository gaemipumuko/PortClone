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

class CSkill_Pillar final : public CSkillObj
{
private:
	CSkill_Pillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Pillar(const CSkill_Pillar& rhs);
	virtual ~CSkill_Pillar() = default;

private:
	_float4x4			m_WorldMatrix = {};

	_float3				m_vDir = {};

	_float				m_fLifeTime = 0.f;
	_float				m_fUp = { -4.f };

	_float				m_fPillarLineLight = { 0.f };
	_float				m_fFadeIn = { 0.f };
	
	_bool				m_bDead = { FALSE };

	_float				m_fWaveDuration = { 0.f };
	list<_int>			m_HitMonsterIDList;

	_bool				m_bAttackStop = { FALSE };

private:
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

private:
	class CCharacterController* m_pController = { nullptr };

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
	static CSkill_Pillar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END