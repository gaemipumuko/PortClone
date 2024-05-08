#include "Terrain.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Terrain.h"
#include "Texture.h"
#include "NaviMesh.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Ray.h"
#include "Brush.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pBrush = CBrush::Create();

	if (nullptr == m_pBrush)
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Apply_HeightMap(TEXT("../../Reference/Resources/Data/Map/WorldHeightMap.dat"))))
		return E_FAIL;

	if (FAILED(Create_Mask(TEXT("../../Reference/Resources/Texture/Terrain/MaskMap/WorldMaskMap.dds"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CTerrain::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('Z')) 
		m_bWire = !m_bWire;

	if (m_pGameInstance->Key_Down('X'))
		m_bShow = !m_bShow;

	return S_OK;
}

HRESULT CTerrain::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_TERRAIN, this);
	m_pRendererCom->Add_DebugComponent(m_pNaviCom);

	return S_OK;
}

HRESULT CTerrain::Render()
{
	if (true == m_bShow) 
	{
		if (true == m_bWire)
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			if (FAILED(m_pBrush->Render()))
				return E_FAIL;
		}

		else
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			if (FAILED(m_pBrush->Render()))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CTerrain::Resize_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	return m_pVIBufferCom->Resize_Terrain(iNumVerticesX, iNumVerticesZ);
}

HRESULT CTerrain::Apply_HeightMap(const wstring& strDataFilePath)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	return m_pVIBufferCom->Apply_HeightMap(strDataFilePath);
}

HRESULT CTerrain::Save_HeightMap(const wstring& strDataFilePath)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	return m_pVIBufferCom->Save_HeightMap(strDataFilePath);
}

HRESULT CTerrain::Create_Mask(const wstring& strDataFilePath)
{
	if (nullptr != m_pMaskSRV)
		Safe_Release(m_pMaskSRV);

	if (nullptr != m_pMaskData)
		Safe_Delete_Array(m_pMaskData);

	if (nullptr != m_pMaskTexture)
		Safe_Release(m_pMaskTexture);

	if (true == strDataFilePath.empty()) 
	{
		D3D11_TEXTURE2D_DESC tMaskTextureDesc = {};
		{
			tMaskTextureDesc.Width = m_pVIBufferCom->Get_NumVerticesX() - 1;
			tMaskTextureDesc.Height = m_pVIBufferCom->Get_NumVerticesZ() - 1;
			tMaskTextureDesc.MipLevels = 1;
			tMaskTextureDesc.ArraySize = 1;
			tMaskTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			tMaskTextureDesc.SampleDesc.Quality = 0;
			tMaskTextureDesc.SampleDesc.Count = 1;

			tMaskTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
			tMaskTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tMaskTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			tMaskTextureDesc.MiscFlags = 0;
		}

		m_pMaskData = new _ulong[tMaskTextureDesc.Width * tMaskTextureDesc.Height];
		ZeroMemory(m_pMaskData, sizeof(_ulong) * tMaskTextureDesc.Width * tMaskTextureDesc.Height);

		for (_uint i = 0; i < tMaskTextureDesc.Height; i++)
		{
			for (_uint j = 0; j < tMaskTextureDesc.Width; j++)
			{
				_uint iIndex = i * tMaskTextureDesc.Width + j;
				m_pMaskData[iIndex] = D3DCOLOR_ABGR(255, 0, 0, 0);
			}
		}

		D3D11_SUBRESOURCE_DATA tSubResourceData = {};
		{
			tSubResourceData.pSysMem = m_pMaskData;
			tSubResourceData.SysMemPitch = tMaskTextureDesc.Width * sizeof(_ulong);
		}

		if (FAILED(m_pDevice->CreateTexture2D(&tMaskTextureDesc, &tSubResourceData, &m_pMaskTexture)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pMaskTexture, nullptr, &m_pMaskSRV)))
			return E_FAIL;
	}

	else 
	{
		/* 마스크 이미지 읽기 */
		_tchar szTextureFilePath[MAX_PATH] = TEXT("");
		wsprintf(szTextureFilePath, strDataFilePath.c_str());

		ID3D11ShaderResourceView* pSrcSRV = nullptr;
		ID3D11Resource* pSrcResource = nullptr;

		if (FAILED(DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, &pSrcResource, &pSrcSRV)))
			return E_FAIL;

		ID3D11Texture2D* pSrcTexture = static_cast<ID3D11Texture2D*>(pSrcResource);

		/* 읽어온 마스크 이미지를 복사하기 위한 텍스쳐 */
		D3D11_TEXTURE2D_DESC tDstTextureDesc = {};
		pSrcTexture->GetDesc(&tDstTextureDesc);
		{
			tDstTextureDesc.Usage = D3D11_USAGE_STAGING;
			tDstTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			tDstTextureDesc.BindFlags = 0;
		}

		ID3D11Texture2D* pDstTexture = nullptr;

		if (FAILED(m_pDevice->CreateTexture2D(&tDstTextureDesc, nullptr, &pDstTexture)))
			return E_FAIL;

		/* 텍스쳐 복사 */
		m_pContext->CopyResource(pDstTexture, pSrcTexture);

		/* 툴에서 사용할 형태로 변환하는 작업 */
		D3D11_TEXTURE2D_DESC tMaskTextureDesc = {};
		{
			tMaskTextureDesc.Width = m_pVIBufferCom->Get_NumVerticesX() - 1;
			tMaskTextureDesc.Height = m_pVIBufferCom->Get_NumVerticesZ() - 1;
			tMaskTextureDesc.MipLevels = 1;
			tMaskTextureDesc.ArraySize = 1;
			tMaskTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			tMaskTextureDesc.SampleDesc.Quality = 0;
			tMaskTextureDesc.SampleDesc.Count = 1;

			tMaskTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
			tMaskTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tMaskTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			tMaskTextureDesc.MiscFlags = 0;
		}

		m_pMaskData = new _ulong[tMaskTextureDesc.Width * tMaskTextureDesc.Height];
		ZeroMemory(m_pMaskData, sizeof(_ulong) * tMaskTextureDesc.Width * tMaskTextureDesc.Height);

		D3D11_MAPPED_SUBRESOURCE tMaskMSR;
		ZeroMemory(&tMaskMSR, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(pDstTexture, 0, D3D11_MAP_READ, 0, &tMaskMSR);

		_ulong* pData = (_ulong*)tMaskMSR.pData;
		memcpy(m_pMaskData, pData, sizeof(_ulong) * tMaskTextureDesc.Width * tMaskTextureDesc.Height);

		m_pContext->Unmap(pDstTexture, 0);

		D3D11_SUBRESOURCE_DATA tSubResourceData = {};
		{
			tSubResourceData.pSysMem = m_pMaskData;
			tSubResourceData.SysMemPitch = tMaskTextureDesc.Width * sizeof(_ulong);
		}

		if (FAILED(m_pDevice->CreateTexture2D(&tMaskTextureDesc, &tSubResourceData, &m_pMaskTexture)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pMaskTexture, nullptr, &m_pMaskSRV)))
			return E_FAIL;

		Safe_Release(pSrcSRV);
		Safe_Release(pSrcResource);
		Safe_Release(pDstTexture);
	}

	m_bMask = true;

	return S_OK;
}

