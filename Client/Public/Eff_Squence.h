#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEff_Squence final : public CEffect
{
private:
	CEff_Squence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEff_Squence(const CEff_Squence& rhs);
	virtual ~CEff_Squence() = default;
	 
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
private:
	_float4x4		m_WorldMatrix = {};

	// 제어
	_vector			m_vScale = {1.f,1.f,0.f,0.f};


	// Shader 관련
	_int			m_iFrameRow = {2};
	_int			m_iFrameCol = {4};
	_int			m_iCurrentFrameRow = {1};
	_int			m_iCurrentFrameCol = {1};

	_float			m_fTimeAcc = 0.f;

	_float2			m_vUVPos = {};
	_float2			m_vUVScale = {};	


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
	static CEff_Squence*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END