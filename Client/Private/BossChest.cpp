#include "stdafx.h"
#include "BossChest.h"

#include "Player.h"
#include "DungeonExit.h"
#include "Ground_Item.h"
#include "Level_Loading.h"
#include "InteractionBox.h"
#include "FX_EffigyFireExplode.h"

#include "GameInstance.h"

CBossChest::CBossChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBossChest::CBossChest(const CBossChest& rhs)
	: CGameObject(rhs)
{
}

_bool CBossChest::isInPlayer()
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

HRESULT CBossChest::Add_Component(void* pArg)
{
	BOSS_CHEST* pBossChest = static_cast<BOSS_CHEST*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boss_Chest"),
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

HRESULT CBossChest::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBossChest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossChest::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	BOSS_CHEST* pBossChest = static_cast<BOSS_CHEST*>(pArg);

	m_eLevel = pBossChest->eLevel;
	m_vReservedPosition = pBossChest->vReservePosition;

	m_pTransformCom->Set_Scale({ 1.5f, 1.5f, 1.5f, 0.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pBossChest->vPosition));
	m_pTransformCom->Rotations(
		vector<_vector>{ { 1.f, 0.f, 0.f, 0.f}, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 0.f } },
		vector<_float>{
			XMConvertToRadians(pBossChest->vRotation.x),
			XMConvertToRadians(pBossChest->vRotation.y),
			XMConvertToRadians(pBossChest->vRotation.z) });

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	/* InteractionBox */
	m_pInteractionBox = static_cast<CInteraction_Box*>(m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr));
	m_pInteractionBox->Add_InteractionBox(L"상자 열기", CInteraction_Box::ICON_TALK);

	/* Drop Table */
	for (auto& iter : pBossChest->DropList)
		m_DropList.push_back(iter);

	return S_OK;
}

HRESULT CBossChest::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CBossChest::Tick(_float fTimeDelta)
{
	if ((CBossChest::isInPlayer() == TRUE) && (m_bOpened == FALSE))
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

	if (m_bOpened == FALSE)
	{
		m_pModelCom->Set_Animation((m_bGrowned == FALSE) ? 0 : 1);

		if (m_bGrowned == FALSE)
		{
			_float fRate = m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
			if (fRate >= m_pModelCom->Get_AnimationDuration(0))
				m_bGrowned = TRUE;
		}
		else
			m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);
	}
	else
	{
		m_pModelCom->Set_Animation(2);
		_float fRate = m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
		if (fRate > m_pModelCom->Get_AnimationDuration(2) * 2.f)
		{
			if (m_vReservedPosition.w != 0.f)
			{
				CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
				tExplodeDesc.fExplodeExpand = 1.5f;
				tExplodeDesc.fScale = 2.5f;
				tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
				tExplodeDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

				CDungeonExit::DUNGEON_EXIT tDungeonExit = { };
				tDungeonExit.eLevel = m_eLevel;
				XMStoreFloat4(&tDungeonExit.vExitPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				tDungeonExit.vReservePosition = m_vReservedPosition;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_eLevel, TEXT("Prototype_GameObject_Dungeon_Exit"), L"Layer_Exit", &tDungeonExit)))
					return E_FAIL;
			}

			Set_Destroy(TRUE);
		}
	}

	return S_OK;
}

HRESULT CBossChest::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	if ((CBossChest::isInPlayer() == TRUE) && (m_bOpened == FALSE))
	{
		m_pInteractionBox->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CBossChest::Render()
{
	if (FAILED(CBossChest::Bind_ShaderResources()))
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

CBossChest* CBossChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossChest* pInstance = new CBossChest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBossChest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossChest::Clone(void* pArg)
{
	CBossChest* pInstance = new CBossChest(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBossChest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossChest::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pInteractionBox);
}
