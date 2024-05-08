#include "stdafx.h"
#include "DvalinPlatform.h"
#include "Transform.h"
#include "Dvalin.h"

#ifdef _DEBUG
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#endif

#include "GameInstance.h"

CDvalinPlatform::CDvalinPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CDvalinPlatform::Initialize()
{
	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pModelCom = (CModel*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DvalinPlatform"));

	if (nullptr == m_pModelCom)
		return E_FAIL;

	_float4x4 vWorldMatrix;
	XMStoreFloat4x4(&vWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	if (FAILED(m_pModelCom->Cooking_StaticModel(vWorldMatrix, "None", (PxRigidStatic**)&m_pRigidStatic)))
		return E_FAIL;

#ifdef _DEBUG
	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"));
	
	if (nullptr == m_pShaderCom)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CDvalinPlatform::Move_ToPlatform(_float4 vPosition, _float4 vDirection)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	m_pTransformCom->Look_At_Position(XMLoadFloat4(&vDirection));

	_float3 vRight, vUp, vLook;
	{
		XMStoreFloat3(&vRight, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		XMStoreFloat3(&vUp, m_pTransformCom->Get_State(CTransform::STATE_UP));
		XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	}

	PxVec3 PxRight = PxVec3(vRight.x, vRight.y, vRight.z);
	PxVec3 PxUp = PxVec3(vUp.x, vUp.y, vUp.z);
	PxVec3 PxLook = PxVec3(vLook.x, vLook.y, vLook.z);
	PxVec3 PxPosition = PxVec3(vPosition.x, vPosition.y, vPosition.z);

	PxMat33 PxRotateMatrix = PxMat33(PxRight.getNormalized(), PxUp.getNormalized(), PxLook.getNormalized());
	PxQuat PxRotate = PxQuat(PxRotateMatrix);

	PxTransform PxSRT = PxTransform(PxPosition, PxRotate);

	m_pRigidStatic->setGlobalPose(PxSRT);

	return S_OK;
}

HRESULT CDvalinPlatform::Look_Platform(_float4 vDirection)
{
	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Look_At_Position(XMLoadFloat4(&vDirection));

	_float3 vRight, vUp, vLook;
	{
		XMStoreFloat3(&vRight, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		XMStoreFloat3(&vUp, m_pTransformCom->Get_State(CTransform::STATE_UP));
		XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	}

	PxVec3 PxRight = PxVec3(vRight.x, vRight.y, vRight.z);
	PxVec3 PxUp = PxVec3(vUp.x, vUp.y, vUp.z);
	PxVec3 PxLook = PxVec3(vLook.x, vLook.y, vLook.z);
	PxVec3 PxPosition = PxVec3(vPosition.x, vPosition.y, vPosition.z);

	PxMat33 PxRotateMatrix = PxMat33(PxRight.getNormalized(), PxUp.getNormalized(), PxLook.getNormalized());
	PxQuat PxRotate = PxQuat(PxRotateMatrix);

	PxTransform PxSRT = PxTransform(PxPosition, PxRotate);

	m_pRigidStatic->setGlobalPose(PxSRT);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CDvalinPlatform::Render()
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

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

#endif

CDvalinPlatform* CDvalinPlatform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDvalinPlatform* pInstance = new CDvalinPlatform(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CDvalinPlatform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDvalinPlatform::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	PX_RELEASE(m_pRigidStatic);

#ifdef _DEBUG
	Safe_Release(m_pShaderCom);
#endif
}
