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

class CRaiden_ElementAttack final : public CSkillObj
{
public:
	typedef struct tagRaidenSkillDesc
	{
		SKILLOBJ_DESC tSkillDesc;
		RAIDEN_SKILL eRaidenSkillType;
	}RAIDEN_SKILL_DESC;
private:
	CRaiden_ElementAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaiden_ElementAttack(const CRaiden_ElementAttack& rhs);
	virtual ~CRaiden_ElementAttack() = default;

private:
	RAIDEN_SKILL		m_eType = { RAIDEN_ELEMENTAL_END };

	_float				m_fLifeTime = { 0.f };
	_float				m_fAttackTime = { 0.f };
	_bool				m_bAttack = { FALSE };

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT			Add_Component(void* pArg);
	HRESULT			Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT	PreviousTick(_float fTimeDelta) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual HRESULT	LateTick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	static CRaiden_ElementAttack*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END