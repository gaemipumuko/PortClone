#include "stdafx.h"
#include "Slime_BIg.h"
#include "Part_Body.h"
#include "Slime_IceCone.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect.h"

//UI
#include "Hp_Bar.h"
#include "UI_Manager.h"
#include "Page_Main.h"

CSlime_Big::CSlime_Big(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CSlime_Big::CSlime_Big(const CSlime_Big& rhs)
	: CCreature(rhs)
{
}

HRESULT CSlime_Big::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = MONSTER_SLIME_ICE;
	tBodyDesc.pController = m_pController;
	tBodyDesc.eType = CTY_MONSTER;

	CREATUREID eCreatureID = MONSTER_SLIME_ICE;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	tSlots = { };
	/* For. Part_Weapon */
	CModel* pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot->Find_Component(TEXT("Com_Model")));

	CPart_Weapon::PARTWEAPON_DESC tWeaponDesc = { };
	tWeaponDesc.pBody = pBody;
	tWeaponDesc.tPartObjDesc.pParent = this;
	tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tWeaponDesc.eType = CTY_MONSTER;

	pPartObject = CPart_Weapon::Create_Weapon(WEAPON_ICECONE, tWeaponDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CSlime_Big::Pattern(_float fTimeDelta)
{
	_bool bPerification = m_Parts[PART_BODY].pPartySlot->Get_Perification();
	if (bPerification == TRUE)
		return S_OK;

	FSM_SLIME_STATE eState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	if (eState == FSM_SLIME_HIT_H || eState == FSM_SLIME_HIT_L || eState == FSM_SLIME_DIE1 || eState == FSM_SLIME_DIE2
		|| eState == FSM_SLIME_HIT_THROW_START || eState == FSM_SLIME_HIT_THROW || eState == FSM_SLIME_HIT_THROW_DOWN)
		return S_OK;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vPlayerPos; _vector vSetDir;
	_float fDistance;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 1명만 온필드 */
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		vSetDir = vPlayerPos - vPos;
		fDistance = XMVector3Length(vSetDir).m128_f32[0];
		vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);
	}
	else /* Player 전체 */
	{
		_vector vNearPlayerPos; _vector vNearSetDir;
		_float  fNearDistance;
		for (_uint Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(Index).pTransform;
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			vSetDir = vPlayerPos - vPos;
			fDistance = XMVector3Length(vSetDir).m128_f32[0];
			vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);

			if (Index == 0)
			{
				vNearPlayerPos = vPlayerPos;
				vNearSetDir = vSetDir;
				fNearDistance = fDistance;
			}
			else if (fDistance <= fNearDistance)
			{
				vNearPlayerPos = vPlayerPos;
				vNearSetDir = vSetDir;
				fNearDistance = fDistance;
			}
		}

		vPlayerPos = vNearPlayerPos;
		vSetDir = vNearSetDir;
		fDistance = fNearDistance;
	}

	CSlime_Big::Return();

	if (m_bReturn /*&& !m_bWalk*/) // 원래 자리로 돌아감
	{
		_vector vSetDir = m_vInitPos - vPos;
		vSetDir.m128_f32[1] = 0.f; 
		vSetDir = XMVector3Normalize(vSetDir);

		_float4 vDirection;
		XMStoreFloat4(&vDirection, vSetDir);

		m_pController->Movement(fTimeDelta, vDirection);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		//m_pTransformCom->Move_To(m_vInitPos, fTimeDelta * 0.5f, 0.f);
		//CCreature::Push_Collision(nullptr, m_Parts[PART_BODY].pPartySlot->Get_PushCollisionID(), fTimeDelta, 0.5f);

		m_fAttackTime = 0.f;

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;

		return S_OK;
	}

	if (fDistance < 10.f && fDistance > 2.f)
	{
		if (m_bPlayerLook == FALSE && m_iAlertState == 0)
			m_bPlayerLook = TRUE;

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		_float4 fDir; XMStoreFloat4(&fDir, vSetDir);
		
		if (m_bPlayerLook == TRUE && m_iAlertState == 0)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_SLIME_ALERT);
			m_iAlertState += 1;
		}
		else if(m_iAlertState > 0)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_SLIME_TURN);
			m_pController->Movement(fTimeDelta, fDir);
		}

		m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
	}
	else if (fDistance <= 2.f)
	{
		m_fAttackTime += fTimeDelta;

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		if (m_ePrevState == FSM_SLIME_TURN
			&& m_fAttackTime >= 3.f)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_SLIME_BREATHATTACK);
			m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
			
		}
		else if (m_ePrevState == FSM_SLIME_BREATHATTACK && m_fAttackTime >= 6.f
			&& static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState() == FSM_SLIME_TURN))
		{
			m_pFiniteStateMachineCom->Change_State(FSM_SLIME_ATTACK);
			m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
		}
		else if (m_ePrevState == FSM_SLIME_ATTACK
			&& static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState() == FSM_SLIME_TURN))
		{
			m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
			m_fAttackTime = 0.f;
		}

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
	}
	else
	{
		m_fWalkTime += fTimeDelta;

		if (m_fWalkTime > 5.f && m_bWalk == FALSE)
		{
			m_bWalk = TRUE;
		}
		if (m_bWalk)
		{
			_vector vSetDir = vPos - m_vInitPos;
			vSetDir.m128_f32[1] = 0.f;
			_float fDistance = XMVector3Length(vSetDir).m128_f32[0];

			if (m_bInitWalk && fDistance <= 0.01f)
			{
				m_bInitWalk = FALSE;
				m_bWalk = FALSE;
				m_fWalkTime = 0.f;
				return S_OK;
			}

			if (m_fWalkTime > 10.f)
			{
				vSetDir = m_vInitPos - vPos;
				m_bInitWalk = TRUE;
			}
			
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir);
			vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);

			_float4 vDirection;
			XMStoreFloat4(&vDirection, vSetDir);

			//if (m_bInitWalk)
			//{
			//	m_pTransformCom->Move_To_Dir(vSetDir, fTimeDelta * 0.15f, 0.f);
			//	CCreature::Push_Collision(nullptr, m_Parts[PART_BODY].pPartySlot->Get_PushCollisionID(), fTimeDelta, 0.15f);
			//}
			//else
			//{
			//	m_pTransformCom->Go_Straight(fTimeDelta * 0.15f, vSetDir);
			//	CCreature::Push_Collision(nullptr, m_Parts[PART_BODY].pPartySlot->Get_PushCollisionID(), fTimeDelta, 0.15f);
			//}

			m_pController->Movement(fTimeDelta, vDirection);

			m_pFiniteStateMachineCom->Change_State(FSM_SLIME_WALK);
		}
		else
			m_pFiniteStateMachineCom->Change_State(FSM_SLIME_IDLE);
	}

	
	if (m_bPlayerLook)
	{
		m_bPlayerLook = FALSE;
		// 플레이어 처음발견했을때 UI 떠야함
	}

	//CSlime_Big::Add_Fog();

	if (static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState()) == FSM_SLIME_BREATHATTACK)
	{
		if (m_fFogTime > 0.2f || m_bFirstFog == FALSE)
		{
			if (m_bFirstFog == FALSE)
			{
				_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				//vPos.m128_f32[1] += 0.5f;
				vLook.m128_f32[1] = 0.f; XMVector3Normalize(vLook);
				vPos += vLook * 0.5f;
				_matrix WorldMatrix = XMMatrixIdentity();
				WorldMatrix.r[3].m128_f32[0] = vPos.m128_f32[0];
				WorldMatrix.r[3].m128_f32[1] = vPos.m128_f32[1];
				WorldMatrix.r[3].m128_f32[2] = vPos.m128_f32[2];
				WorldMatrix.r[3].m128_f32[3] = vPos.m128_f32[3];

				
				m_pFogCollider->Update(WorldMatrix);
			}

			CSlime_Big::Add_Fog();
			m_bFirstFog = TRUE;
			m_bFogAttack = TRUE;
			m_fFogAttackDelay = 0.0001f;
			m_fFogTime = 0.f;

		}
	}
	else
	{
		m_fFogTime = 0.f;
		m_bFirstFog = FALSE;
	}

	return S_OK;
}

