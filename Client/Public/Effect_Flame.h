#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Flame final : public CEffect
{
private:
	CEffect_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Flame(const CEffect_Flame& rhs);
	virtual ~CEffect_Flame() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4		m_WorldMatrix = {};

private:
	_float			m_fFrameTime = { 0.f };

	CGameObject*	m_pObject = { nullptr };

	_float			m_fLifeTime = 0.f;

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
	static CEffect_Flame*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

END