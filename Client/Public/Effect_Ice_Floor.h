#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Ice_Floor final : public CEffect
{
private:
	CEffect_Ice_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Ice_Floor(const CEffect_Ice_Floor& rhs);
	virtual ~CEffect_Ice_Floor() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_vector			m_vScale = {1.f,1.f,0.f,0.f};

	// Shader 관련
	_float			m_fColorRate = 0.f;
	_float2			m_vScaleUV = {};
	_float2			m_vSpinUV = {};

	// 충돌
	_int			m_iColliderID = { -1 };

private:
	HRESULT	Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CEffect_Ice_Floor*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END