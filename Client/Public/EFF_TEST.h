#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEff_TEST_CLASS final : public CEffect
{
private:
	enum TEX_CHANNELS { RED,GREEN,BLUE,CHANNEL_END };

private:
	CEff_TEST_CLASS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEff_TEST_CLASS(const CEff_TEST_CLASS& rhs);
	virtual ~CEff_TEST_CLASS() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CTexture*		m_pRampTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_float			m_fLifeTime = 0.f;
	_vector			m_vColor = {0.f,0.f,0.f,0.f};


	// Shader 관련
	_float			m_fTimeAcc = 0.f;
	_float			m_fColorRate0 = 0.f;

	_float			m_fAlpha = { 255.f };

	_bool			m_bRchannel = { true };
	_bool			m_bGchannel = { true };
	_bool			m_bBchannel = { true };

	_int	m_eTextureChannel = { CHANNEL_END };

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
	static CEff_TEST_CLASS*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END