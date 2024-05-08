#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEff_Barbara_Normal_Atk final : public CEffect
{
private:
	CEff_Barbara_Normal_Atk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEff_Barbara_Normal_Atk(const CEff_Barbara_Normal_Atk& rhs);
	virtual ~CEff_Barbara_Normal_Atk() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CTexture*		m_pStarTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
private:
	_float4x4		m_WorldMatrix = {};

	_float2			m_vStarScale = {};
	_float2			m_vCircleScale = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_vector			m_vScale = { 1.f,1.f,0.f,0.f };

	// Shader 관련
	_float			m_fTimeAcc = 0.f;
	_float			m_fColorRate0 = 0.f;


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
	static CEff_Barbara_Normal_Atk*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END