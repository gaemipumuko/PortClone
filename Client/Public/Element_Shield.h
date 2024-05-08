#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CElement_Shield final : public CDeciderObject
{
private:
	CElement_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElement_Shield(const CElement_Shield& rhs);
	virtual ~CElement_Shield() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CTexture*			m_pCrackTextureCom = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fLifeTime = 0.f;
	_float				m_fTimeAcc = 0.f;

	_float				m_fCrackDisplay = {0.f};

	_int				m_CrackIndex = {};

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
	static CElement_Shield*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END