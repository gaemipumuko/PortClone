#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Hitting final : public CEffect
{
private:
	CEffect_Hitting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Hitting(const CEffect_Hitting& rhs);
	virtual ~CEffect_Hitting() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_vector			m_vScale = {1.f,1.f,0.f,0.f};
	_vector			m_fScale1 = {};
	_vector			m_fScale2 = {};

	// Shader 관련
	_float			m_fTimeAcc = 0.f;
	_float			m_fColorRate0 = 0.f;
	_float			m_fColorRate1 = 0.01f;
	_float			m_fColorRate2 = 0.01f;


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
	static CEffect_Hitting*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END