HRESULT CTerrain::Save_Mask(const wstring& strDataFilePath)
{
	if (true == strDataFilePath.empty())
		return E_FAIL;
	
	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, m_pMaskTexture, strDataFilePath.c_str())))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Brushing_OnTerrain(_float& fBrushForce, _float3& vBrushPos, _float& fBrushSize, _float& fStrength, _float& fMaxHeight)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	return m_pVIBufferCom->Brushing_OnTerrain(fBrushForce, vBrushPos, fBrushSize, fStrength, fMaxHeight);
}

HRESULT CTerrain::Masking_OnTerrain(_float3& vBrushPos, _float& fBrushSize, MASK_TYPE eType)
{
	D3D11_TEXTURE2D_DESC tMaskDesc = {};

	m_pMaskTexture->GetDesc(&tMaskDesc);

	_int iLeft = max(0, (_int)vBrushPos.x - fBrushSize);
	_int iTop = max(0, (_int)vBrushPos.z - fBrushSize);
	_int iRight = min(tMaskDesc.Width, (_int)vBrushPos.x + fBrushSize);
	_int iBottom = min(tMaskDesc.Height, (_int)vBrushPos.z + fBrushSize);

	_ulong iColor = 0;

	if (MASK_GRASS == eType)
		iColor = D3DCOLOR_ABGR(255, 0, 0, 255);

	else if (MASK_ROAD == eType)
		iColor = D3DCOLOR_ABGR(255, 0, 255, 0);

	else if (MASK_STONE == eType)
		iColor = D3DCOLOR_ABGR(255, 255, 0, 0);

	for (_int i = iTop; i < iBottom; i++)
	{
		for (_int j = iLeft; j < iRight; j++) 
		{
			float fDistX = j - vBrushPos.x;
			float fDistZ = i - vBrushPos.z;
			float fDistance = sqrt(fDistX * fDistX + fDistZ * fDistZ);

			if (fDistance > fBrushSize)
				continue;

			_uint iIndex = i * tMaskDesc.Width + j;
			m_pMaskData[iIndex] = iColor;
		}
	}

	D3D11_MAPPED_SUBRESOURCE tMaskResource;
	ZeroMemory(&tMaskResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pMaskTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMaskResource);

	_ulong* pData = (_ulong*)tMaskResource.pData;
	memcpy(pData, m_pMaskData, sizeof(_ulong) * tMaskDesc.Width * tMaskDesc.Height);

	m_pContext->Unmap(m_pMaskTexture, 0);

	return S_OK;
}

