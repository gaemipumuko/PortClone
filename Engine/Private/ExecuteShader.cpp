#include "ExecuteShader.h"

#include "Camera.h"
#include "GameInstance.h"

#define HALF_THREAD 16
#define FULL_THREAD 32

#define NORM_SIZE 1024

#define WINCX 1600
#define WINCY 900

CExecuteShader::CExecuteShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CExecuteShader::Ready_Shader()
{
#ifdef _DEBUG
	m_iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	m_iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(CExecuteShader::Ready_VertexShader()))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_PixelShader()))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ComputeShader()))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_BlendState()))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_SamplerState()))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ConstantBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExecuteShader::Ready_VertexShader()
{
	/* Bloom */
	if (FAILED(CExecuteShader::Ready_VertexShader(L"VS_BLOOM",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "VS_BLOOM")))
		return E_FAIL;

	/* Tone */
	if (FAILED(CExecuteShader::Ready_VertexShader(L"VS_TONE",
		L"../../Reference/ShaderFiles/Shader_Tone.fx", "VS_TONE")))
		return E_FAIL;

	/* Outline */
	if (FAILED(CExecuteShader::Ready_VertexShader(L"VS_OUTLINE",
		L"../../Reference/ShaderFiles/Shader_Outline.fx", "VS_OUTLINE")))
		return E_FAIL;

	return S_OK;
}

HRESULT CExecuteShader::Ready_PixelShader()
{
	/* Bloom */
	if (FAILED(CExecuteShader::Ready_PixelShader(L"PS_THRESHOLD",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "PS_THRESHOLD")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_PixelShader(L"PS_SCALING",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "PS_SCALING")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_PixelShader(L"PS_BLOOM",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "PS_BLOOM")))
		return E_FAIL;

	/* Tone */
	if (FAILED(CExecuteShader::Ready_PixelShader(L"PS_TONE",
		L"../../Reference/ShaderFiles/Shader_Tone.fx", "PS_TONE")))
		return E_FAIL;

	/* Outline */
	if (FAILED(CExecuteShader::Ready_PixelShader(L"PS_OUTLINE",
		L"../../Reference/ShaderFiles/Shader_Outline.fx", "PS_OUTLINE")))
		return E_FAIL;

	return S_OK;
}

HRESULT CExecuteShader::Ready_ComputeShader()
{
	/* Luminance */
	if (FAILED(CExecuteShader::Ready_ComputeShader(L"LUM_MAPPING",
		L"../../Reference/ShaderFiles/Shader_Luminance.fx", "LUM_MAPPING")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ComputeShader(L"LUM_CALCAVG",
		L"../../Reference/ShaderFiles/Shader_Luminance.fx", "LUM_CALCAVG")))
		return E_FAIL;

	/* Bloom */
	if (FAILED(CExecuteShader::Ready_ComputeShader(L"BLM_THRESHOLD",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "BLM_THRESHOLD")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ComputeShader(L"BLM_DOWNSCALE",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "BLM_DOWNSCALE")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ComputeShader(L"BLM_UPSCALE",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "BLM_UPSCALE")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ComputeShader(L"BLM_HBLUR",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "BLM_HBLUR")))
		return E_FAIL;

	if (FAILED(CExecuteShader::Ready_ComputeShader(L"BLM_VBLUR",
		L"../../Reference/ShaderFiles/Shader_Bloom.fx", "BLM_VBLUR")))
		return E_FAIL;

	return S_OK;
}

HRESULT CExecuteShader::Ready_BlendState()
{
	D3D11_BLEND_DESC BlendDesc = { };
	BlendDesc.AlphaToCoverageEnable					= FALSE;
	BlendDesc.IndependentBlendEnable				= FALSE;
	BlendDesc.RenderTarget[0].BlendEnable			= FALSE;
	BlendDesc.RenderTarget->RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pDevice->CreateBlendState(&BlendDesc, &m_pBlendState);

	return S_OK;
}

HRESULT CExecuteShader::Ready_SamplerState()
{
	/* PointSampler */
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	SamplerDesc.MipLODBias = 0.f;
	SamplerDesc.MaxLOD = FLT_MAX;
	SamplerDesc.MinLOD = -FLT_MAX;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.BorderColor[0] = 0.f;
	SamplerDesc.BorderColor[1] = 0.f;
	SamplerDesc.BorderColor[2] = 0.f;
	SamplerDesc.BorderColor[3] = 0.f;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	ID3D11SamplerState* pPointSampler = nullptr;

	if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &pPointSampler)))
		return E_FAIL;

	m_SamplerStates.emplace(L"SMP_Point", pPointSampler);

	/* LinearSampler */
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	ID3D11SamplerState* pLinearSampler = nullptr;

	if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &pLinearSampler)))
		return E_FAIL;

	m_SamplerStates.emplace(L"SMP_Linear", pLinearSampler);

	/* BlurSampler */
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	ID3D11SamplerState* pBlurSampler = nullptr;

	if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &pBlurSampler)))
		return E_FAIL;

	m_SamplerStates.emplace(L"SMP_Blur", pBlurSampler);

	/* AnisotropicSampler */
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	SamplerDesc.MaxAnisotropy = 16;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	ID3D11SamplerState* pAnisotropicSampler = nullptr;

	if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &pAnisotropicSampler)))
		return E_FAIL;

	m_SamplerStates.emplace(L"SMP_Anisotropic", pAnisotropicSampler);

	/* Sampler State */
	ID3D11SamplerState* pSamplerStates[4] = { pPointSampler, pLinearSampler, pBlurSampler, pAnisotropicSampler };

	m_pContext->PSSetSamplers(0, 4, pSamplerStates);
	m_pContext->CSSetSamplers(0, 4, pSamplerStates);

	return S_OK;
}

