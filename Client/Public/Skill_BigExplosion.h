#pragma once
#include "Client_Defines.h"
#include "SkillObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTransform;
END

BEGIN(Client)

class CSKill_BigExplosion final : public CSkillObj
{
private:
	CSKill_BigExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSKill_BigExplosion(const CSKill_BigExplosion& rhs);
	virtual ~CSKill_BigExplosion() = default;

private:
	_float		m_fLifeTime = 0.f;
	_float		m_fTimeAcc = 0.f;
	_uint		m_iExplosion = 0;
	_bool		m_bShockWave = { FALSE };

private:
	CShader*	m_pShaderCom = { nullptr };
	CRenderer*	m_pRendererCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

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
	static CSKill_BigExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END