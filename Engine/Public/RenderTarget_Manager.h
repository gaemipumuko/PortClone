#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
private:
	CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget_Manager() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	ID3D11RenderTargetView*	m_pBackBufferRTV = {nullptr};
	ID3D11DepthStencilView*	m_pMainDepthStencilView = { nullptr };

private:
	map<const wstring, class CRenderTarget*>		m_RenderTargets;
	map<const wstring, list<class CRenderTarget*>>	m_MultiRenderTargets;

public:
	HRESULT Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue);
	HRESULT Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor);
	HRESULT Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize);
	HRESULT Bind_RenderTarget(class CShader* pShader, _char* pConstantName, const wstring& strTargetTag);
	
	HRESULT Add_MultiRenderTarget(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MultiRenderTarget(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencilView = nullptr, _bool bBackBuffer = FALSE, _bool bClearBuffer = TRUE);
	HRESULT Begin_NonDepthMultiRenderTarget(const wstring& strMRTTag, _bool bBackBuffer = FALSE, _bool bClearBuffer = TRUE);
	HRESULT End_MultiRenderTarget();

	class CRenderTarget*		Find_RenderTarget(const wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MultiRenderTarget(const wstring& strMRTTag);
	ID3D11ShaderResourceView*	Find_ShaderResourceView(const wstring& strTargetTag);

#ifdef _DEBUG
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const wstring& strMTRTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public:
	HRESULT	Initialize();

public:
	static CRenderTarget_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void					Free() override;
};

END