#include "stdafx.h"
#include "DungeonDoor.h"

#include "GameInstance.h"
#include "Camera_Main.h"

CDungeonDoor::CDungeonDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CDungeonDoor::CDungeonDoor(const CDungeonDoor& rhs)
    : CGameObject(rhs)
{
}

HRESULT CDungeonDoor::Add_Component(void* pArg)
{
	DDOOR_DESC* pDungeonDoorDesc = static_cast<DDOOR_DESC*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(pDungeonDoorDesc->eDoorLevel, TEXT("Prototype_Component_Model_Dungeon_Door"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
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

    return S_OK;
}

HRESULT CDungeonDoor::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

    return S_OK;
}

HRESULT CDungeonDoor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDungeonDoor::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	DDOOR_DESC* pDungeonDoorDesc = static_cast<DDOOR_DESC*>(pArg);

	m_pTransformCom->Set_Scale(XMLoadFloat4(&pDungeonDoorDesc->eTransformDesc.vScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDungeonDoorDesc->eTransformDesc.vTranslation));
	m_pTransformCom->Rotations(
		vector<_vector>{ { 1.f, 0.f, 0.f, 0.f}, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 0.f } },
		vector<_float>{ 
			XMConvertToRadians(pDungeonDoorDesc->eTransformDesc.vRotation.x), 
			XMConvertToRadians(pDungeonDoorDesc->eTransformDesc.vRotation.y), 
			XMConvertToRadians(pDungeonDoorDesc->eTransformDesc.vRotation.z) });

	// m_pModelCom->Cooking_StaticModel(m_pTransformCom->Get_WorldFloat4x4());

    return S_OK;
}

HRESULT CDungeonDoor::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CDungeonDoor::Tick(_float fTimeDelta)
{
	if (m_bReacted == FALSE)
	{
		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);
	}
	else
	{
		if (m_byStep == 0)
		{
			if (m_iCameraScene == 0)
			{
				CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
				if (pCamera != nullptr)
				{
					pCamera->Set_SceneMode(CAMSCENEID_DUNGEONDOOR, SCENE_ONE, 0.f, 1.f, TRUE, m_pTransformCom);
					m_iCameraScene++;
				}
			}
			m_pModelCom->Set_Animation(1);
			_float fRate = m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
			if (fRate > m_pModelCom->Get_AnimationDuration(1))
			{
				++m_byStep;
				if (m_iCameraScene == 1)
				{
					CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
					if (pCamera != nullptr)
					{
						pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
						m_iCameraScene++;
					}
				}
			}
		}
		if (m_byStep == 1)
		{
			m_pModelCom->Set_Animation(2);
			m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);
		}
	}

    return S_OK;
}

HRESULT CDungeonDoor::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CDungeonDoor::Render()
{
	if (FAILED(CDungeonDoor::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DUNGEONOBJECT)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

    return S_OK;
}

CDungeonDoor* CDungeonDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDungeonDoor* pInstance = new CDungeonDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDungeonDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDungeonDoor::Clone(void* pArg)
{
	CDungeonDoor* pInstance = new CDungeonDoor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDungeonDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonDoor::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
