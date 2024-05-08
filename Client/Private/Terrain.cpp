#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Terrain.h"
#include "Texture.h"
#include "Navigation.h"
#include "Camera.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTerrain::Initialize()
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(TERRAIN_DESC* pTerrainDesc)
{
	if (nullptr == pTerrainDesc)
		return E_FAIL;

	//m_fTerrainX = pTerrainDesc->iRow * m_iTerrainWidth + m_iTerrainWidth;
	//m_fTerrainY = pTerrainDesc->iColl * m_iTerrainWidth + m_iTerrainWidth;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Initialize_QuadTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Update()
{
	return S_OK;
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXNORTEX_TERRAIN)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Invalidate_Culling()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pVIBufferCom->Culling_QuadTree(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));
	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"));
	
	m_pVIBufferCom = (CVIBuffer_Terrain*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_World"));
	m_pVIBufferCom->Initialize_QuadTree();

	m_pTextureCom[GRASS_DIFFUSE] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_Diffuse"));
	m_pTextureCom[GRASS_NORMAL] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_Normal"));
	
	m_pTextureCom[ROAD_DIFFUSE] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Road_Diffuse"));
	m_pTextureCom[ROAD_NORMAL] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Road_Normal"));
	
	m_pTextureCom[STONE_DIFFUSE] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stone_Diffuse"));
	m_pTextureCom[STONE_NORMAL] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stone_Normal"));
	
	m_pTextureCom[TEX_MASK] = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"));

	_uint iNumVertices = m_pVIBufferCom->Get_NumVertices();
	_uint iNumIndices = m_pVIBufferCom->Get_NumIndices();

	_float3* pVerticesPos = m_pVIBufferCom->Get_VerticesPos();
	_ulong* pIndices = m_pVIBufferCom->Get_Indices();
	
	_float4x4 vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(m_pGameInstance->Create_StaticMesh(iNumVertices, pVerticesPos, iNumIndices, pIndices, vWorldMatrix)))
		return E_FAIL;

	wcout << TEXT("Complete Cooking Terrain!") << endl;

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

	if (FAILED(m_pTextureCom[TEX_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TERRAIN_DESC* pTerrainDesc)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pTerrainDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	for (_int i = 0; i < TEX_END; i++)
		Safe_Release(m_pTextureCom[i]);
}