HRESULT CExecuteShader::Ready_ConstantBuffer()
{
	D3D11_BUFFER_DESC CBBuffDesc = { };
	CBBuffDesc.ByteWidth = 160;
	CBBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBBuffDesc.MiscFlags = 0;
	CBBuffDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA SubResource = { };
	SubResource.SysMemPitch = 0;
	SubResource.SysMemSlicePitch = 0;

	/* CB_WINDOW */
	_uint           iNumViewports = { 1 };
	D3D11_VIEWPORT  ViewportDesc = { };

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	ID3D11Buffer* pCBWindow = { };

	CB_WINDOW tCBWindow = { };
	tCBWindow.fWINCX = ViewportDesc.Width;
	tCBWindow.fWINCY = ViewportDesc.Height;

	SubResource.pSysMem = &tCBWindow;

	if (FAILED(m_pDevice->CreateBuffer(&CBBuffDesc, &SubResource, &pCBWindow)))
		return E_FAIL;

	m_ConstantBuffers.emplace(L"CB_Window", pCBWindow);

	/* CB_BLOOM */
	ID3D11Buffer* pCBBloom = { };

	CB_BLOOM tCBBloom = { };
	tCBBloom.fBloomScale = 0.7f;
	tCBBloom.fBloomThreshold = 10.f;

	SubResource.pSysMem = &tCBBloom;

	if (FAILED(m_pDevice->CreateBuffer(&CBBuffDesc, &SubResource, &pCBBloom)))
		return E_FAIL;

	m_ConstantBuffers.emplace(L"CB_Bloom", pCBBloom);

	/* CB_ColorGrade */
	ID3D11Buffer* pCBColorGrade = { };

	CB_COLORGRADE tCBColorGrade = { };
	tCBColorGrade.vColorFilter = { 1.f, 1.f, 1.f };
	tCBColorGrade.fSaturation = 1.f;
	tCBColorGrade.fExposure = 1.f;
	tCBColorGrade.fContrast = 1.f;

	SubResource.pSysMem = &tCBColorGrade;

	if (FAILED(m_pDevice->CreateBuffer(&CBBuffDesc, &SubResource, &pCBColorGrade)))
		return E_FAIL;

	m_ConstantBuffers.emplace(L"CB_ColorGrade", pCBColorGrade);

	/* CB_ORTHOWVP */
	ID3D11Buffer* pCBOrthoWVP = { };

	CB_ORTHOWVP	  tCBOrthoWVP = { };

	_float4x4	matWorld, matView, matProj;
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	matWorld._11 = ViewportDesc.Width;
	matWorld._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&matView, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	_matrix matWVP = XMLoadFloat4x4(&matWorld) * XMLoadFloat4x4(&matView) * XMLoadFloat4x4(&matProj);
	XMStoreFloat4(&tCBOrthoWVP.vOrthoWVPRight, matWVP.r[0]);
	XMStoreFloat4(&tCBOrthoWVP.vOrthoWVPUp, matWVP.r[1]);
	XMStoreFloat4(&tCBOrthoWVP.vOrthoWVPLook, matWVP.r[2]);
	XMStoreFloat4(&tCBOrthoWVP.vOrthoWVPPosition, matWVP.r[3]);

	SubResource.pSysMem = &tCBOrthoWVP;

	if (FAILED(m_pDevice->CreateBuffer(&CBBuffDesc, &SubResource, &pCBOrthoWVP)))
		return E_FAIL;

	m_ConstantBuffers.emplace(L"CB_OrthoWVP", pCBOrthoWVP);

	/* CB_OUTLINE */
	ID3D11Buffer* pCBOutline = { };

	CB_OUTLINE	  tCBOutline = { };

	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
		tCBOutline.fSceneFar = pCamera->Get_CamFar();
	else
		tCBOutline.fSceneFar = 200.f;

	SubResource.pSysMem = &tCBOutline;

	if (FAILED(m_pDevice->CreateBuffer(&CBBuffDesc, &SubResource, &pCBOutline)))
		return E_FAIL;

	m_ConstantBuffers.emplace(L"CB_Outline", pCBOutline);

	/* Constant Buffer */
	ID3D11Buffer* pConstantBuffers[5] = {
		m_ConstantBuffers.find(L"CB_Window")->second,
		m_ConstantBuffers.find(L"CB_Bloom")->second,
		m_ConstantBuffers.find(L"CB_ColorGrade")->second,
		m_ConstantBuffers.find(L"CB_OrthoWVP")->second,
		m_ConstantBuffers.find(L"CB_Outline")->second,
	};

	m_pContext->VSSetConstantBuffers(0, 5, pConstantBuffers);
	m_pContext->PSSetConstantBuffers(0, 5, pConstantBuffers);
	m_pContext->CSSetConstantBuffers(0, 5, pConstantBuffers);

	return S_OK;
}

