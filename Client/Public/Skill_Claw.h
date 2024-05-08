#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CSkill_Claw final : public CDeciderObject
{
public:
	typedef struct tagClawDesc
	{
		_vector vPosition;
		_vector vDir;
		_vector vScale;
		_float	fAngle;
		_bool	bReverse;
	}CLAW_DESC;
private:
	CSkill_Claw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Claw(const CSkill_Claw& rhs);
	virtual ~CSkill_Claw() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fLifeTime = 0.f;
	_float				m_fTimeAcc = 0.f;


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
	static CSkill_Claw*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END