#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
public:
	enum RT_TYPE		{ RT_FLOAT, RT_VECTOR, RT_END };
	enum RT_SINGULTI	{ RSM_SINGLE, RSM_MULTI, RSM_END };

private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	ID3D11Texture2D*			m_pTexture2D = { nullptr };

	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };

	RT_TYPE						m_eRenderTargetType = { RT_END };
	RT_SINGULTI					m_eRenderTargetSingulti = { RSM_END };

	_int						m_iNumRenderTarget = { 1 };
	_float						m_fClearValue = { };
	_float4						m_vClearColor = { };

#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix = { };
#endif

public:
	ID3D11RenderTargetView*		Get_RenderTargetView() { return m_pRTV; }
	ID3D11ShaderResourceView*	Get_ShaderResourceView() { return m_pSRV; }

public:
	/* SRV를 매개변수로 받은 Shader에 넘김 */
	HRESULT Bind_RenderTargetView(class CShader* pShader, const _char* pConstantName);
	void	Clear_RenderTargetView();

#ifdef _DEBUG
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public:
	HRESULT	Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue);
	HRESULT	Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	HRESULT	Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize);

public:
	static CRenderTarget*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue);
	static CRenderTarget*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	static CRenderTarget*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize);
	virtual void			Free() override;
};

END