RT_DESC* CExecuteShader::Ready_RenderTarget(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _bool bUAV, _bool bStore)
{
	for (auto& iter : m_pRenderTargets)
	{
		if (iter->bUse == TRUE)
			continue;

		if ((iter->bUAV == bUAV) && (iter->iWidth == iWidth) &&
			(iter->iHeight == iHeight) && (iter->eFormat == eFormat))
		{
			iter->bUse = TRUE;
			return iter;
		}
	}

	RT_DESC* pRenderTarget = new RT_DESC{ };

	pRenderTarget->bUAV = bUAV;
	pRenderTarget->iWidth = iWidth;
	pRenderTarget->iHeight = iHeight;
	pRenderTarget->eFormat = eFormat;

	D3D11_TEXTURE2D_DESC			TextureDesc = {};
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	if (bUAV == TRUE)
		TextureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pRenderTarget->pTexture2D)))
	{
		Safe_Release(pRenderTarget->pTexture2D);
		return pRenderTarget;
	}

	if (FAILED(m_pDevice->CreateRenderTargetView(pRenderTarget->pTexture2D, nullptr, &pRenderTarget->pRTV)))
	{
		Safe_Release(pRenderTarget->pRTV);
		Safe_Release(pRenderTarget->pTexture2D);
		return pRenderTarget;
	}

	if (FAILED(m_pDevice->CreateShaderResourceView(pRenderTarget->pTexture2D, nullptr, &pRenderTarget->pSRV)))
	{
		Safe_Release(pRenderTarget->pRTV);
		Safe_Release(pRenderTarget->pSRV);
		Safe_Release(pRenderTarget->pTexture2D);
		return pRenderTarget;
	}

	if ((bUAV == TRUE) && FAILED(m_pDevice->CreateUnorderedAccessView(pRenderTarget->pTexture2D, nullptr, &pRenderTarget->pUAV)))
	{
		Safe_Release(pRenderTarget->pRTV);
		Safe_Release(pRenderTarget->pSRV);
		Safe_Release(pRenderTarget->pUAV);
		Safe_Release(pRenderTarget->pTexture2D);
		return pRenderTarget;
	}

	if (bStore == TRUE)
	{
		pRenderTarget->bUse = TRUE;
		m_pRenderTargets.push_back(pRenderTarget);
	}

	return pRenderTarget;
}

HRESULT CExecuteShader::Ready_VertexShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry)
{
	ID3DBlob* pBlob = { nullptr };
	VS_DESC tVertexShaderDesc = { };

	if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(),
		NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntry.c_str(), "vs_5_0", m_iFlag, NULL, &tVertexShaderDesc.pBlob, &pBlob)))
	{
		auto error = (char*)pBlob->GetBufferPointer();
		MSG_BOX(ConvertCToWC(error).c_str());
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateVertexShader(
		tVertexShaderDesc.pBlob->GetBufferPointer(),
		tVertexShaderDesc.pBlob->GetBufferSize(),
		NULL, &tVertexShaderDesc.pVertexShader)))
		return E_FAIL;

	Safe_Release(pBlob);
	m_VertexShaders.emplace(strShaderTag, tVertexShaderDesc);

	return S_OK;
}

HRESULT CExecuteShader::Ready_PixelShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry)
{
	ID3DBlob* pBlob = { nullptr };
	PS_DESC tPixelShaderDesc = { };

	if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(),
		NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntry.c_str(), "ps_5_0", m_iFlag, NULL, &tPixelShaderDesc.pBlob, &pBlob)))
	{
		auto error = (char*)pBlob->GetBufferPointer();
		MSG_BOX(ConvertCToWC(error).c_str());
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreatePixelShader(
		tPixelShaderDesc.pBlob->GetBufferPointer(),
		tPixelShaderDesc.pBlob->GetBufferSize(),
		NULL, &tPixelShaderDesc.pPixelShader)))
		return E_FAIL;

	Safe_Release(pBlob);
	m_PixelShaders.emplace(strShaderTag, tPixelShaderDesc);

	return S_OK;
}

HRESULT CExecuteShader::Ready_ComputeShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry)
{
	ID3DBlob* pBlob = { nullptr };
	CS_DESC tComputeShaderDesc = { };

	if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(),
		NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntry.c_str(), "cs_5_0", m_iFlag, NULL, &tComputeShaderDesc.pBlob, &pBlob)))
	{
		auto error = (char*)pBlob->GetBufferPointer();
		MSG_BOX(ConvertCToWC(error).c_str());
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateComputeShader(
		tComputeShaderDesc.pBlob->GetBufferPointer(),
		tComputeShaderDesc.pBlob->GetBufferSize(),
		NULL, &tComputeShaderDesc.pComputeShader)))
		return E_FAIL;

	Safe_Release(pBlob);
	m_ComputeShaders.emplace(strShaderTag, tComputeShaderDesc);

	return S_OK;
}

HRESULT CExecuteShader::Update_LUTIndex(_uint iIndex)
{
	m_iLUTIndex = iIndex;

	return S_OK;
}

HRESULT CExecuteShader::Update_WindowCB(_float fWINCX, _float fWINCY)
{
	CB_WINDOW tCBWindow = { };
	tCBWindow.fWINCX = fWINCX;
	tCBWindow.fWINCY = fWINCY;

	ID3D11Buffer* pCB = m_ConstantBuffers.find(L"CB_Window")->second;

	D3D11_MAPPED_SUBRESOURCE SubResource = { };
	m_pContext->Map(pCB, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, &tCBWindow, sizeof(CB_WINDOW));
	m_pContext->Unmap(pCB, 0);

	/* Constant Buffer */
	ID3D11Buffer* pConstantBuffers[1] = {
		m_ConstantBuffers.find(L"CB_Window")->second
	};

	m_pContext->VSSetConstantBuffers(0, 1, pConstantBuffers);
	m_pContext->PSSetConstantBuffers(0, 1, pConstantBuffers);
	m_pContext->CSSetConstantBuffers(0, 1, pConstantBuffers);

	return S_OK;
}

