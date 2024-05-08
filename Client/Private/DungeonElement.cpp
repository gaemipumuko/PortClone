#include "stdafx.h"
#include "DungeonElement.h"
#include "Transform.h"
#include "Shader.h"
#include "Model.h"
#include "GameInstance.h"
#include "Camera.h"

CDungeonElement::CDungeonElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CDungeonElement::Initialize(ELEMENT_DESC* pElementDesc)
{
	if (nullptr == pElementDesc)
		return E_FAIL;

	m_eLevel = pElementDesc->eLevel;
	m_eType = pElementDesc->eType;
	m_vWorldMatrix = pElementDesc->vWorldMatrix;
	m_strModelTag = pElementDesc->strModelTag;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonElement::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CDungeonElement::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_eLevel == LEVEL_STORMTERROR)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2")))
			return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pShaderCom->Begin(m_ePassID)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDungeonElement::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(VTXMESH_SHADOW)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDungeonElement::Set_PassID(PASSID_VTXMESH ePassID)
{
	if (0 > ePassID)
		return E_FAIL;

	m_ePassID = ePassID;

	return S_OK;
}

PASSID_VTXMESH CDungeonElement::Get_PassID()
{
	return m_ePassID;
}

HRESULT CDungeonElement::Add_Components()
{
	m_pTextureCom = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GrayFireTexture"));

	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(m_vWorldMatrix);

	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"));

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	m_pModelCom = (CModel*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, m_strModelTag);

	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(m_pModelCom->Cooking_StaticModel(m_pTransformCom->Get_WorldFloat4x4(), "DungeonElement")))
		return E_FAIL;

	//wcout << m_strModelTag << endl;

	return S_OK;
}

HRESULT CDungeonElement::Bind_ShaderResources()
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

	return S_OK;
}

CDungeonElement* CDungeonElement::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ELEMENT_DESC* pElementDesc)
{
	CDungeonElement* pInstance = new CDungeonElement(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pElementDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CDungeonElement"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonElement::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}