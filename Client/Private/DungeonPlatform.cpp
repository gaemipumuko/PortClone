#include "stdafx.h"
#include "DungeonPlatform.h"

#include "GameInstance.h"
#include "Player.h"
#include "CharacterController.h"

CDungeonPlatform::CDungeonPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CDungeonPlatform::CDungeonPlatform(const CDungeonPlatform& rhs)
    : CGameObject(rhs)
{
}

HRESULT CDungeonPlatform::Add_Component(void* pArg)
{
	DPLATFORM_DESC* pDungeonPlatformDesc = static_cast<DPLATFORM_DESC*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(pDungeonPlatformDesc->ePlatformLevel, TEXT("Prototype_Component_Model_Dungeon_Platform"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Collider */
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

	/* For.Com_Collider */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
		return E_FAIL;

    return S_OK;
}

HRESULT CDungeonPlatform::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

    return S_OK;
}

_bool CDungeonPlatform::isInPlatform(CGameObject** ppGameObject)
{
	_bool bisColl = false;

	vector<CGameObject*> pCollisions = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);

	if (false == pCollisions.empty()) 
	{
		bisColl = true;
		*ppGameObject = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	}

	return bisColl;
}

HRESULT CDungeonPlatform::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDungeonPlatform::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	DPLATFORM_DESC* pDungeonPlatformDesc = static_cast<DPLATFORM_DESC*>(pArg);

	m_pTransformCom->Set_Scale(XMLoadFloat4(&pDungeonPlatformDesc->eTransformDesc.vScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDungeonPlatformDesc->eTransformDesc.vTranslation));
	m_pTransformCom->Rotations(
		vector<_vector>{ { 1.f, 0.f, 0.f, 0.f}, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 0.f } },
		vector<_float>{ 
			XMConvertToRadians(pDungeonPlatformDesc->eTransformDesc.vRotation.x), 
			XMConvertToRadians(pDungeonPlatformDesc->eTransformDesc.vRotation.y), 
			XMConvertToRadians(pDungeonPlatformDesc->eTransformDesc.vRotation.z) });
	
	m_vLiftTranslation = pDungeonPlatformDesc->vLiftTranslation;

	_float4x4 vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(m_pModelCom->Cooking_StaticModel(vWorldMatrix, "None", (PxRigidStatic**)(&m_pRigidStatic))))
		return E_FAIL;

    return S_OK;
}

HRESULT CDungeonPlatform::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CDungeonPlatform::Tick(_float fTimeDelta)
{
	if (m_bReacted == TRUE)
	{
		if (abs(m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] - m_vLiftTranslation.y) > 0.1f)
		{
			m_pTransformCom->Move_To(XMLoadFloat4(&m_vLiftTranslation), fTimeDelta * 0.5f);

			if (XMVectorGetX(XMVector4Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&m_vLiftTranslation))) < 0.1f) 
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vLiftTranslation));

			_float4 vPosition;
			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			PxVec3 PxPosition = PxVec3(vPosition.x, vPosition.y, vPosition.z);
			PxTransform PxTranslation = PxTransform(PxPosition);
			m_pRigidStatic->setGlobalPose(PxTranslation);
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

    return S_OK;
}

HRESULT CDungeonPlatform::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CDungeonPlatform::Render()
{
	if (FAILED(CDungeonPlatform::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_DUNGEONPLATFORM)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

    return S_OK;
}

CDungeonPlatform* CDungeonPlatform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDungeonPlatform* pInstance = new CDungeonPlatform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDungeonPlatform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDungeonPlatform::Clone(void* pArg)
{
	CDungeonPlatform* pInstance = new CDungeonPlatform(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDungeonPlatform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonPlatform::Free()
{
	__super::Free();

	PX_RELEASE(m_pRigidStatic);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
}
