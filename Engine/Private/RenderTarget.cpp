#include "RenderTarget.h"

#include "GameInstance.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Bind_RenderTargetView(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_ShaderResourceView(pConstantName, m_pSRV);
}

void CRenderTarget::Clear_RenderTargetView()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (m_eRenderTargetType == RT_FLOAT ? &m_fClearValue : (_float*)&m_vClearColor));
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	_uint		iNumViewports = { 1 };

	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_WorldMatrix.m[0][0] = fSizeX;
	m_WorldMatrix.m[1][1] = fSizeY;
	m_WorldMatrix.m[3][0] = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix.m[3][1] = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_ShaderResourceView((m_eRenderTargetSingulti == RSM_SINGLE ? "g_Texture" : "g_TextureArray"), m_pSRV)))
		return E_FAIL;

	if (FAILED(pShader->Begin((m_eRenderTargetSingulti == RSM_SINGLE ? DEFERRED_SINGLEDEBUG : DEFERRED_MULTIDEBUG))))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}
#endif

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue)
{
	m_fClearValue = fValue;
	m_eRenderTargetType = RT_FLOAT;
	m_eRenderTargetSingulti = RSM_SINGLE;

	D3D11_TEXTURE2D_DESC			TextureDesc = {};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	m_vClearColor = vColor;
	m_eRenderTargetType = RT_VECTOR;
	m_eRenderTargetSingulti = RSM_SINGLE;

	D3D11_TEXTURE2D_DESC			TextureDesc = {};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize)
{
	m_vClearColor = vColor;
	m_iNumRenderTarget = iArraySize;
	m_eRenderTargetType = RT_VECTOR;
	m_eRenderTargetSingulti = RSM_MULTI;

	D3D11_TEXTURE2D_DESC			TextureDesc = {};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = iArraySize;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = { };
	RTVDesc.Format = TextureDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RTVDesc.Texture2DArray.FirstArraySlice = 0;
	RTVDesc.Texture2DArray.ArraySize = TextureDesc.ArraySize;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, &RTVDesc, &m_pRTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = { };
	SRVDesc.Format = TextureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = TextureDesc.ArraySize;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, fValue)))
	{
		MSG_BOX(TEXT("Failed to Create : CRenderTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vColor)))
	{
		MSG_BOX(TEXT("Failed to Create : CRenderTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vColor, iArraySize)))
	{
		MSG_BOX(TEXT("Failed to Create : CRenderTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);

	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
