#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdPartyLib/stb/stb_image.h"

#include "GameInstance.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_fCurrentTexture(rhs.m_fCurrentTexture)
	, m_Textures(rhs.m_Textures)
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::CreateTGATextureFromFile(ID3D11Device* d3dDevice, const wchar_t* szFileName, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, size_t maxsize)
{
	_int iWidth = { }, iHeight = { }, iChannels = { };
	unsigned char* pData = stbi_load(ConvertWCToC(szFileName).c_str(), &iWidth, &iHeight, &iChannels, STBI_rgb_alpha);

	if (pData == nullptr)
		return E_FAIL;

	ID3D11Texture2D*			pTexture = nullptr;
	ID3D11ShaderResourceView*	pSRV	 = nullptr;

	D3D11_TEXTURE2D_DESC tTextureDesc = { };
	tTextureDesc.Width				= iWidth;
	tTextureDesc.Height				= iHeight;
	tTextureDesc.MipLevels			= tTextureDesc.ArraySize = 1;
	tTextureDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	tTextureDesc.SampleDesc.Count	= 1;
	tTextureDesc.Usage				= D3D11_USAGE_DEFAULT;
	tTextureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA tInitData = { };
	tInitData.pSysMem = pData;
	tInitData.SysMemPitch = iWidth * 4; // RGBA

	HRESULT hr = d3dDevice->CreateTexture2D(&tTextureDesc, &tInitData, &pTexture);

	if (FAILED(hr))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
	tSRVDesc.Format					= tTextureDesc.Format;
	tSRVDesc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDesc.Texture2D.MipLevels	= 1;

	hr = d3dDevice->CreateShaderResourceView(pTexture, &tSRVDesc, &pSRV);
	pTexture->Release();

	stbi_image_free(pData);

	*textureView = pSRV;

	return S_OK;
}

void CTexture::Set_CurrentTexture(_float fNumTexture)
{
	m_fCurrentTexture = fNumTexture; 
	
	if (m_fCurrentTexture > m_iNumTextures) 
		m_fCurrentTexture = 0;
}

ID3D11ShaderResourceView* CTexture::Get_ShaderResourceView(_uint iSRVIndex)
{
	if (iSRVIndex > m_iNumTextures)
		return nullptr;

	return m_Textures[iSRVIndex];
}

void CTexture::Set_NextAnimTexture(_float fTimeDelta, _float fScale, _float fMinTexture, _float fMaxTexture)
{
	m_fCurrentTexture += fTimeDelta * fScale;

	if (m_fCurrentTexture > ((fMaxTexture == -1.f) ? m_iNumTextures : fMaxTexture))
		m_fCurrentTexture = fMinTexture;
}

HRESULT CTexture::Bind_ShaderCurrentTexture(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_ShaderResourceView(pConstantName, m_Textures[static_cast<size_t>(Get_CurrentTexture())]);
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_ShaderResourceView(pConstantName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_ShaderResourceViews(pConstantName, &m_Textures.front(), m_iNumTextures);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	_tchar szTextureFilePath[MAX_PATH] = TEXT("");

	m_Textures.reserve(iNumTextures);
	m_iNumTextures = iNumTextures;

	for (size_t idx = 0; idx < iNumTextures; ++idx)
	{
		ID3D11ShaderResourceView* pTexture = nullptr;

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), idx);

		_tchar szEXT[MAX_PATH] = TEXT("");
		_wsplitpath_s(szTextureFilePath, NULL, NULL, NULL, NULL, NULL, NULL, szEXT, MAX_PATH);

		HRESULT hr = E_FAIL;
		if (!lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pTexture);
		else if (!lstrcmp(szEXT, TEXT(".tga")))
			hr = CreateTGATextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pTexture);
		else
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pTexture);

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTexture)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTexture)))
	{
		MSG_BOX(TEXT("Failed to Create : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();
}
