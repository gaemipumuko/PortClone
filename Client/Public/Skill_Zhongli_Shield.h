#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CSkill_Zhongli_Shield final : public CSkillObj
{
private:
	CSkill_Zhongli_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Zhongli_Shield(const CSkill_Zhongli_Shield& rhs);
	virtual ~CSkill_Zhongli_Shield() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CTexture*			m_pCrackTextureCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};

	_float				m_fLifeTime = 0.f;
	_float				m_fCrackDisplay = { 0.f };

	_int				m_CrackIndex = {};

	_bool				m_bDead = { FALSE };

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
	static CSkill_Zhongli_Shield*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END