HRESULT CExecuteShader::Update_BloomCB(_float fBloomScale, _float fBloomThreshold)
{
	CB_BLOOM tCBBloom = { };
	tCBBloom.fBloomScale = fBloomScale;
	tCBBloom.fBloomThreshold = fBloomThreshold;

	ID3D11Buffer* pCB = m_ConstantBuffers.find(L"CB_Bloom")->second;

	D3D11_MAPPED_SUBRESOURCE SubResource = { };
	m_pContext->Map(pCB, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, &tCBBloom, sizeof(CB_BLOOM));
	m_pContext->Unmap(pCB, 0);

	/* Constant Buffer */
	ID3D11Buffer* pConstantBuffers[5] = {
		m_ConstantBuffers.find(L"CB_Window")->second,
		m_ConstantBuffers.find(L"CB_Bloom")->second,
		m_ConstantBuffers.find(L"CB_ColorGrade")->second,
		m_ConstantBuffers.find(L"CB_OrthoWVP")->second,
		m_ConstantBuffers.find(L"CB_Outline")->second,
	};

	m_pContext->VSSetConstantBuffers(0, 5, pConstantBuffers);
	m_pContext->PSSetConstantBuffers(0, 5, pConstantBuffers);
	m_pContext->CSSetConstantBuffers(0, 5, pConstantBuffers);

	return S_OK;
}

HRESULT CExecuteShader::Update_ColorGradeCB(_float3 vColorFilter, _float fSaturation, _float fExposure, _float fContrast)
{
	CB_COLORGRADE tCBColorGrade = { };
	tCBColorGrade.vColorFilter = vColorFilter;
	tCBColorGrade.fSaturation = fSaturation;
	tCBColorGrade.fExposure = fExposure;
	tCBColorGrade.fContrast = fContrast;

	ID3D11Buffer* pCB = m_ConstantBuffers.find(L"CB_ColorGrade")->second;

	D3D11_MAPPED_SUBRESOURCE SubResource = { };
	m_pContext->Map(pCB, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, &tCBColorGrade, sizeof(CB_COLORGRADE));
	m_pContext->Unmap(pCB, 0);

	/* Constant Buffer */
	ID3D11Buffer* pConstantBuffers[5] = {
		m_ConstantBuffers.find(L"CB_Window")->second,
		m_ConstantBuffers.find(L"CB_Bloom")->second,
		m_ConstantBuffers.find(L"CB_ColorGrade")->second,
		m_ConstantBuffers.find(L"CB_OrthoWVP")->second,
		m_ConstantBuffers.find(L"CB_Outline")->second,
	};

	m_pContext->VSSetConstantBuffers(0, 5, pConstantBuffers);
	m_pContext->PSSetConstantBuffers(0, 5, pConstantBuffers);
	m_pContext->CSSetConstantBuffers(0, 5, pConstantBuffers);

	return S_OK;
}

void CExecuteShader::Set_ViewPort(_uint iSizeX, _uint iSizeY)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iSizeX;
	ViewPortDesc.Height = (_float)iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	Update_WindowCB(static_cast<_float>(iSizeX), static_cast<_float>(iSizeY));
}

void CExecuteShader::Execute_PixelShader(ID3D11ShaderResourceView* pSRV, const wstring& strShaderTag)
{
	PS_DESC		tPSDesc = m_PixelShaders.find(strShaderTag)->second;

	ID3D11ShaderResourceView* pNullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pNullSRVs);

	/* In Target */
	ID3D11ShaderResourceView* pSRVs[1] = { pSRV };
	m_pContext->PSSetShaderResources(0, 1, pSRVs);

	/* Pixel Shader */
	m_pContext->PSSetShader(tPSDesc.pPixelShader, NULL, 0);
	m_pContext->DrawIndexed(6, 0, 0);
}

void CExecuteShader::Execute_PixelShader(ID3D11ShaderResourceView* pSRV, ID3D11RenderTargetView* pRTV, const wstring& strShaderTag)
{
	PS_DESC		tPSDesc = m_PixelShaders.find(strShaderTag)->second;

	ID3D11ShaderResourceView* pNullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pNullSRVs);
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	/* In Target */
	ID3D11ShaderResourceView* pSRVs[1] = { pSRV };
	m_pContext->PSSetShaderResources(0, 1, pSRVs);

	/* Out Target */
	ID3D11RenderTargetView* pRTVs[1] = { pRTV };
	m_pContext->OMSetRenderTargets(1, pRTVs, NULL);

	/* Pixel Shader */
	m_pContext->PSSetShader(tPSDesc.pPixelShader, NULL, 0);
	m_pContext->DrawIndexed(6, 0, 0);

	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);
}

void CExecuteShader::Set_CSInputs(ID3D11ShaderResourceView* pSRV0, ID3D11ShaderResourceView* pSRV1, ID3D11ShaderResourceView* pSRV2, ID3D11ShaderResourceView* pSRV3)
{
	ID3D11ShaderResourceView* SRVs[4] = { pSRV0, pSRV1, pSRV2, pSRV3 };
	m_pContext->CSSetShaderResources(0, 4, SRVs);
}

void CExecuteShader::Clear_CSInputs()
{
	Set_CSInputs(NULL, NULL, NULL, NULL);
}

