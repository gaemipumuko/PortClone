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

class CBabaraBasicAttack final : public CSkillObj
{
public:
	typedef struct tagBarbaraAtkDesc
	{
		SKILLOBJ_DESC tSkillDesc;
		BARBARA_ATTACK eBarbaraAtkType;
	}BARBARA_ATK_DESC;
private:
	CBabaraBasicAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBabaraBasicAttack(const CBabaraBasicAttack& rhs);
	virtual ~CBabaraBasicAttack() = default;

private:
	BARBARA_ATTACK		m_eType = BARBARA_ATK_END;
	_float				m_fDamage = { 0.f };

	_float				m_fLifeTime = { 0.f };
	_float				m_fAttackTime = { 0.f };
	_bool				m_bAttack = { FALSE };

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

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
	static CBabaraBasicAttack*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END