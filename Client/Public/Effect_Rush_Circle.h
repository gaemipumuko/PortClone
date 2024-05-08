#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Rush_Circle final : public CEffect
{
private:
	CEffect_Rush_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Rush_Circle(const CEffect_Rush_Circle& rhs);
	virtual ~CEffect_Rush_Circle() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_vector			m_vScale = {1.f,1.f,0.f,0.f};

	// Shader 관련
	_float			m_fColorRate = 0.f;


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
	static CEffect_Rush_Circle*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END