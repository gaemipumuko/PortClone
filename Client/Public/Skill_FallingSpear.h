#pragma once

#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CSkill_FallingSpear final : public CSkillObj
{
private:
	CSkill_FallingSpear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FallingSpear(const CSkill_FallingSpear& rhs);
	virtual ~CSkill_FallingSpear() = default;

private:
	_float4x4		m_WorldMatrix = {};
	_float4			m_vDropPoint = {};
	_float			m_fHeight = { 0.f };
	_float			m_fLifeTime = { 0.f };
	_float			m_fCreateDelay = { 0.f };

	_bool			m_bCreate = { FALSE };
	_bool			m_bEffect = { FALSE };

private:
	_float			m_fDissolve = { 0.f };
	_float			m_fDissolveSize = { 0.2f };
	_float3			m_vDissolveColor = {1.f, 1.f, 0.5f};

	_float			m_fAddColor = { 0.5f };
	_float3			m_vAddColor = {};

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

	CTexture*		m_pDissolveCom = { nullptr };

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
	static CSkill_FallingSpear*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

END