void CExecuteShader::Set_CSOutputs(ID3D11UnorderedAccessView* pUAV0, ID3D11UnorderedAccessView* pUAV1, ID3D11UnorderedAccessView* pUAV2, ID3D11UnorderedAccessView* pUAV3)
{
	ID3D11UnorderedAccessView* UAVs[4] = { pUAV0, pUAV1, pUAV2, pUAV3 };
	m_pContext->CSSetUnorderedAccessViews(0, 4, UAVs, NULL);
}

void CExecuteShader::Clear_CSOutputs()
{
	Set_CSOutputs(NULL, NULL, NULL, NULL);
}

HRESULT CExecuteShader::Initialize()
{
	if (FAILED(CExecuteShader::Ready_Shader()))
		return E_FAIL;

	m_pLUTTexture = CTexture::Create(m_pDevice, m_pContext, L"../../Reference/Resources/LUT/TX_FX_PPC_LUT_%d.tga", 62);

	m_pAvgLumTarget = CExecuteShader::Ready_RenderTarget(1, 1, DXGI_FORMAT_R32_FLOAT, TRUE, FALSE);
	m_pProcessedTarget = CExecuteShader::Ready_RenderTarget(WINCX, WINCY, DXGI_FORMAT_R16G16B16A16_UNORM, TRUE, FALSE);
	m_pInvenBlurTarget = CExecuteShader::Ready_RenderTarget(WINCX, WINCY, DXGI_FORMAT_R16G16B16A16_UNORM, TRUE, FALSE);

	return S_OK;
}