HRESULT CTerrain::Get_TerrainSize(_float2* pTerrainSize)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	pTerrainSize->x = m_pVIBufferCom->Get_NumVerticesX();
	pTerrainSize->y = m_pVIBufferCom->Get_NumVerticesZ();

	return S_OK;
}

HRESULT CTerrain::Activate_Brush(_bool bValue)
{
	if (nullptr == m_pBrush)
		return E_FAIL;

	m_pBrush->Set_State(bValue);

	return S_OK;
}

HRESULT CTerrain::Resize_Brush(_float fScale)
{
	if (nullptr == m_pBrush)
		return E_FAIL;

	m_pBrush->Resize_Brush(fScale);

	return S_OK;
}

void CTerrain::Set_BrushPos(_float4 vBrushPos)
{
	if (nullptr == m_pBrush)
		return;

	m_pBrush->Invalidate_Brush(&vBrushPos);
}

HRESULT CTerrain::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	m_pVIBufferCom = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 1025, 1025);
	m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);
	Safe_AddRef(m_pVIBufferCom);

	/* For.Com_NaviMesh */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_NaviMesh"),
		TEXT("Com_NaviMesh"), (CComponent**)&m_pNaviCom)))
		return E_FAIL;

	/* For.Com_Grass_Diffuse_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Grass_Diffuse"),
		TEXT("Com_Grass_Diffuse_Texture"), (CComponent**)&m_pTextureCom[GRASS_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Grass_Normal_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Grass_Normal"),
		TEXT("Com_Grass_Normal_Texture"), (CComponent**)&m_pTextureCom[GRASS_NORMAL])))
		return E_FAIL;

	/* For.Com_Road_Diffuse_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Road_Diffuse"),
		TEXT("Com_Road_Diffuse_Texture"), (CComponent**)&m_pTextureCom[ROAD_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Road_Normal_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Road_Normal"),
		TEXT("Com_Road_Normal_Texture"), (CComponent**)&m_pTextureCom[ROAD_NORMAL])))
		return E_FAIL;

	/* For.Com_Stone_Diffuse_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Stone_Diffuse"),
		TEXT("Com_Stone_Diffuse_Texture"), (CComponent**)&m_pTextureCom[STONE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Stone_Normal_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Stone_Normal"),
		TEXT("Com_Stone_Normal_Texture"), (CComponent**)&m_pTextureCom[STONE_NORMAL])))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[GRASS_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_GrassDiffuse")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[GRASS_NORMAL]->Bind_ShaderResource(m_pShaderCom, "g_GrassNormal")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[ROAD_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_RoadDiffuse")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[ROAD_NORMAL]->Bind_ShaderResource(m_pShaderCom, "g_RoadNormal")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[STONE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_StoneDiffuse")))
		return E_FAIL;

	if (FAILED(m_pTextureCom[STONE_NORMAL]->Bind_ShaderResource(m_pShaderCom, "g_StoneNormal")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ShaderResourceView("g_MaskTexture", m_pMaskSRV)))
		return E_FAIL;

	return S_OK;
}

_float CTerrain::Compute_MaskInfluence(_float3& vBrushPoint, _float3& vBrushPos, _float& fBrushSize)
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vBrushPos) - XMLoadFloat3(&vBrushPoint)));
	_float finfluence = max(0.0f, 1.0f - fDistance / fBrushSize);

	return finfluence;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pNaviCom);

	for (_int i = 0; i < TEX_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pBrush);
	Safe_Release(m_pMaskSRV);
	Safe_Release(m_pMaskTexture);

	if (nullptr != m_pMaskData)
		Safe_Delete_Array(m_pMaskData);
}