HRESULT CSlime_Big::Return()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPos - m_vInitPos;
	vSetDir.m128_f32[1] = 0.f;
	_float fDistance = XMVector3Length(vSetDir).m128_f32[0];

	if (fDistance > 10.f)
	{
		m_bPlayerLook = FALSE;
		m_bReturn = TRUE;
	}
	else if (fDistance <= 0.1f)
	{
		m_pFiniteStateMachineCom->Change_State(FSM_SLIME_IDLE);
		m_bReturn = FALSE;
	}

	return S_OK;
}

void CSlime_Big::Add_Fog()
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += 0.5f; 
	vLook.m128_f32[1] = 0.f; XMVector3Normalize(vLook);

	CEffect::EFFECT_DESC desc = {};
	desc.eEffectID = EFFECT_FOG;
	desc.fLifeTime = 5.f;
	vPos += vLook * 0.5f;
	desc.vSpawnPos = vPos;
	desc.vScale = { 0.1f, 0.1f, 0.f, 0.f };
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Fog", L"Layer_Effect", &desc)))
		return;

	vPos.m128_f32[0] += 1.f;
	desc.vSpawnPos = vPos;
	desc.fLifeTime = 5.f;
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Fog", L"Layer_Effect", &desc)))
		return;
	
	vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += vLook * 0.5f;
	vPos.m128_f32[0] -= 1.f;
	desc.vSpawnPos = vPos;
	desc.fLifeTime = 5.f;
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Fog", L"Layer_Effect", &desc)))
		return;
	
	vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += vLook * 0.5f;
	vPos.m128_f32[2] += 0.3f;
	desc.vSpawnPos = vPos;
	desc.fLifeTime = 5.f;
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Fog", L"Layer_Effect", &desc)))
		return;
	
	vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += vLook * 0.5f;
	vPos.m128_f32[2] -= 0.1f;
	desc.vSpawnPos = vPos;
	desc.fLifeTime = 5.f;
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Fog", L"Layer_Effect", &desc)))
		return;
}

