#include "stdafx.h"
#include "WorldElement.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "Model.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Collider.h"
#include "Camera.h"

CWorldElement::CWorldElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CWorldElement::Initialize(ELEMENT_DESC* pElementDesc)
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

HRESULT CWorldElement::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CWorldElement::Render()
{
	if (true == m_bShow) 
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pShaderCom->Begin(VTXMESH_TERRAIN)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CWorldElement::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
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
	
	//if (true == m_bShow)
	//{
	//	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
	//		return E_FAIL;

	//	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	//	{
	//		if (FAILED(m_pShaderCom->Begin(VTXMESH_SHADOW)))
	//			return E_FAIL;

	//		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
	//			return E_FAIL;
	//	}
	//}

	//else 
	//{
	//	CGameObject* pPlayer = m_pGameInstance->Find_Object(LEVEL_WORLD, TEXT("Layer_Player"));

	//	if (nullptr != pPlayer) 
	//	{
	//		CTransform* pTransform = (CTransform*)pPlayer->Find_Component(TEXT("Com_Transform"));

	//		if (nullptr != pTransform) 
	//		{
	//			_float4 vPlayerPos;
	//			XMStoreFloat4(&vPlayerPos, pTransform->Get_State(CTransform::STATE_POSITION));

	//			_float4 vPosition;
	//			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//			_float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPosition) - XMLoadFloat4(&vPlayerPos)));

	//			if (fDistance < 50.0f) 
	//			{
	//				if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
	//					return E_FAIL;

	//				if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
	//					return E_FAIL;

	//				_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//				for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	//				{
	//					if (FAILED(m_pShaderCom->Begin(VTXMESH_SHADOW)))
	//						return E_FAIL;

	//					if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
	//						return E_FAIL;
	//				}
	//			}
	//		}
	//	}
	//}

	return S_OK;
}

_float4 CWorldElement::Get_Position()
{
	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vPosition;
}

HRESULT CWorldElement::Add_Components()
{
	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));

	m_pTransformCom->Set_WorldFloat4x4(m_vWorldMatrix);

	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"));
	m_pModelCom = (CModel*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, m_strModelTag);

	_float4x4 vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	m_pModelCom->Cooking_StaticModel(vWorldMatrix, "WorldElement");
	wcout << m_strModelTag << endl;

	_float3 vMin = _float3(9999999.0f, 9999999.0f, 9999999.0f);
	_float3 vMax = _float3(-9999999.0f, -9999999.0f, -9999999.0f);
	
	if (FAILED(m_pModelCom->Compute_Extents(&vMin, &vMax)))
		return E_FAIL;

	_float3 vExtents = _float3((vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f);
	_float3 vCenter = _float3((vMax.x + vMin.x) * 0.5f, (vMax.y + vMin.y) * 0.5f, (vMax.z + vMin.z) * 0.5f);

	CBounding_OBB::BOUNDING_OBB_DESC tBounding_Desc = {};
	{
		tBounding_Desc.vExtents = vExtents;
		tBounding_Desc.vCenter = vCenter;
		tBounding_Desc.vRadians = _float3(0.0f, 0.0f, 0.0f);
	}

	m_pColliderCom = (CCollider*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), &tBounding_Desc);
	m_pColliderCom->Update(XMLoadFloat4x4(&m_vWorldMatrix));

	return S_OK;
}

HRESULT CWorldElement::Bind_ShaderResources()
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

CWorldElement* CWorldElement::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ELEMENT_DESC* pElementDesc)
{
	CWorldElement* pInstance = new CWorldElement(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pElementDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CWorldElement"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorldElement::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
