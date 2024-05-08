#include "stdafx.h"
#include "SpawnBox.h"

#include "Creature.h"
#include "GameInstance.h"

CSpawnBox::CSpawnBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEventCollider(pDevice, pContext)
{
}

CSpawnBox::CSpawnBox(const CSpawnBox& rhs)
	: CEventCollider(rhs)
{
}

HRESULT CSpawnBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpawnBox::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	SPAWNER_DESC* pSpawnerDesc = (SPAWNER_DESC*)pArg;

	m_iLevel = pSpawnerDesc->iLevel;
	m_vSpawnerPos = pSpawnerDesc->vSpawnerPos;
	m_fSpawnRange = pSpawnerDesc->fSpawnRange;

	for (_int i = 0; i < pSpawnerDesc->pMonsterInfo->size(); i++)
	{
		MONSTER_INFO tMonsterInfo = {};
		{
			tMonsterInfo.eCreatureId = (*pSpawnerDesc->pMonsterInfo)[i].eCreatureId;
			tMonsterInfo.vSpawnPosition = (*pSpawnerDesc->pMonsterInfo)[i].vSpawnPosition;
		}

		m_MonsterInfo.push_back(tMonsterInfo);
	}


	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawnBox::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSpawnBox::Tick(_float fTimeDelta)
{
	vector<CGameObject*> isColl = m_pGameInstance->Find_Collision_Objects(COLL_SPAWN_CREATURE, m_iColliderID, COLL_EVENTREACT_PLAYER);

	if (isColl.empty() == FALSE)
	{
		for (_int i = 0; i < m_MonsterInfo.size(); i++) 
		{
			MONSTER_INFO tMonsterInfo = m_MonsterInfo[i];

			switch (tMonsterInfo.eCreatureId)
			{
			case MONSTER_SLIME_WATER:
			{
				_float3 vIntPos = _float3(tMonsterInfo.vSpawnPosition.x, tMonsterInfo.vSpawnPosition.y, tMonsterInfo.vSpawnPosition.z);

				OBJECT_DESC	ObjDesc = {};
				ObjDesc.vInitPos = XMLoadFloat3(&vIntPos);
				ObjDesc.vInitPos.m128_f32[3] = 1.f;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_iLevel, TEXT("Prototype_GameObject_Slime_Small"), CSpawnBox::Get_SpawnerTag(), &ObjDesc)))
					return E_FAIL;
			}
				break;
			case MONSTER_SLIME_ICE:
			{
				_float3 vIntPos = _float3(tMonsterInfo.vSpawnPosition.x, tMonsterInfo.vSpawnPosition.y, tMonsterInfo.vSpawnPosition.z);

				OBJECT_DESC	ObjDesc = {};
				ObjDesc.vInitPos = XMLoadFloat3(&vIntPos);
				ObjDesc.vInitPos.m128_f32[3] = 1.f;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_iLevel, TEXT("Prototype_GameObject_Slime_Big"), CSpawnBox::Get_SpawnerTag(), &ObjDesc)))
					return E_FAIL;
			}
				break;
			case MONSTER_HILI_NORMAL:
			{
				_float3 vIntPos = _float3(tMonsterInfo.vSpawnPosition.x, tMonsterInfo.vSpawnPosition.y, tMonsterInfo.vSpawnPosition.z);

				OBJECT_DESC	ObjDesc = {};
				ObjDesc.vInitPos = XMLoadFloat3(&vIntPos);
				ObjDesc.vInitPos.m128_f32[3] = 1.f;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_iLevel, TEXT("Prototype_GameObject_Hili_Normal"), CSpawnBox::Get_SpawnerTag(), &ObjDesc)))
					return E_FAIL;
			}
				break;
			case MONSTER_BRUTE_AXE:
			{
				_float3 vIntPos = _float3(tMonsterInfo.vSpawnPosition.x, tMonsterInfo.vSpawnPosition.y, tMonsterInfo.vSpawnPosition.z);

				OBJECT_DESC	ObjDesc = {};
				ObjDesc.iObjectType = tMonsterInfo.eCreatureId;
				ObjDesc.vInitPos = XMLoadFloat3(&vIntPos);
				ObjDesc.vInitPos.m128_f32[3] = 1.f;
				
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_iLevel, TEXT("Prototype_GameObject_Brute"), CSpawnBox::Get_SpawnerTag(), &ObjDesc)))
					return E_FAIL;
			}
				break;
			case MONSTER_ABYSS_MAGE_ELEC:
			{
				_float3 vIntPos = _float3(tMonsterInfo.vSpawnPosition.x, tMonsterInfo.vSpawnPosition.y, tMonsterInfo.vSpawnPosition.z);

				OBJECT_DESC	ObjDesc = {};
				ObjDesc.vInitPos = XMLoadFloat3(&vIntPos);
				ObjDesc.vInitPos.m128_f32[3] = 1.f;
				ObjDesc.iObjectType = MONSTER_ABYSS_MAGE_ELEC;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_iLevel, TEXT("Prototype_GameObject_Abyss_Mage"), L"Layer_Abyss_Mage", &ObjDesc)))
					return E_FAIL;
			}
			break;
			default:
				break;
			}
		}


		m_pGameInstance->Set_IsCollision(COLL_SPAWN_CREATURE, m_iColliderID, FALSE);
		m_pGameInstance->Release_Collider_Object(COLL_SPAWN_CREATURE, m_iColliderID);
		Set_Destroy(TRUE);

		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
			m_pGameInstance->Set_InteractionCount(L"BattleBGM", m_MonsterInfo.size());
	}

	return S_OK;
}

HRESULT CSpawnBox::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

wstring CSpawnBox::Get_SpawnerTag()
{
	wstring strUniqueLayerID = TEXT("Layer_SpawnBox_Monster_");
	strUniqueLayerID += to_wstring(m_iObjectID);

	return strUniqueLayerID;
}

HRESULT CSpawnBox::Render()
{
	return S_OK;
}

HRESULT CSpawnBox::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vSpawnerPos));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBounding_Desc = {};
	{
		tBounding_Desc.fRadius = m_fSpawnRange * 0.5f;
		tBounding_Desc.vCenter = _float3(0.0f, 0.0f, 0.0f);
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
		return E_FAIL;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_Collider_Object(COLL_SPAWN_CREATURE, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

CSpawnBox* CSpawnBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpawnBox* pInstance = new CSpawnBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSpawnBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpawnBox::Clone(void* pArg)
{
	CSpawnBox* pInstance = new CSpawnBox(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSpawnBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpawnBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