void CSlime_Big::Collision_State(_float fTimeDelta)
{
	if (m_fHp <= 0.f)
		return;
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	_int iWeaponID = -1;
	vector<CGameObject*> vecPlayer;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	{
		if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;

			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
		}
	}
	else /* Player 전체 */
	{
		if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;
			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}
			
		}
	}
	
	/* 슬라임 상태에 따라 충돌 처리할 콜라이더를 정하고 플레이어와의 충돌 검사 */
	if (eState == FSM_SLIME_ATTACK)
	{
		iWeaponID = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CollisionID();
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_BODY, iWeaponID, COLL_PLAYER_BODY);

	}
	else
		m_bHitPlayer = TRUE;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	{
		/* 플레이어가 충돌했을 때 */
		if (m_bHitPlayer == FALSE && vecPlayer.size() != 0)
		{
			m_bHitPlayer = TRUE;

			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(501, 1024));
		}

		if (m_fFogAttackDelay != 0.f)
		{
			m_fFogAttackDelay += fTimeDelta;
			if (m_fFogAttackDelay > 0.7f)
			{
				++m_iFogAttackNum;
				m_fFogAttackDelay = 0.0001f;
				m_bFogAttack = TRUE;
			}

			if (m_iFogAttackNum >= 3)
			{
				m_bFogAttack = FALSE;
				m_fFogAttackDelay = 0.f;
				m_iFogAttackNum = 0;
			}
			else if (m_bFogAttack == TRUE)
			{
				vector<CGameObject*> Player = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iFogColliderID, COLL_EVENTREACT_PLAYER);
				if (Player.empty() == FALSE)
				{
					m_bFogAttack = FALSE;

					pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(97, 222), ELEMENT_FROST, FSM_END);
				}
			}
		}
	}
	else /* Player 전체 */
	{
		/* 플레이어가 충돌했을 때 */
		if (m_bHitPlayer == FALSE && vecPlayer.size() != 0)
		{
			m_bHitPlayer = TRUE;

			for (auto& pPlayerBody : vecPlayer)
			{
				PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID(eIndex);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);
				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(501, 1024));
			}
		}

		if (m_fFogAttackDelay != 0.f)
		{
			m_fFogAttackDelay += fTimeDelta;
			if (m_fFogAttackDelay > 0.7f)
			{
				++m_iFogAttackNum;
				m_fFogAttackDelay = 0.0001f;
				m_bFogAttack = TRUE;
			}

			if (m_iFogAttackNum >= 3)
			{
				m_bFogAttack = FALSE;
				m_fFogAttackDelay = 0.f;
				m_iFogAttackNum = 0;
			}
			else if (m_bFogAttack == TRUE)
			{
				vector<CGameObject*> Player = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iFogColliderID, COLL_PLAYER_BODY);
				if (Player.empty() == FALSE)
				{
					m_bFogAttack = FALSE;

					for (auto& pPlayerBody : vecPlayer)
					{
						PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
						_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID(eIndex);
						m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);
						pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(97, 222), ELEMENT_FROST);
					}
				}
			}
		}
	}
	
}