HRESULT CExecuteShader::Render_Scene(ID3D11ShaderResourceView* pSceneSRV)
{
	static _float4	vClearColor = { 1.f, 0.f, 1.f, 0.f };

	m_pContext->OMSetBlendState(m_pBlendState, (_float*)&vClearColor, 0xffffffff);
	m_pContext->ClearRenderTargetView(m_pProcessedTarget->pRTV, (_float*)&vClearColor);
	m_pContext->ClearRenderTargetView(m_pInvenBlurTarget->pRTV, (_float*)&vClearColor);
	
	/* Sampler State */
	ID3D11SamplerState* pSamplerStates[4] = {
		m_SamplerStates.find(L"SMP_Point")->second,
		m_SamplerStates.find(L"SMP_Linear")->second,
		m_SamplerStates.find(L"SMP_Blur")->second,
		m_SamplerStates.find(L"SMP_Anisotropic")->second,
	};

	m_pContext->PSSetSamplers(0, 4, pSamplerStates);
	m_pContext->CSSetSamplers(0, 4, pSamplerStates);

	if (FAILED(CExecuteShader::Calculate_AvgLum(pSceneSRV)))
		return E_FAIL;

	if (FAILED(CExecuteShader::Calculate_BloomRTV(pSceneSRV)))
		return E_FAIL;

	if (FAILED(CExecuteShader::Render_Tone(pSceneSRV)))
		return E_FAIL;

	if (FAILED(CExecuteShader::Render_Outline(pSceneSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CExecuteShader::Render_Effect(ID3D11ShaderResourceView* pSceneSRV)
{
	static _float4	vClearColor = { 1.f, 0.f, 1.f, 0.f };

	m_pContext->OMSetBlendState(m_pBlendState, (_float*)&vClearColor, 0xffffffff);

	if (m_bInvenBlur == TRUE)
	{
		/* Clear Outline Buff */
		m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

		Safe_Release(m_pBackBufferRTV);
		Safe_Release(m_pMainDepthStencilView);

		if (FAILED(CExecuteShader::Render_InvenBlur(pSceneSRV)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CExecuteShader::Calculate_AvgLum(ID3D11ShaderResourceView* pSceneSRV)
{
	/* Ready SRV */
	ID3D11ShaderResourceView* pWorldSRV = m_pGameInstance->Find_ShaderResourceView(TEXT("Target_SceneWorld"));

	_uint		iInSize = NORM_SIZE;

	CS_DESC		tLumMapping = m_ComputeShaders.find(L"LUM_MAPPING")->second;
	CS_DESC		tLumCalcAvg = m_ComputeShaders.find(L"LUM_CALCAVG")->second;

	RT_DESC*	pInitLum = CExecuteShader::Ready_RenderTarget(iInSize, iInSize, DXGI_FORMAT_R32_FLOAT, TRUE);

	/* DownScale Scene Image (1024x1024) */
	Set_CSInputs(pSceneSRV, pWorldSRV);
	Set_CSOutputs(NULL, pInitLum->pUAV);

	m_pContext->CSSetShader(tLumMapping.pComputeShader, NULL, NULL);
	m_pContext->Dispatch(FULL_THREAD, FULL_THREAD, 1);

	Clear_CSInputs();
	Clear_CSOutputs();

	/* Calculate Avg Luminance */
	RT_DESC* InLuminance = pInitLum;
	while (iInSize > 1)
	{
		RT_DESC*	OutLuminance = { nullptr };
		_uint		iOutSize = max(iInSize / (FULL_THREAD * 2), 1);

		if (iOutSize != 1)
			OutLuminance = CExecuteShader::Ready_RenderTarget(iOutSize, iOutSize, DXGI_FORMAT_R32_FLOAT, TRUE);
		else
			OutLuminance = m_pAvgLumTarget;

		Set_CSInputs(InLuminance->pSRV);
		Set_CSOutputs(NULL, OutLuminance->pUAV);

		m_pContext->CSSetShader(tLumCalcAvg.pComputeShader, NULL, NULL);
		m_pContext->Dispatch(iOutSize, iOutSize, 1);

		Clear_CSInputs();
		Clear_CSOutputs();

		iInSize = iOutSize;
		InLuminance->bUse = FALSE;
		InLuminance = OutLuminance;
	}

	pInitLum->bUse = FALSE;

	return S_OK;
}

HRESULT CExecuteShader::Calculate_BloomRTV(ID3D11ShaderResourceView* pSceneSRV)
{
	/* Ready SRV */
	ID3D11ShaderResourceView* pWorldSRV = m_pGameInstance->Find_ShaderResourceView(TEXT("Target_SceneWorld"));
	ID3D11ShaderResourceView* pCShaderSRV = m_pGameInstance->Find_ShaderResourceView(TEXT("Target_SceneCShader"));

	/* Draw Bloom Threshold */
	VS_DESC		tVSDesc			= m_VertexShaders.find(L"VS_BLOOM")->second;
	PS_DESC		tPSThreshold	= m_PixelShaders.find(L"PS_THRESHOLD")->second;
	PS_DESC		tPSBloom		= m_PixelShaders.find(L"PS_BLOOM")->second;
	CS_DESC		tCSHBlur		= m_ComputeShaders.find(L"BLM_HBLUR")->second;
	CS_DESC		tCSVBlur		= m_ComputeShaders.find(L"BLM_VBLUR")->second;

	RT_DESC*	pBloomSrc = CExecuteShader::Ready_RenderTarget(WINCX, WINCY, DXGI_FORMAT_R11G11B10_FLOAT, FALSE);

	/* Vertex Shader */
	m_pContext->VSSetShader(tVSDesc.pVertexShader, NULL, 0);

	/* GeoMetry Shader */
	m_pContext->GSSetShader(NULL, NULL, 0);

	/* In Target */
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	ID3D11ShaderResourceView* pSRVs[4] = { pSceneSRV, m_pAvgLumTarget->pSRV, pWorldSRV, pCShaderSRV };
	m_pContext->PSSetShaderResources(0, 4, pSRVs);

	/* Out Target */
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	ID3D11RenderTargetView* pRTVs[1] = { pBloomSrc->pRTV };
	m_pContext->OMSetRenderTargets(1, pRTVs, NULL);

	/* Pixel Shader */
	m_pContext->PSSetShader(tPSThreshold.pPixelShader, NULL, 0);
	m_pContext->DrawIndexed(6, 0, 0);

	/* Clear */
	ID3D11ShaderResourceView* pClsSRVs[4] = { NULL, NULL, NULL, NULL };
	m_pContext->PSSetShaderResources(0, 4, pClsSRVs);

	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);

	/* Down Scale */
	RT_DESC* pDownScaleA = CExecuteShader::Ready_RenderTarget(WINCX / 2, WINCY / 2, DXGI_FORMAT_R16G16B16A16_FLOAT, FALSE);
	Set_ViewPort(WINCX / 2, WINCY / 2);
	Execute_PixelShader(pBloomSrc->pSRV, pDownScaleA->pRTV, L"PS_SCALING");
	pBloomSrc->bUse = FALSE;

	RT_DESC* pDownScaleB = CExecuteShader::Ready_RenderTarget(WINCX / 4, WINCY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, FALSE);
	Set_ViewPort(WINCX / 4, WINCY / 4);
	Execute_PixelShader(pDownScaleA->pSRV, pDownScaleB->pRTV, L"PS_SCALING");

	RT_DESC* pDownScaleC = CExecuteShader::Ready_RenderTarget(WINCX / 8, WINCY / 8, DXGI_FORMAT_R16G16B16A16_FLOAT, TRUE);
	Set_ViewPort(WINCX / 8, WINCY / 8);
	Execute_PixelShader(pDownScaleB->pSRV, pDownScaleC->pRTV, L"PS_SCALING");

	/* Blur */
	for (_int iIndex = 0; iIndex < 3; ++iIndex)
	{
		RT_DESC* pBlurTarget = CExecuteShader::Ready_RenderTarget(WINCX / 8, WINCY / 8, DXGI_FORMAT_R16G16B16A16_FLOAT, TRUE);

		/* HBlur */
		Set_CSInputs(pDownScaleC->pSRV);
		Set_CSOutputs(NULL, pBlurTarget->pUAV);

		m_pContext->CSSetShader(tCSHBlur.pComputeShader, NULL, NULL);
		m_pContext->Dispatch(static_cast<_uint>(ceil((WINCX / 8.f) / 128.f)), static_cast<_uint>(ceil(WINCY / 8.f)), 1);

		Clear_CSInputs();
		Clear_CSOutputs();

		/* VBlur */
		Set_CSInputs(pBlurTarget->pSRV);
		Set_CSOutputs(NULL, pDownScaleC->pUAV);

		m_pContext->CSSetShader(tCSVBlur.pComputeShader, NULL, NULL);
		m_pContext->Dispatch(static_cast<_uint>(ceil(WINCX / 8.f)), static_cast<_uint>(ceil((WINCY / 8.f) / 128.f)), 1);

		Clear_CSInputs();
		Clear_CSOutputs();

		pBlurTarget->bUse = FALSE;
	}

	Set_ViewPort(WINCX / 4, WINCY / 4);
	Execute_PixelShader(pDownScaleC->pSRV, pDownScaleB->pRTV, L"PS_SCALING");
	pDownScaleC->bUse = FALSE;

	Set_ViewPort(WINCX / 2, WINCY / 2);
	Execute_PixelShader(pDownScaleB->pSRV, pDownScaleA->pRTV, L"PS_SCALING");
	pDownScaleB->bUse = FALSE;

	m_pBloomTarget = pDownScaleA;

	Set_ViewPort(WINCX, WINCY);

	return S_OK;
}

HRESULT CExecuteShader::Render_Tone(ID3D11ShaderResourceView* pSceneSRV)
{
	/* Ready SRV */
	ID3D11ShaderResourceView* pWorldSRV = m_pGameInstance->Find_ShaderResourceView(TEXT("Target_SceneWorld"));

	/* Draw ToneMapping */
	VS_DESC		tVSDesc = m_VertexShaders.find(L"VS_TONE")->second;
	PS_DESC		tPSDesc = m_PixelShaders.find(L"PS_TONE")->second;

	/* In Target */
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	ID3D11ShaderResourceView* pSRVs[4] = { pSceneSRV, m_pLUTTexture->Get_ShaderResourceView(m_iLUTIndex), m_pBloomTarget->pSRV, pWorldSRV };
	m_pContext->PSSetShaderResources(0, 4, pSRVs);

	/* Out Target */
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	ID3D11RenderTargetView* pRTVs[1] = { m_pProcessedTarget->pRTV };
	m_pContext->OMSetRenderTargets(1, pRTVs, NULL);

	/* Vertex Shader */
	m_pContext->VSSetShader(tVSDesc.pVertexShader, NULL, 0);

	/* GeoMetry Shader */
	m_pContext->GSSetShader(NULL, NULL, 0);

	/* Pixel Shader */
	m_pContext->PSSetShader(tPSDesc.pPixelShader, NULL, 0);
	m_pContext->DrawIndexed(6, 0, 0);

	/* Clear */
	ID3D11ShaderResourceView* pClsSRVs[4] = { NULL, NULL, NULL, NULL };
	m_pContext->PSSetShaderResources(0, 4, pClsSRVs);

	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);

	m_pBloomTarget->bUse = FALSE;

	return S_OK;
}

HRESULT CExecuteShader::Render_Outline(ID3D11ShaderResourceView* pSceneSRV)
{
	/* Ready SRV */
	ID3D11ShaderResourceView* pDepthSRV = m_pGameInstance->Find_ShaderResourceView(TEXT("Target_DeferredDepth"));
	ID3D11ShaderResourceView* pNormalSRV = m_pGameInstance->Find_ShaderResourceView(TEXT("Target_SceneNormal"));

	if (pDepthSRV == nullptr || pNormalSRV == nullptr)
		return E_FAIL;

	/* Ready ViewProjMatrix */
	CB_OUTLINE tCBOutline = { };

	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
		tCBOutline.fSceneFar = pCamera->Get_CamFar();
	else
		tCBOutline.fSceneFar = 200.f;

	_matrix OutViewMatrix = XMLoadFloat4x4(&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
	_matrix OutProjMatrix = XMLoadFloat4x4(&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

	XMStoreFloat4x4(&tCBOutline.SceneViewMatrix, XMMatrixTranspose(OutViewMatrix));
	XMStoreFloat4x4(&tCBOutline.SceneProjMatrix, XMMatrixTranspose(OutProjMatrix));

	ID3D11Buffer* pCB = m_ConstantBuffers.find(L"CB_Outline")->second;

	D3D11_MAPPED_SUBRESOURCE SubResource = { };
	m_pContext->Map(pCB, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, &tCBOutline, sizeof(CB_OUTLINE));
	m_pContext->Unmap(pCB, 0);

	m_pContext->VSSetConstantBuffers(4, 1, &m_ConstantBuffers.find(L"CB_Outline")->second);
	m_pContext->PSSetConstantBuffers(4, 1, &m_ConstantBuffers.find(L"CB_Outline")->second);
	m_pContext->CSSetConstantBuffers(4, 1, &m_ConstantBuffers.find(L"CB_Outline")->second);

	/* Draw Outline */
	VS_DESC		tVSDesc = m_VertexShaders.find(L"VS_OUTLINE")->second;
	PS_DESC		tPSDesc = m_PixelShaders.find(L"PS_OUTLINE")->second;

	/* In Target */
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	ID3D11ShaderResourceView* pSRVs[3] = { m_pProcessedTarget->pSRV, pDepthSRV, pNormalSRV };
	m_pContext->PSSetShaderResources(0, 3, pSRVs);

	if (m_bInvenBlur == TRUE)
	{
		/* Out Target */
		m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

		ID3D11RenderTargetView* pRTVs[1] = { m_pInvenBlurTarget->pRTV };
		m_pContext->OMSetRenderTargets(1, pRTVs, NULL);
	}

	/* Vertex Shader */
	m_pContext->VSSetShader(tVSDesc.pVertexShader, NULL, 0);

	/* GeoMetry Shader */
	m_pContext->GSSetShader(NULL, NULL, 0);

	/* Pixel Shader */
	m_pContext->PSSetShader(tPSDesc.pPixelShader, NULL, 0);
	m_pContext->DrawIndexed(6, 0, 0);

	/* Clear */
	ID3D11ShaderResourceView* pClsSRVs[3] = { NULL, NULL, NULL };
	m_pContext->PSSetShaderResources(0, 3, pClsSRVs);

	return S_OK;
}

HRESULT CExecuteShader::Render_InvenBlur(ID3D11ShaderResourceView* pSceneSRV)
{
	/* Draw Bloom Threshold */
	VS_DESC		tVSDesc			= m_VertexShaders.find(L"VS_BLOOM")->second;
	CS_DESC		tCSHBlur		= m_ComputeShaders.find(L"BLM_HBLUR")->second;
	CS_DESC		tCSVBlur		= m_ComputeShaders.find(L"BLM_VBLUR")->second;

	m_pContext->VSSetShader(tVSDesc.pVertexShader, NULL, 0);

	/* Down Scale */
	RT_DESC* pDownScaleA = CExecuteShader::Ready_RenderTarget(WINCX / 2, WINCY / 2, DXGI_FORMAT_R16G16B16A16_FLOAT, FALSE);
	Set_ViewPort(WINCX / 2, WINCY / 2);
	Execute_PixelShader(m_pInvenBlurTarget->pSRV, pDownScaleA->pRTV, L"PS_SCALING");

	RT_DESC* pDownScaleB = CExecuteShader::Ready_RenderTarget(WINCX / 4, WINCY / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, FALSE);
	Set_ViewPort(WINCX / 4, WINCY / 4);
	Execute_PixelShader(pDownScaleA->pSRV, pDownScaleB->pRTV, L"PS_SCALING");

	RT_DESC* pDownScaleC = CExecuteShader::Ready_RenderTarget(WINCX / 8, WINCY / 8, DXGI_FORMAT_R16G16B16A16_FLOAT, TRUE);
	Set_ViewPort(WINCX / 8, WINCY / 8);
	Execute_PixelShader(pDownScaleB->pSRV, pDownScaleC->pRTV, L"PS_SCALING");

	/* Blur */
	for (_int iIndex = 0; iIndex < 3; ++iIndex)
	{
		RT_DESC* pBlurTarget = CExecuteShader::Ready_RenderTarget(WINCX / 8, WINCY / 8, DXGI_FORMAT_R16G16B16A16_FLOAT, TRUE);

		/* HBlur */
		Set_CSInputs(pDownScaleC->pSRV);
		Set_CSOutputs(NULL, pBlurTarget->pUAV);

		m_pContext->CSSetShader(tCSHBlur.pComputeShader, NULL, NULL);
		m_pContext->Dispatch(static_cast<_uint>(ceil((WINCX / 8.f) / 128.f)), static_cast<_uint>(ceil(WINCY / 8.f)), 1);

		Clear_CSInputs();
		Clear_CSOutputs();

		/* VBlur */
		Set_CSInputs(pBlurTarget->pSRV);
		Set_CSOutputs(NULL, pDownScaleC->pUAV);

		m_pContext->CSSetShader(tCSVBlur.pComputeShader, NULL, NULL);
		m_pContext->Dispatch(static_cast<_uint>(ceil(WINCX / 8.f)), static_cast<_uint>(ceil((WINCY / 8.f) / 128.f)), 1);

		Clear_CSInputs();
		Clear_CSOutputs();

		pBlurTarget->bUse = FALSE;
	}

	Set_ViewPort(WINCX / 4, WINCY / 4);
	Execute_PixelShader(pDownScaleC->pSRV, pDownScaleB->pRTV, L"PS_SCALING");
	pDownScaleC->bUse = FALSE;

	Set_ViewPort(WINCX / 2, WINCY / 2);
	Execute_PixelShader(pDownScaleB->pSRV, pDownScaleA->pRTV, L"PS_SCALING");
	pDownScaleB->bUse = FALSE;

	Set_ViewPort(WINCX, WINCY);
	Execute_PixelShader(pDownScaleB->pSRV, L"PS_SCALING");
	pDownScaleA->bUse = FALSE;

	return S_OK;
}

CExecuteShader* CExecuteShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CExecuteShader* pInstance = new CExecuteShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CExecuteShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExecuteShader::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pLUTTexture);

	Safe_Release(m_pAvgLumTarget->pRTV);
	Safe_Release(m_pAvgLumTarget->pSRV);
	Safe_Release(m_pAvgLumTarget->pUAV);
	Safe_Release(m_pAvgLumTarget->pTexture2D);

	Safe_Delete(m_pAvgLumTarget);

	Safe_Release(m_pProcessedTarget->pRTV);
	Safe_Release(m_pProcessedTarget->pSRV);
	Safe_Release(m_pProcessedTarget->pUAV);
	Safe_Release(m_pProcessedTarget->pTexture2D);

	Safe_Delete(m_pProcessedTarget);

	Safe_Release(m_pInvenBlurTarget->pRTV);
	Safe_Release(m_pInvenBlurTarget->pSRV);
	Safe_Release(m_pInvenBlurTarget->pUAV);
	Safe_Release(m_pInvenBlurTarget->pTexture2D);

	Safe_Delete(m_pInvenBlurTarget);

	for (auto& iter : m_pRenderTargets)
	{
		Safe_Release(iter->pRTV);
		Safe_Release(iter->pSRV);
		Safe_Release(iter->pUAV);
		Safe_Release(iter->pTexture2D);

		Safe_Delete(iter);
	}

	for (auto& iter : m_VertexShaders)
	{
		Safe_Release(iter.second.pBlob);
		Safe_Release(iter.second.pVertexShader);
	}

	for (auto& iter : m_PixelShaders)
	{
		Safe_Release(iter.second.pBlob);
		Safe_Release(iter.second.pPixelShader);
	}

	for (auto& iter : m_ComputeShaders)
	{
		Safe_Release(iter.second.pBlob);
		Safe_Release(iter.second.pComputeShader);
	}

	Safe_Release(m_pBlendState);

	for (auto& iter : m_SamplerStates)
	{
		Safe_Release(iter.second);
	}

	for (auto& iter : m_ConstantBuffers)
	{
		Safe_Release(iter.second);
	}
}
