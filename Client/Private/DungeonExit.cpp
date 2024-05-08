#include "stdafx.h"
#include "DungeonExit.h"

#include "Player.h"
#include "Level_Loading.h"
#include "InteractionBox.h"

#include "GameInstance.h"

CDungeonExit::CDungeonExit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDungeonExit::CDungeonExit(const CDungeonExit& rhs)
	: CGameObject(rhs)
{
}

_bool CDungeonExit::isInPlayer()
{
	_bool bisColl = false;
	CGameObject* pGameObject = m_pGameInstance->Find_Object(m_eLevel, TEXT("Layer_Player"));

	if (nullptr != pGameObject)
	{
		CPlayer* pPlayer = (CPlayer*)pGameObject;

		_int iColliderID = pPlayer->Get_BodyCollisionID();

		vector<CGameObject*> CollisionObjects = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);

		if (false == CollisionObjects.empty())
		{
			CCollider* pCollider = (CCollider*)CollisionObjects.front()->Find_Component(TEXT("Com_Collider_OBB"));

			if (nullptr != pCollider)
			{
				bisColl = m_pColliderCom->Intersect(pCollider);
				return bisColl;
			}
		}
	}

	return bisColl;
}

HRESULT CDungeonExit::Add_Component(void* pArg)
{
	DUNGEON_EXIT* pDungeonExit = static_cast<DUNGEON_EXIT*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(pDungeonExit->eLevel, TEXT("Prototype_Component_Model_Dungeon_Exit"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(pDungeonExit->eLevel, TEXT("Prototype_Component_Texture_Dungeon_Exit"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 1.5f;
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonExit::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonExit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDungeonExit::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	DUNGEON_EXIT* pDungeonExitDesc = static_cast<DUNGEON_EXIT*>(pArg);

	m_eLevel = pDungeonExitDesc->eLevel;
	m_vReservePosition = pDungeonExitDesc->vReservePosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDungeonExitDesc->vExitPosition));

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	/* InteractionBox */
	m_pInteractionBox = static_cast<CInteraction_Box*>(m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr));
	m_pInteractionBox->Add_InteractionBox(L"비경 나가기", CInteraction_Box::ICON_TALK);

	return S_OK;
}

HRESULT CDungeonExit::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CDungeonExit::Tick(_float fTimeDelta)
{
	if ((CDungeonExit::isInPlayer() == TRUE) && (m_bReserveLevel == FALSE))
	{
		m_pInteractionBox->Tick(fTimeDelta);
	
		if (m_pGameInstance->Key_Down('F') && m_pInteractionBox->Get_SelectedIndex() == 0)
		{
			m_bReserveLevel = TRUE;
		}
	}

	if (m_bReserveLevel == TRUE)
	{
		m_bReserveLevel = FALSE;

		CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_WORLD);
		if (pLevel == nullptr)
			return E_FAIL;

		m_pGameInstance->Set_ReservePosition(m_vReservePosition);
		if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDungeonExit::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	if ((CDungeonExit::isInPlayer() == TRUE) && (m_bReserveLevel == FALSE))
	{
		m_pInteractionBox->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CDungeonExit::Render()
{
	if (FAILED(CDungeonExit::Bind_ShaderResources()))
		return E_FAIL;

	// MagicLine
	if (FAILED(m_pShaderCom->Begin(MESHFX_DUNGEONEXIT_MAGICLINE)))
		return E_FAIL;
	
	if (FAILED(m_pModelCom->Render(static_cast<_uint>(0))))
		return E_FAIL;
	
	// Cylinder
	if (FAILED(m_pShaderCom->Begin(MESHFX_DUNGEONEXIT_CYLINDER)))
		return E_FAIL;
	
	if (FAILED(m_pModelCom->Render(static_cast<_uint>(1))))
		return E_FAIL;
	
	// Ring
	if (FAILED(m_pShaderCom->Begin(MESHFX_DUNGEONEXIT_RING)))
		return E_FAIL;
	
	if (FAILED(m_pModelCom->Render(static_cast<_uint>(2))))
		return E_FAIL;

	return S_OK;
}

CDungeonExit* CDungeonExit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDungeonExit* pInstance = new CDungeonExit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDungeonExit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDungeonExit::Clone(void* pArg)
{
	CDungeonExit* pInstance = new CDungeonExit(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDungeonExit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonExit::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pInteractionBox);
}
