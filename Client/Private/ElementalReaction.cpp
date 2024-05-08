#include "stdafx.h"
#include "ElementalReaction.h"
#include "Part_Body.h"
#include "GameInstance.h"
#include "Player.h"
#include "ChainLightning.h"

CElementalReaction::CElementalReaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CElementalReaction::CElementalReaction(const CElementalReaction& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CElementalReaction::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;*/


	return S_OK;
}

HRESULT CElementalReaction::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CElementalReaction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElementalReaction::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CElementalReaction::Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
		m_eReaction = pDesc->eReaction;
		m_eElement = pDesc->eElement;
		m_eCollider = pDesc->eCollider;
		m_fDamage = pDesc->fDamage;
		XMStoreFloat4(&m_vColor, pDesc->vColor);

		if (m_eCollider != COLL_PLAYER_SKILL && m_eCollider != COLL_MONSTER_SKILL)
			return E_FAIL;

		/* For.Com_Collider_SPHERE */
		CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};

		if(m_eReaction == REACT_ELECTROCHARGED)
			tBoundingDesc.fRadius = 7.f;
		else if(m_eReaction >= REACT_SWIRL_FIRE && m_eReaction <= REACT_SWIRL_FROST)
			tBoundingDesc.fRadius = 4.f;

		tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
			TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CElementalReaction::PreviousTick(_float fTimeDelta)
{
	if (Get_Destroy() == TRUE)
		return S_OK;

	return S_OK;
}

HRESULT CElementalReaction::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > 2.f)
	{
		Set_Destroy(TRUE);
		return S_OK;
	}
		

	if (m_bAttack == FALSE)
	{
		if (m_eReaction >= REACT_SWIRL_FIRE && m_eReaction <= REACT_SWIRL_FROST) /* 확산 */
		{
			if (m_eCollider == COLL_PLAYER_SKILL)
			{
				vector<CGameObject*> HitMonsters = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_MONSTER_BODY);
				if (HitMonsters.empty() == FALSE)
				{
					for (auto& Hit : HitMonsters)
					{
						static_cast<CPartObject*>(Hit)->Set_Damage(m_pGameInstance->Get_Float_Random(0.5f, 0.8f) * m_fDamage, m_eElement);
					}
				}
				m_bAttack = TRUE;
				//Set_Destroy(TRUE);
			}
			else if (m_eCollider = COLL_MONSTER_SKILL)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
				vector<CGameObject*> HitPlayers = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_PLAYER_BODY);
				if (pPlayer->Get_CombatMode() == FALSE)
				{
					if (HitPlayers.empty() == FALSE)
					{
						pPlayer->Set_Damage(m_pGameInstance->Get_Float_Random(0.5f, 0.8f) * m_fDamage, m_eElement);
					}
				}
				else
				{
					for (auto& pPlayerBody : HitPlayers)
					{
						PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
						pPlayer->Set_AutoDamage(eIndex, m_pGameInstance->Get_Float_Random(0.5f, 0.8f) * m_fDamage, m_eElement);
					}
				}
				m_bAttack = TRUE;
				//Set_Destroy(TRUE);

			}
		}
		else if (m_eReaction == REACT_ELECTROCHARGED) /* 감전 */
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			if (m_eCollider == COLL_PLAYER_SKILL)
			{
				vector<CGameObject*> HitMonsters = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_MONSTER_BODY);
				if (HitMonsters.empty() == FALSE)
				{
					for (auto& Hit : HitMonsters)
					{
						if (static_cast<CPartObject*>(Hit)->isHostElement(ELEMENT_WATER) == TRUE && static_cast<CPartObject*>(Hit)->isHostElement(ELEMENT_ELECTRIC) == FALSE)
						{
							CChainLightning::CHAIN_LIGHTNING_DESC tDesc = {};
							tDesc.vPointA = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
							tDesc.vPointA.m128_f32[1] += 0.5f;
							tDesc.vPointB = static_cast<CPart_Body*>(Hit)->Get_BodyWorldPosition();
							tDesc.vPointB.m128_f32[1] += 0.5f;

							if (tDesc.vPointA.m128_f32[0] == tDesc.vPointB.m128_f32[0] &&
								tDesc.vPointA.m128_f32[1] == tDesc.vPointB.m128_f32[1] &&
								tDesc.vPointA.m128_f32[2] == tDesc.vPointB.m128_f32[2]
								)
								continue;
							else
							{
								if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_ChainLightning", L"Layer_Effect", &tDesc)))
									return E_FAIL;
								static_cast<CPart_Body*>(Hit)->Set_Damage(m_pGameInstance->Get_Float_Random(0.6f, 0.8f) * m_fDamage, ELEMENT_ELECTRIC);
							}

							
						}
					}
				}
				m_bAttack = TRUE;
				//Set_Destroy(TRUE);
			}
			else
			{
				m_bAttack = TRUE;
				//Set_Destroy(TRUE);
			}
		}
	}

//#ifdef _DEBUG
//	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
//#endif

	return S_OK;
}

HRESULT CElementalReaction::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CElementalReaction::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CElementalReaction* CElementalReaction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElementalReaction* pInstance = new CElementalReaction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CElementalReaction"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElementalReaction::Clone(void* pArg)
{
	CElementalReaction* pInstance = new CElementalReaction(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CElementalReaction"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElementalReaction::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
}