FSM_SLIME_STATE CSlime_Big::Get_State(FSM_OBJECT_STATE eForceState)
{
	FSM_SLIME_STATE eSlimeState = FSM_SLIME_END;

	switch (eForceState)
	{
	case FSM_OBJECT_HIT_H:
		eSlimeState = FSM_SLIME_HIT_H;
		break;
	case FSM_OBJECT_HIT_L:
		eSlimeState = FSM_SLIME_HIT_L;
		break;
	case FSM_OBJECT_HIT_THROW:
		eSlimeState = FSM_SLIME_HIT_THROW_START;
		break;
	}

	return eSlimeState;
}

void CSlime_Big::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CSlime_Big::Set_Damage(fDamage * 2.f, ELEMENT_WATER);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CSlime_Big::Set_Damage(fDamage * 1.5f, ELEMENT_WATER);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_MELT:
		CSlime_Big::Set_Damage(fDamage * 2.f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_MELT_REVERSE:
		CSlime_Big::Set_Damage(fDamage * 1.5f, ELEMENT_FROST);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_OVERLOADED:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_SWIRL_FIRE:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_FIRE);
		break;
	case Client::REACT_SWIRL_WATER:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_SWIRL_ELECTRIC:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_SWIRL_FROST:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_SUPERCONDUCT:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_ICE, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_ELECTROCHARGED:
		if (m_pGameInstance->IsProgress_StatusEffect(SE_ELECTRO_CHARGE, m_strStatusTag) == FALSE)
		{
			m_pGameInstance->Add_StatusEffect(SE_ELECTRO_CHARGE, m_strStatusTag, 0, 10.f);
			m_fElectroChargedTime = 1.f;
		}
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WATER, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_FROZEN:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WATER, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_CRYSTALLIZE_FIRE:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_FIRE);
		break;
	case Client::REACT_CRYSTALLIZE_WATER:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_CRYSTALLIZE_ELECTIRC:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_CRYSTALLIZE_FROST:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_ICE);
		break;
	default:
		break;
	}

	if (eReact != REACT_END)
		__super::Create_ReactionFont(eReact, {}, REACT_FROZEN);
}

void CSlime_Big::Set_ElementUI(ELEMENT eElement)
{
	switch (eElement)
	{
	case Client::ELEMENT_FIRE:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_FIRE, TRUE);
		break;
	case Client::ELEMENT_WATER:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_WATER, TRUE);
		break;
	case Client::ELEMENT_WIND:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_WIND, TRUE);
		break;
	case Client::ELEMENT_ELECTRIC:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_LIGHTNING, TRUE);
		break;
	case Client::ELEMENT_FROST:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_ICE, TRUE);
		break;
	case Client::ELEMENT_EARTH:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_EARTH, TRUE);
		break;
	default:
		break;
	}
}

void CSlime_Big::Set_Damage(_float fDamage, ELEMENT eElement)
{
	if (m_fShield != 0.f)
	{
		m_fShield -= fDamage;
		m_pHpBar->Set_CurrentHp(m_fShield);
		if (m_fShield < 0.f)
		{
			m_fShield = 0.f;
			static_cast<CSlime_IceCone*>(m_Parts[PART_SHIELD].pPartySlot)->Break_IceCone(2);
			m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_MONSTER);
			m_pHpBar->Set_MaxHp(m_fHp);
			m_pHpBar->Set_CurrentHp(m_fHp);
		}
		else if (m_fShield > 0.f && m_fShield < 200.f)
		{
			static_cast<CSlime_IceCone*>(m_Parts[PART_SHIELD].pPartySlot)->Break_IceCone(1);
			m_pHpBar->Set_CurrentHp(m_fShield);
		}
	}
	else
	{
		m_fHp -= fDamage;
		if (m_fHp < 0.f)
		{
			m_fHp = 0.f;
			for (auto iter = m_MonsterTarget.begin(); iter != m_MonsterTarget.end(); ++iter)
			{
				if (*iter == this)
				{
					m_MonsterTarget.erase(iter);
					break;
				}
			}
		}
		m_pHpBar->Set_CurrentHp(m_fHp);
	}
	__super::Create_DamageFont(fDamage, eElement); /* 데미지 폰트 */

	CEffect::EFFECT_DESC descc = {};
	descc.eEffectID = EFFECT_HITTING_STAR;
	descc.fLifeTime = 5.f;
	descc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.vSpawnPos.m128_f32[1] += 1.f;
	descc.vScale = { 5.f, 5.f, 1.f, 0.f };

	m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc);
}

