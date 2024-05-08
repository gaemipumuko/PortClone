#include "stdafx.h"
#include "DungeonChest.h"

#include "Player.h"
#include "Ground_Item.h"
#include "Level_Loading.h"
#include "InteractionBox.h"

#include "GameInstance.h"

CDungeonChest::CDungeonChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDungeonChest::CDungeonChest(const CDungeonChest& rhs)
	: CGameObject(rhs)
{
}

_bool CDungeonChest::isInPlayer()
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

HRESULT CDungeonChest::Add_Component(void* pArg)
{
	DUNGEON_CHEST* pDungeonChest = static_cast<DUNGEON_CHEST*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(pDungeonChest->eLevel, TEXT("Prototype_Component_Model_Dungeon_Chest"),
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

	/* For.Com_Collider_OBB */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = { };
	tBoundingDesc.fRadius = 1.5f;
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonChest::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CDungeonChest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDungeonChest::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	DUNGEON_CHEST* pDungeonChest = static_cast<DUNGEON_CHEST*>(pArg);

	m_eLevel = pDungeonChest->eLevel;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDungeonChest->vPosition));
	m_pTransformCom->Rotations(
		vector<_vector>{ { 1.f, 0.f, 0.f, 0.f}, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 0.f } },
		vector<_float>{
			XMConvertToRadians(pDungeonChest->vRotation.x),
			XMConvertToRadians(pDungeonChest->vRotation.y),
			XMConvertToRadians(pDungeonChest->vRotation.z) });

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Set_Animation(0);
	m_pModelCom->Play_Animation(FALSE, m_pTransformCom, 0.00000001f);

	/* InteractionBox */
	m_pInteractionBox = static_cast<CInteraction_Box*>(m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr));
	m_pInteractionBox->Add_InteractionBox(L"상자 열기", CInteraction_Box::ICON_TALK);

	/* Drop Table */
	for (auto& iter : pDungeonChest->DropList)
		m_DropList.push_back(iter);

	return S_OK;
}

HRESULT CDungeonChest::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CDungeonChest::Tick(_float fTimeDelta)
{
	if ((CDungeonChest::isInPlayer() == TRUE) && (m_bOpened == FALSE))
	{
		m_pInteractionBox->Tick(fTimeDelta);
	
		if (m_pGameInstance->Key_Down('F'))
		{
			m_bOpened = TRUE;

			CGround_Item::DROP_ITEM_DESC	titemdesc = { };
			for (auto& iter : m_DropList)
			{
				titemdesc.iItemIndex	= iter.first;
				titemdesc.iItemQuantity = iter.second;
				titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
					return E_FAIL;
			}
			m_pGameInstance->PlaySoundW(TEXT("Open.mp3"), 10.f);
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
			_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
			if (iIndex == PLAYER_ONE)
			{
				wstring soundFileName = L"VO_KO_Diluc_Opening_Treasure_Chest_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (iIndex == PLAYER_TWO)
			{
				wstring soundFileName = L"VO_KO_Zhongli_Opening_Treasure_Chest_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (iIndex == PLAYER_THREE)
			{
				wstring soundFileName = L"VO_KO_Barbara_Opening_Treasure_Chest_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (iIndex == PLAYER_FOUR)
			{
				wstring soundFileName = L"VO_KO_Raiden_Shogun_Opening_Treasure_Chest_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
		}
	}

	if (m_bOpened == TRUE)
		m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);

	return S_OK;
}

HRESULT CDungeonChest::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	if ((CDungeonChest::isInPlayer() == TRUE) && (m_bOpened == FALSE))
	{
		m_pInteractionBox->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CDungeonChest::Render()
{
	if (FAILED(CDungeonChest::Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DUNGEONOBJECT)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CDungeonChest* CDungeonChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDungeonChest* pInstance = new CDungeonChest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDungeonChest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDungeonChest::Clone(void* pArg)
{
	CDungeonChest* pInstance = new CDungeonChest(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDungeonChest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeonChest::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pInteractionBox);
}
