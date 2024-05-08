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

class CElementalReaction final : public CSkillObj
{
private:
	CElementalReaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElementalReaction(const CElementalReaction& rhs);
	virtual ~CElementalReaction() = default;

private:
	ELEMENTAL_REACTION	m_eReaction = { REACT_END };
	ELEMENT				m_eElement = { ELEMENT_END };
	COLLID				m_eCollider = { COLL_END };
	_float4				m_vColor = {};
	_float				m_fLifeTime = { 0.f };
	_float				m_fDamage = { 0.f };
	_bool				m_bAttack = { FALSE };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

private:
	HRESULT			Add_Component(void* pArg);
	HRESULT			Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CElementalReaction*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

END