HRESULT CSlime_Big::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlime_Big::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 3.f, 3.f, 3.f, 0.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ObjDesc.vInitPos);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(pPlayer->Get_CurrentIndex()).pTransform);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPlayerPos - vPos;
	vSetDir.m128_f32[1] = 0.f;
	vSetDir = XMVector3Normalize(vSetDir);
	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(vSetDir); //

	if (FAILED(Initialize_Controller(5.5f, 0.25f, 0.0f, m_pTransformCom)))
		return E_FAIL;
	
	m_vInitPos = ObjDesc.vInitPos;

	_float4 fPos; XMStoreFloat4(&fPos, ObjDesc.vInitPos);
	m_pController->Set_Position(fPos);

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_vInitPos.m128_f32[2] += 0.1f;

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ FSM_SLIME_ALERT , FSM_SLIME_ATTACK, FSM_SLIME_BREATHATTACK });

	COrtho::ORTHO_DESC tOrthoDesc;

	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
	tOrthoDesc.fPosition = { static_cast<_float>(0.f - g_iWinSizeX / 2), static_cast<_float>(-(0.f - g_iWinSizeY / 2)) };
	tOrthoDesc.fSize = { 100.f,5.f };
	tOrthoDesc.ePage = PAGE_MAIN;

	m_pHpBar = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));
	m_pHpBar->Set_MaxHp(m_fShield);
	m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_SHIELD);
	m_pHpBar->Set_CurrentHp(m_fShield);
	m_pHpBar->Set_OwnerInfo(TEXT("LV.15"), TEXT(""), 0);

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);


	/* Fog Attack Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.8f, 0.8f, 0.8f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);


	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Fog_OBB"), (CComponent**)&m_pFogCollider, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iFogColliderID, m_pFogCollider, this);

	m_strStatusTag = Get_ParticleTag(L"Slime_Big");

	return S_OK;
}

HRESULT CSlime_Big::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CombinedMatrix();
		if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&CombinedWorldMatrix).r[3], 5.f) && m_fHp > 0.f)
			Add_Target(this);

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);
		}

		if (m_Parts[PART_BODY].pPartySlot->Get_Dead() == TRUE)
		{
			if (FAILED(CCreature::Drop_Item(124, 1000)))
				return E_FAIL;

			if (FAILED(CCreature::Drop_Item(143, 1)))
				return E_FAIL;

			m_pGameInstance->Add_InteractionCount(L"BattleBGM", -1);
			Set_Destroy(TRUE);
		}

		if (m_fSelfDamage != 0.f)
		{
			Set_Damage(m_fSelfDamage, ELEMENT_ELECTRIC);
			m_fSelfDamage = 0.f;
		}

		/* 데미지 받았을 때 */
		_float fDamage = m_Parts[PART_BODY].pPartySlot->Get_Damage();
		_bool bPerification = m_Parts[PART_BODY].pPartySlot->Get_Perification();

		FSM_OBJECT_STATE eReceiveState = m_Parts[PART_BODY].pPartySlot->Get_ReceiveState();


		if (fDamage != 0.f && m_fHp != 0.f)
		{
			ELEMENT eElement = m_Parts[PART_BODY].pPartySlot->Get_ReceiveElement();
			Set_ElementUI(eElement);
			ELEMENTAL_REACTION eReaction = m_Parts[PART_BODY].pPartySlot->Get_ElementalReaction(); /* 원소반응 */
			ElementalReaction(fDamage, eReaction);

			if (bPerification == TRUE && m_bInitPerification == TRUE)
			{
				m_Parts[PART_SHIELD].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
			}

			if (bPerification == TRUE && m_bInitPerification == FALSE)
				m_Parts[PART_SHIELD].pPartySlot->Set_Petrification();
			else if (bPerification == TRUE && m_bInitPerification == TRUE)
				m_Parts[PART_SHIELD].pPartySlot->Set_Petrification(FALSE);

			/*if (m_fShield != 0.f)
			{
				m_fShield -= fDamage;
				m_pHpBar->Set_CurrentHp(m_fShield);
				if (m_fShield < 0.f)
				{
					m_fShield = 0.f;
					static_cast<CSlime_IceCone*>(m_Parts[PART_SHIELD].pPartySlot)->Break_IceCone(2);
					m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_MONSTER);
					m_pHpBar->Set_MaxHp(m_fHp);
					m_pHpBar->Set_CurrentHp(m_fHp);
				}
				else if (m_fShield > 0.f && m_fShield < 200.f)
				{
					static_cast<CSlime_IceCone*>(m_Parts[PART_SHIELD].pPartySlot)->Break_IceCone(1);
					m_pHpBar->Set_CurrentHp(m_fShield);
				}
			}
			else
			{
				m_fHp -= fDamage;
				if (m_fHp < 0.f)
				{
					m_fHp = 0.f;
					for (auto iter = m_MonsterTarget.begin(); iter != m_MonsterTarget.end(); ++iter)
					{
						if (*iter == this)
						{
							m_MonsterTarget.erase(iter);
							break;
						}
					}
				}
				m_pHpBar->Set_CurrentHp(m_fHp);
			}*/

			CSlime_Big::Set_Damage(fDamage, eElement);

			if (eReceiveState != FSM_OBJECT_END && m_fHp > 0.f)
			{
				m_pFiniteStateMachineCom->Change_ForceState(CSlime_Big::Get_State(eReceiveState));
				m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);
			}
			//__super::Create_DamageFont(fDamage, m_Parts[PART_BODY].pPartySlot->Get_ReceiveElement()); /* 데미지 폰트 */
		}

		if (m_Parts[PART_BODY].pPartySlot->Get_Dead() == TRUE)
		{
			Set_Destroy(TRUE);
		}

		FSM_SLIME_STATE eState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		if (m_fHp <= 0.f)
		{
			if (eState == FSM_SLIME_HIT_H || eState == FSM_SLIME_HIT_L
				|| eState == FSM_SLIME_HIT_THROW_START || eState == FSM_SLIME_HIT_THROW || eState == FSM_SLIME_HIT_THROW_DOWN)
			{
				m_Parts[PART_SHIELD].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
				m_Parts[PART_SHIELD].pPartySlot->Release_ColliderID();
				m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iFogColliderID);
				m_pFiniteStateMachineCom->Change_State(FSM_SLIME_DIE1);
			}
			else
			{
				m_Parts[PART_SHIELD].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
				m_Parts[PART_SHIELD].pPartySlot->Release_ColliderID();
				m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iFogColliderID);
				m_pFiniteStateMachineCom->Change_ForceState(FSM_SLIME_DIE1);
			}
			return S_OK;
		}

		m_bInitPerification = bPerification;
	}

	m_pHpBar->PreviousTick(fTimeDelta);
	m_pElement_OHD->PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CSlime_Big::Tick(_float fTimeDelta)
{
if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CSlime_Big::Pattern(fTimeDelta)))
			return E_FAIL;

		CSlime_Big::Collision_State(fTimeDelta);
		
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;
		
		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}

		m_pHpBar->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f, 0.f);
		m_pHpBar->Tick(fTimeDelta);
		_float2 vPos = m_pHpBar->Get_WindowPos();
		m_pElement_OHD->Update_Matrix(vPos.x, vPos.y + 45.f, 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);

		m_pController->Apply_Gravity(fTimeDelta);
	}

	return S_OK;
}

HRESULT CSlime_Big::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		Clear_Target();

		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->LateTick(fTimeDelta);
		}

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;

		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);
		if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0 &&
			XMVector3Length(vToPlayer).m128_f32[0] <= 10.f)
		{

			CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
			CPage_Main* m_pPage_Main = (CPage_Main*)pUIManager->Get_UI_Page(PAGE_MAIN);

			m_pPage_Main->Register_On_MiniMap(m_pTransformCom, 0);
			m_pHpBar->LateTick(fTimeDelta);
			m_pElement_OHD->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CSlime_Big::Render()
{
	return S_OK;
}

CSlime_Big* CSlime_Big::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Big* pInstance = new CSlime_Big(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSlime_Big"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSlime_Big::Clone(void* pArg)
{
	CSlime_Big* pInstance = new CSlime_Big(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSlime_Big"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlime_Big::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		Safe_Release(m_pHpBar);
		Safe_Release(m_pElement_OHD);
	}

	Safe_Release(m_pFogCollider);
	
	for (auto& pPart : m_Parts)
	{
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();
}