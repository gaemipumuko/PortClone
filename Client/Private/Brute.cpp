#include "stdafx.h"
#include "Brute.h"
#include "Part_Body.h"
#include "Part_Weapon.h"

#include "GameInstance.h"
#include "Player.h"

#include "Body_Brute.h"

//UI
#include "Hp_Bar.h"
#include "UI_Manager.h"
#include "Page_Main.h"

#include "Effect.h"

CBrute::CBrute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CBrute::CBrute(const CBrute& rhs)
	: CCreature(rhs)
{
}

HRESULT CBrute::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = m_eCreatureID;
	tBodyDesc.eType = CTY_MONSTER;
	tBodyDesc.pController = m_pController;

	pPartObject = CPart_Body::Create_Body(m_eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(m_eCreatureID);

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

	if (m_eCreatureID == MONSTER_BRUTE_SHIELD)
		pPartObject = CPart_Weapon::Create_Weapon(WEAPON_SHIELD, tWeaponDesc);
	else if(m_eCreatureID == MONSTER_BRUTE_AXE)
		pPartObject = CPart_Weapon::Create_Weapon(WEAPON_AXE, tWeaponDesc);

	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CBrute::Pattern(_float fTimeDelta)
{
	// 현재 내 상태 가져오기
	FSM_BRUTE_STATE m_eCurrentState = static_cast<FSM_BRUTE_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	//// 플레이어 정보 구해놓기
	//CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	//_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	//CTransform* pPlayerTransform =
	//	static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	//_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));

	// 내 정보들 구해놓기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

	// 플레이어를 향하는 방향벡터
	//_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	_vector vPlayerPos = {};
	_vector vPlayerLook = {};
	_vector vDirectionToPlayer = {};
	_float fDistance = 0.f;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 1명만 온필드 */
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);
		fDistance = XMVector3Length(vDirectionToPlayer).m128_f32[0];
		vDirectionToPlayer.m128_f32[1] = 0.f; 
	}
	else /* Player 전체 */
	{
		_vector vNearPlayerPos = {}; 
		_vector vNearSetDir = {};
		_float  fNearDistance = {};

		for (size_t Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(static_cast<_int>(Index)).pTransform;
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);
			fDistance = XMVector3Length(vDirectionToPlayer).m128_f32[0];
			vDirectionToPlayer.m128_f32[1] = 0.f;

			if (Index == 0)
			{
				vNearPlayerPos = vPlayerPos;
				vNearSetDir = vDirectionToPlayer;
				fNearDistance = fDistance;
			}
			else if (fDistance <= fNearDistance)
			{
				vNearPlayerPos = vPlayerPos;
				vNearSetDir = vDirectionToPlayer;
				fNearDistance = fDistance;
			}
		}

		vPlayerPos = vNearPlayerPos;
		vDirectionToPlayer = vNearSetDir;
		fDistance = fNearDistance;
	}
	 
	if (m_pGameInstance->Key_Down('5'))
	{
		_float4 fPlayerPos = {};
		XMStoreFloat4(&fPlayerPos, vPlayerPos);
		m_pController->Set_Position(fPlayerPos);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	}

	// 플레이어를 인지하지 못한 상태일때 플레이어가 근처에 온다면? 플레이어 인지
	if (m_bIsEnemyDetected == FALSE)
	{
		if (VerifyIsPlayerNearby(9.f))
			m_bIsEnemyDetected = TRUE;
	} 

	//if (VerifyIsPlayerInFront() == TRUE)
	//	cout << " 앞쪽 " << endl;
	//else
	//	cout << " 뒤쪽 " << endl;
		

	switch (m_eCurrentState)
	{
	case Client::FSM_BRUTE_THINK:
	{
		if (m_eCreatureID == MONSTER_BRUTE_SHIELD)			// 쉴드 폭추라면
		{
			if (m_bIsEnemyDetected == FALSE)		// 적을 인지한 상태가 아니라면
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_SHIELD);
			else if (m_bIsEnemyDetected == TRUE)		// 적을 인지한 상태라면
			{
				if (m_bIsAttacking == TRUE)	// 스윙 공격중이라면 패턴에 맞게 이어서 공격
				{
					Shield_Attack_Pattern();
					break;
				}

				if (VerifyIsPlayerNearby(4.f))		// 적이 가깝다면
				{
					if (m_bAlreadyWalkedBackward == FALSE)		// 아직 뒤로 걸어가지 않았다면 
						m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_BACKWARD_WITH_SHIELD);
					else	// 뒤로 이미 한번 걸어갔다면 
						m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
				}
				else	// 적이 멀리 있다면
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
			}
		}
		else if (m_eCreatureID == MONSTER_BRUTE_AXE)		// 도끼 폭츄라면
		{
			if (m_bIsEnemyDetected == FALSE)		// 적을 인지한 상태가 아니라면
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_AXE);
			else if (m_bIsEnemyDetected == TRUE)		// 적을 인지한 상태라면
			{
				if (m_bIsAttacking == TRUE)	// 스윙 공격중이라면 패턴에 맞게 이어서 공격
				{
					Axe_Attack_Pattern();
					break;
				}

				if (VerifyIsPlayerNearby(4.f))		// 적이 가깝다면
				{
					if (m_bAlreadyWalkedBackward == FALSE)		// 아직 뒤로 걸어가지 않았다면 
						m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_BACKWARD_WITH_AXE);
					else	// 뒤로 이미 한번 걸어갔다면 
						m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_RUN_WITH_AXE);
				}
				else	// 적이 멀리 있다면
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_RUN_WITH_AXE);
			}
		}
		break;
	}
	case Client::FSM_BRUTE_FIND_PLAYER:
	{
		if (m_eCreatureID == MONSTER_BRUTE_SHIELD)			// 쉴드 폭추라면
		{
			_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
			if (fAngleDifference > 30.f)
			{
				if (VerifyIsPlayerOnLeft() == TRUE)
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_TURN_LEFT_WITH_SHIELD);
				else
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_TURN_RIGHT_WITH_SHIELD);
			}
			else
			{
				if (VerifyIsPlayerNearby(4.f) == TRUE)
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_BACKWARD_WITH_SHIELD);
				else
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
			}

		}
		else if (m_eCreatureID == MONSTER_BRUTE_AXE)		// 도끼 폭츄라면
		{
			_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
			if (fAngleDifference > 30.f)
			{
				if (VerifyIsPlayerOnLeft() == TRUE)
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_TURN_LEFT_WITH_AXE);
				else
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_TURN_RIGHT_WITH_AXE);
			}
			else
			{
				if (VerifyIsPlayerNearby(4.f) == TRUE)
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_BACKWARD_WITH_AXE);
				else
					m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_RUN_WITH_AXE);
			}
		}
	}
	break;
	case Client::FSM_BRUTE_SIT_TALK:
		break;
	case Client::FSM_BRUTE_SLEEP:
		break;
	case Client::FSM_BRUTE_SIT_IDLE:
		break;
	case Client::FSM_BRUTE_SIT:
		break;
	case Client::FSM_BRUTE_STANDUP:
		break;
	case Client::FSM_BRUTE_KNEEL_AND_NOD:
		break;
	case Client::FSM_BRUTE_WALK_RIGHT_WITH_SHIELD:
	{
		//m_fPatternTimer += fTimeDelta;

		//if (m_fPatternTimer >= 2.f)
		//{
		//	m_fPatternTimer = 0.f;
		//	m_pFiniteStateMachineCom->Change_RegulateState(FSM_BRUTE_THINK);
		//	m_bAlreadyWalkedBackward = TRUE;
		//}

		//m_pTransformCom->Go_Right(fTimeDelta * 0.5f);
		//m_pTransformCom->Reset_Look_At_Direction();
		//m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_BRUTE_WALK_LEFT_WITH_SHIELD:
	{
		//m_fPatternTimer += fTimeDelta;

		//if (m_fPatternTimer >= 2.f)
		//{
		//	m_fPatternTimer = 0.f;
		//	m_pFiniteStateMachineCom->Change_RegulateState(FSM_BRUTE_THINK);
		//	m_bAlreadyWalkedBackward = TRUE;
		//}

		//m_pTransformCom->Go_Left(fTimeDelta * 0.5f);
		//m_pTransformCom->Reset_Look_At_Direction();
		//m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_BRUTE_WALK_FORWARD_WITH_SHIELD:
	{
		//m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);
		//m_pTransformCom->Reset_Look_At_Direction();
		//m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);

		//if (VerifyIsPlayerNearby(4.f))
		//	m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_PUNCH_WITH_SHIELD);
	}
	break;
	case Client::FSM_BRUTE_WALK_BACKWARD_WITH_SHIELD:
	{
		//m_fPatternTimer += fTimeDelta;

		//if (m_fPatternTimer >= 2.f)
		//{
		//	m_fPatternTimer = 0.f;
		//	m_pFiniteStateMachineCom->Change_RegulateState(FSM_BRUTE_THINK);
		//	m_bAlreadyWalkedBackward = TRUE;
		//}

		//m_pTransformCom->Go_Backward(fTimeDelta * 0.5f);
		//m_pTransformCom->Reset_Look_At_Direction();
		//m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_BRUTE_TURN_RIGHT_WITH_SHIELD:
	{
		//m_pTransformCom->Reset_Look_At_Direction();
		//m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 11.f);

		//// 각도 차이 계산
		//_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
		//if (fAngleDifference <= 20.f)	// 플레이어를 거의 정확히 바라봤다면
		//{
		//	m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
		//}
	}
	break;
	case Client::FSM_BRUTE_TURN_LEFT_WITH_SHIELD:
	{
		//m_pTransformCom->Reset_Look_At_Direction();
		//m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 11.f);

		//// 각도 차이 계산
		//_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
		//if (fAngleDifference <= 20.f)	// 플레이어를 거의 정확히 바라봤다면
		//{
		//	m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
		//}
	}
	break;
	case Client::FSM_BRUTE_THROW_SOMETHING_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_PUNCH_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_RUSH_LOOP_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_START_RUSH_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_FINISH_RUSH_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_ROAR_AND_SUMMON_SLIME_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_IDLE_WITH_SHIELD:
	{
		//if (m_bIsEnemyDetected == TRUE)
		//	m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_FIND_PLAYER);	// 플레이어 발견 상태
	}
	break;
	case Client::FSM_BRUTE_HIT_AND_FALL_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_HIT_AND_FALLING_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_HIT_AND_DIE_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_HIT_AND_FLY_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_HIT_ON_BACK_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_HIT_ON_BACK_STRONGLY_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_TURN_OFF_FIRE_SHIELD:
		break;
	case Client::FSM_BRUTE_BLOCK_AND_COUNTER_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_BLOCK_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_SUMMON_SHIELD_WITH_SLIME:
		break;
	case Client::FSM_BRUTE_WALK_RIGHT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_WALK_LEFT_WITHHANDS:
		break;
	case Client::FSM_BRUTE_WALK_FORWARD_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_WALK_BACKWARD_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_TURN_RIGHT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_TURN_LEFT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_DOUBLE_HAND_HIT_ON_GROUND_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_THROW_SLIME_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_ROAR_AND_SWOOP_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_RUSH_LOOP_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_START_RUSH_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_STOP_RUSH_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_RUN_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_POWER_UP_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_POWER_DOWN_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_JUMP_ATTACK_ON_GROUND_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_IDLE_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_HIT_AND_FALL_ON_GROUND_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_HIT_AND_FALLING_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_HIT_AND_DIE_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_HIT_AND_FLY_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_WEAK_HIT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_STRONG_HIT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_SWOOP_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_ATTACK4_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_ATTACK3_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_ATTACK2_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_ATTACK1_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_FALL_FORWARD_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_FALL_FORWARD_AND_DIE_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_SOFT_HIT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_DROWNING_LOOP:
		break;
	case Client::FSM_BRUTE_DROWNING:
		break;
	case Client::FSM_BRUTE_DIE:
		break;
	case Client::FSM_BRUTE_WHIRL_LOOP_WITH_AXE:
	{
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 2.8f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 0.7f);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_BRUTE_WHIRL_CRASH_WALL_WITH_AXE:
		break;
	case Client::FSM_BRUTE_START_AXE_WHIRL:
		break;
	case Client::FSM_BRUTE_FINISH_AXE_WHIRL:
		break;
	case Client::FSM_BRUTE_WALK_FORWARD_WITH_AXE:
	{
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 2.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_BRUTE_WALK_BACKWARD_WITH_AXE:
	{
		m_fPatternTimer += fTimeDelta;

		if (m_fPatternTimer >= 2.f)
		{
			m_fPatternTimer = 0.f;
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_BRUTE_THINK);
			m_bAlreadyWalkedBackward = TRUE;
		}

		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, -vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 2.f);
		//m_pTransformCom->Go_Backward(fTimeDelta * 0.5f);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_BRUTE_TURN_RIGHT_WITH_AXE:
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 11.f);

		// 각도 차이 계산
		_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
		if (fAngleDifference <= 20.f)	// 플레이어를 거의 정확히 바라봤다면
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_RUN_WITH_AXE);
		}
	}
	break;
	case Client::FSM_BRUTE_TURN_LEFT_WITH_AXE:
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 11.f);

		// 각도 차이 계산
		_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
		if (fAngleDifference <= 20.f)	// 플레이어를 거의 정확히 바라봤다면
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_RUN_WITH_AXE);
		}
	}
	break;
	case Client::FSM_BRUTE_SLIME_PROPERTY_TO_AXE:
		break;
	case Client::FSM_BRUTE_THROW_SLIME_WITH_AXE:
		break;
	case Client::FSM_BRUTE_RUN_WITH_AXE:
	{
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 4.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 1.f);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);

		if (VerifyIsPlayerNearby(4.f))
			Identify_Axe_AttackType();
	}
	break;
	case Client::FSM_BRUTE_PICK_UP_SLIME_FROM_GROUND_WITH_AXE:
		break;
	case Client::FSM_BRUTE_ELECTRIFY_AXE_3:
		break;
	case Client::FSM_BRUTE_ELECTRIFY_AXE_2:
		break;
	case Client::FSM_BRUTE_ELECTRIFY_AXE_1:
		break;
	case Client::FSM_BRUTE_START_ELECTRIFY_AXE:
		break;
	case Client::FSM_BRUTE_FINISH_ELECTRIFY_AXE_AND_FALL_BACKWARD:
		break;
	case Client::FSM_BRUTE_JUMP_ATTACK_WITH_AXE:
		break;
	case Client::FSM_BRUTE_IDLE_WITH_AXE:
	{
		if (m_bIsEnemyDetected == TRUE)
			m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_FIND_PLAYER);	// 플레이어 발견 상태
	}
	break;
	case Client::FSM_BRUTE_HIT_AND_FALL_WITH_AXE:
		break;
	case Client::FSM_BRUTE_HIT_AND_FALLING_WITH_AXE:
		break;
	case Client::FSM_BRUTE_WEAK_HIT_WITH_AXE:
		break;
	case Client::FSM_BRUTE_STRONG_HIT_WITH_AXE:
		break;
	case Client::FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
		}
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_4:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
		}
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_3:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
		}
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_2:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
		}
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_1:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
		}
	}
	break;
	case Client::FSM_BRUTE_UNKNOWN_ACTION:
		break;
	case Client::FSM_BRUTE_END:
		break;
	default:
		break;
	}

	return S_OK;
}

/// <summary>
/// 어떤 걷기를 할것인지 판별
/// </summary>
/// <returns></returns>
HRESULT CBrute::IdentifyWalkType()
{
	_int iRandomAttack; 

	if (m_eCreatureID == MONSTER_BRUTE_SHIELD)
	{
		iRandomAttack = m_pGameInstance->Get_Int_Random(1, 4);

		switch (iRandomAttack)
		{
		case 1:
			// 오른쪽 경계 걷기
			break;
		case 2:
			// 왼쪽 경계 걷기
			break;
		case 3:
			// 정면 경계 걷기
			break;
		case 4:
			// 후방 경계 걷기
			break;
		}

	}
	else if (m_eCreatureID == MONSTER_BRUTE_AXE)
	{
		iRandomAttack = m_pGameInstance->Get_Int_Random(1, 2);
		iRandomAttack = 1;

		switch (iRandomAttack)
		{
		case 1 :
			m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_AXE);
			break;
		case 2 :
			m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_BACKWARD_WITH_AXE);
			break;
		}

	}

	return S_OK;
}

/// <summary>
/// 어떤 타입의 스윙 공격을 할것인지 판별
/// </summary>
/// <returns></returns>
HRESULT CBrute::Identify_Shield_AttackType()
{
	/*FSM_BRUTE_PUNCH_WITH_SHIELD*/
	return S_OK;
}

/// <summary>
/// 방패 공격 제어 함수
/// </summary>
/// <returns></returns>
HRESULT CBrute::Shield_Attack_Pattern()
{
	m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_1);

	return S_OK;
}

/// <summary>
/// 어떤 타입의 스윙 공격을 할것인지 판별
/// </summary>
/// <returns></returns>
HRESULT CBrute::Identify_Axe_AttackType()
{
	_int iRandomAttack = m_pGameInstance->Get_Int_Random(1, 5);

	switch (iRandomAttack)
	{
	case 1:
		m_eAttack_Type = AXE_ATTACK_TYPE_1;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_1);
		m_iAttackIndex++;
		m_bIsAttacking = TRUE;
		break;
	case 2:
		m_eAttack_Type = AXE_ATTACK_TYPE_2;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_4);
		m_iAttackIndex++;
		m_bIsAttacking = TRUE;
		break;
	case 3:
		m_eAttack_Type = AXE_ATTACK_TYPE_3;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_1);
		m_iAttackIndex++;
		m_bIsAttacking = TRUE;
		break;
	case 4:
		m_eAttack_Type = AXE_ATTACK_TYPE_WHIRL;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_START_AXE_WHIRL);
		break;
	case 5:
		m_eAttack_Type = AXE_ATTACK_TYPE_DANCE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE);
	default:
		return E_FAIL;
	}

	return S_OK;
}

/// <summary>
/// 스윙 공격 제어 함수
/// </summary>
/// <returns></returns>
HRESULT CBrute::Axe_Attack_Pattern()
{
	if (m_eCreatureID == MONSTER_BRUTE_SHIELD)			// 방패 BRUTE 라면
	{

	}
	else if (m_eCreatureID == MONSTER_BRUTE_AXE)		// 도끼 BRUTE 라면
	{
		switch (m_eAttack_Type)
		{
		case Client::CBrute::AXE_ATTACK_TYPE_1:
		{
			m_fPatternTimer = 0.f;
			if (m_iAttackIndex == 1)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_3);
				m_iAttackIndex++;
			}
			else if (m_iAttackIndex == 2)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_AXE);
				m_iAttackIndex = 0;
				m_bIsAttacking = FALSE;
			}
		}
		break;
		case Client::CBrute::AXE_ATTACK_TYPE_2:
		{
			m_fPatternTimer = 0.f;
			if (m_iAttackIndex == 1)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_2);
				m_iAttackIndex++;
			}
			else if (m_iAttackIndex == 2)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_AXE);
				m_iAttackIndex = 0;
				m_bIsAttacking = FALSE;
			}
		}
		break;
		case Client::CBrute::AXE_ATTACK_TYPE_3:
		{
			m_fPatternTimer = 0.f;
			if (m_iAttackIndex == 1)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_3);
				CBody_Brute* pBruteBody = dynamic_cast<CBody_Brute*>(m_Parts[PART_BODY].pPartySlot);
				pBruteBody->Set_IsSwing_Type_3(TRUE);
				m_iAttackIndex++;
			}
			else if (m_iAttackIndex == 2)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_ATTACK_2);
				m_iAttackIndex++;

			}
			else if (m_iAttackIndex == 3)
			{
				//m_pGameInstance->PlaySoundW(L"Brute_Chopping.mp3", 100.f);

				m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_AXE);
				m_iAttackIndex = 0;
				m_bIsAttacking = FALSE;
				CBody_Brute* pBruteBody = dynamic_cast<CBody_Brute*>(m_Parts[PART_BODY].pPartySlot);
				pBruteBody->Set_IsSwing_Type_3(FALSE);
			}
		}
		break;
		}
	}
	return S_OK;
}

/// <summary> 
/// /// 플레이어가 왼쪽에 있는지, 오른쪽에 있는지 판별하는 함수, // True면 왼쪽, False 면 오른쪽
/// </summary>
/// <returns></returns>
_bool CBrute::VerifyIsPlayerOnLeft()
{
	_vector vPlayerPos = {};
	_vector vSelfPos = {};
	_vector vDirToPlayer = {};
	_float fDistance = 0;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (pPlayer->Get_CombatMode() == FALSE)
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		vDirToPlayer = vPlayerPos - vSelfPos;
		fDistance = XMVector3Length(vDirToPlayer).m128_f32[0];
		vDirToPlayer.m128_f32[1] = 0.f;
		vDirToPlayer = XMVector3Normalize(vDirToPlayer);

		// 플레이어를 향하는 벡터와 수직인 벡터
		_vector vPerpendicular = XMVector3Normalize(XMVectorSet(vDirToPlayer.m128_f32[2], 0.0f, -vDirToPlayer.m128_f32[0], 0.0f));

		// 자신의 Look과 플레이어를 향하는 벡터와 수직인 벡터를 내적
		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_float fDotResult = XMVectorGetX(XMVector3Dot(vSelfLook, XMVector3Normalize(vPerpendicular)));

		// 내적의 결과가 양수 = 왼쪽
		// 내적의 결과가 음수 = 오른쪽
		// 내적의 결과가 -1 = 정반대
		if (fDotResult > 0)
			return TRUE;
		else if (fDotResult < 0)
			return FALSE;
		else if (fDotResult == -1)
			return TRUE;
	}
	else
	{
		_vector vNearPlayerPos = {};
		_vector vNearDirToPlayer = {};
		_vector vNearPlayerLook = {};
		_float fNearDistance = 0;
		for (size_t Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(static_cast<_int>(Index)).pTransform;

			vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			vDirToPlayer = vPlayerPos - vSelfPos;
			fDistance = XMVector3Length(vDirToPlayer).m128_f32[0];
			vDirToPlayer.m128_f32[1] = 0.f;
			vDirToPlayer = XMVector3Normalize(vDirToPlayer);

			if (Index == 0)
			{
				vNearPlayerPos = vPlayerPos;
				vNearDirToPlayer = vDirToPlayer;
				fNearDistance = fDistance;
			}
			else if (fDistance <= fNearDistance)
			{
				vNearPlayerPos = vPlayerPos;
				vNearDirToPlayer = vDirToPlayer;
				fNearDistance = fDistance;
			}
		}

		vPlayerPos = vNearPlayerPos;
		vDirToPlayer = vNearDirToPlayer;
		fDistance = fNearDistance;

		// 플레이어를 향하는 벡터와 수직인 벡터
		_vector vPerpendicular = XMVector3Normalize(XMVectorSet(vDirToPlayer.m128_f32[2], 0.0f, -vDirToPlayer.m128_f32[0], 0.0f));

		// 자신의 Look과 플레이어를 향하는 벡터와 수직인 벡터를 내적
		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_float fDotResult = XMVectorGetX(XMVector3Dot(vSelfLook, XMVector3Normalize(vPerpendicular)));

		// 내적의 결과가 양수 = 왼쪽
		// 내적의 결과가 음수 = 오른쪽
		// 내적의 결과가 -1 = 정반대
		if (fDotResult > 0)
			return TRUE;
		else if (fDotResult < 0)
			return FALSE;
		else if (fDotResult == -1)
			return TRUE;
	}

	return TRUE;
}

/// <summary>
/// 플레이어가 가까이에 있는지 판별하는 함수
/// </summary>
/// <returns></returns>
_bool CBrute::VerifyIsPlayerNearby(_float _fDistance)
{
	_vector vSelfPos = {};
	_vector vPlayerPos = {};
	_vector vDirToPlayer = {};
	_float fDistance = 0;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (pPlayer->Get_CombatMode() == FALSE)
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		vDirToPlayer = vPlayerPos - vSelfPos;
		fDistance = XMVectorGetX(XMVector3Length(vDirToPlayer));

		return fDistance < _fDistance ? TRUE : FALSE;
	}
	else
	{
		_vector vNearPlayerPos = {};
		_vector vNearDirToPlayer = {};
		_vector vNearPlayerLook = {};
		_float fNearDistance = 0;
		for (size_t Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(static_cast<_int>(Index)).pTransform;

			vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			vDirToPlayer = vPlayerPos - vSelfPos;
			fDistance = XMVector3Length(vDirToPlayer).m128_f32[0];
			vDirToPlayer.m128_f32[1] = 0.f;
			vDirToPlayer = XMVector3Normalize(vDirToPlayer);

			if (Index == 0)
			{
				fNearDistance = fDistance;
			}
			else if (fDistance <= fNearDistance)
			{
				fNearDistance = fDistance;
			}
		}

		fDistance = fNearDistance;

		return fDistance < _fDistance ? TRUE : FALSE;
	}
}

/// <summary>
/// 플레이어가 앞에 있는지 뒤에 있는지 판별하는 함수, // True면 앞에 있음, False면 뒤에 있음
/// </summary>
/// <returns></returns>
_bool CBrute::VerifyIsPlayerInFront()
{
	_vector vPlayerPos = {};
	_vector vSelfPos = {};
	_vector vDirToPlayer = {};
	_float fDistance = 0;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (pPlayer->Get_CombatMode() == FALSE)
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		vDirToPlayer = vPlayerPos - vSelfPos;
		fDistance = XMVector3Length(vDirToPlayer).m128_f32[0];
		vDirToPlayer.m128_f32[1] = 0.f;
		vDirToPlayer = XMVector3Normalize(vDirToPlayer);

		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float fDotResult = XMVectorGetX(XMVector3Dot(vSelfLook, vDirToPlayer));

		return fDotResult > 0 ? TRUE : FALSE;
	}
	else
	{
		_vector vNearPlayerPos = {};
		_vector vNearDirToPlayer = {};
		_vector vNearPlayerLook = {};
		_float fNearDistance = 0;
		for (size_t Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(static_cast<_int>(Index)).pTransform;

			vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			vDirToPlayer = vPlayerPos - vSelfPos;
			fDistance = XMVector3Length(vDirToPlayer).m128_f32[0];
			vDirToPlayer.m128_f32[1] = 0.f;
			vDirToPlayer = XMVector3Normalize(vDirToPlayer);
 
			if (Index == 0)
			{
				vNearPlayerPos = vPlayerPos;
				vNearDirToPlayer = vDirToPlayer;
				fNearDistance = fDistance;	
			}
			else if(fDistance <= fNearDistance)
			{
				vNearPlayerPos = vPlayerPos;
				vNearDirToPlayer = vDirToPlayer;
				fNearDistance = fDistance;	
			}
		}
		
		vPlayerPos = vNearPlayerPos;
		vDirToPlayer = vNearDirToPlayer;
		fDistance = fNearDistance;

		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_float fDotResult = XMVectorGetX(XMVector3Dot(vSelfLook, vDirToPlayer));

		return fDotResult > 0 ? TRUE : FALSE;
	}
}

void CBrute::Collision_State()
{
	if (m_fHp <= 0.f)
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	FSM_BRUTE_STATE eState = static_cast<FSM_BRUTE_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
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

	if ((eState >= FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE && eState <= FSM_BRUTE_AXE_ATTACK_1) || eState == FSM_BRUTE_WHIRL_LOOP_WITH_AXE)
	{
		iWeaponID = static_cast<CPart_Body*>(m_Parts[PART_WEAPON].pPartySlot)->Get_CollisionID();
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);
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

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(521, 1021));
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

				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(521, 1021));
			}
		}
	}
}

FSM_BRUTE_STATE CBrute::Get_State(FSM_OBJECT_STATE eForceState)
{
	FSM_BRUTE_STATE eBruteState = FSM_BRUTE_END;

	switch (eForceState)
	{
	case FSM_OBJECT_HIT_H:
		eBruteState = FSM_BRUTE_END;
		break;
	case FSM_OBJECT_HIT_L:
		eBruteState = FSM_BRUTE_END;
		break;
	case FSM_OBJECT_HIT_THROW:
		eBruteState = FSM_BRUTE_END;
		break;
	}

	return eBruteState;
}

void CBrute::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CBrute::Set_Damage(fDamage * 2.f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CBrute::Set_Damage(fDamage * 1.5f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_MELT:
		
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_MELT_REVERSE:
		
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
		__super::Create_ReactionFont(eReact);
}

void CBrute::Set_ElementUI(ELEMENT eElement)
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

void CBrute::Set_Damage(_float fDamage, ELEMENT eElement)
{
	// 체력에 데미지 적용
	m_fHp -= fDamage;
	if (m_fHp < 0.f)
		m_fHp = 0.f;
	m_pHpBar->Set_CurrentHp(m_fHp);

	if (eElement != ELEMENT_END)
		__super::Create_DamageFont(fDamage, m_Parts[PART_BODY].pPartySlot->Get_ReceiveElement()); /* 데미지 폰트 */
	else
		__super::Create_DamageFont(fDamage, eElement); /* 데미지 폰트 */

	CEffect::EFFECT_DESC descc = {};
	descc.eEffectID = EFFECT_HITTING_STAR;
	descc.fLifeTime = 5.f;
	descc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.vSpawnPos.m128_f32[1] += 1.f;
	descc.vScale = { 5.f, 5.f, 1.f, 0.f };

	m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc);
}

HRESULT CBrute::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBrute::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);
	m_vInitPos = ObjDesc.vInitPos;
	m_eCreatureID = static_cast<CREATUREID>(ObjDesc.iObjectType);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_Controller(2.0f, 2.5f, 0.0f, m_pTransformCom)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;
	 
	_float4 fInitPos = {};
	XMStoreFloat4(&fInitPos, m_vInitPos);
	m_pController->Set_Position(fInitPos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);

	// 플레이어 정보 구해놓기
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
	CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPlayerPos - vPos;
	vSetDir.m128_f32[1] = 0.f;
	vSetDir = XMVector3Normalize(vSetDir);
	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(vSetDir); //

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ 
		// 강제로 변경되어야 할 패턴들
	});
	
	// 방패 
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_SHIELD);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_WALK_LEFT_WITH_SHIELD);
	

	// 도끼
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE);
	m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_IDLE_WITH_AXE);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_THINK);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_RUN_WITH_AXE);

#pragma region UI
	COrtho::ORTHO_DESC tOrthoDesc;
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
	tOrthoDesc.fPosition = { static_cast<_float>(0.f - g_iWinSizeX / 2), static_cast<_float>(-(0.f - g_iWinSizeY / 2)) };
	tOrthoDesc.fSize = { 100.f,5.f };
	tOrthoDesc.ePage = PAGE_MAIN;

	m_pHpBar = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));
	m_pHpBar->Set_MaxHp(m_fHp);
	m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_MONSTER);
	m_pHpBar->Set_CurrentHp(m_fHp);
	m_pHpBar->Set_OwnerInfo(TEXT("LV.70"), TEXT(""), 0);
#pragma endregion

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_strStatusTag = Get_ParticleTag(L"Brute");

	return S_OK;
}

HRESULT CBrute::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		// 절두체 안에있다면, 
		// 에 자기자신을 추가
		_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CombinedMatrix();
		if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&CombinedWorldMatrix).r[3], 5.f) && m_fHp > 0.f)
			Add_Target(this);

		// Parts 들의 PreviousTick 실행
		for (auto& pPart : m_Parts)
			pPart.pPartySlot->PreviousTick(fTimeDelta);

		if (m_fSelfDamage != 0.f)
		{
			Set_Damage(m_fSelfDamage, ELEMENT_ELECTRIC);
			m_fSelfDamage = 0.f;
		}

		// 데미지 받았을 때
		_float fDamage = m_Parts[PART_BODY].pPartySlot->Get_Damage();				// 데미지 받았는지 체크
		_bool bPerification = m_Parts[PART_BODY].pPartySlot->Get_Perification();	// 석화중인지 체크
		FSM_OBJECT_STATE eReceiveState = m_Parts[PART_BODY].pPartySlot->Get_ReceiveState();		// 현재 오브젝트의 공통상태를 받아옴 ( 무슨 피격 상태인지 )

		if (fDamage != 0.f && m_fHp != 0.f)		// 데미지를 받았고, 체력이 0이 아니라면
		{
			ELEMENT eElement = m_Parts[PART_BODY].pPartySlot->Get_ReceiveElement();
			Set_ElementUI(eElement);
			ELEMENTAL_REACTION eReaction = m_Parts[PART_BODY].pPartySlot->Get_ElementalReaction(); /* 원소반응 */
			ElementalReaction(fDamage, eReaction);

			if (bPerification == TRUE)	// 석화중이라면
			{
				if (m_bInitPerification == TRUE) // 석화 시작 안 했으면
				{
					m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE); // 석화 해제
					m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);   // 석화 해제
				}
				else // 석화 시작했으면
					m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(TRUE); // 석화 적용
			}

			CBrute::Set_Damage(fDamage, eElement);

			if (eReceiveState != FSM_OBJECT_END && m_fHp > 0.f)		// 피격중이고, 체력이 0 이상이라면
			{
				//if (static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState()) == FSM_HILI_RUNATTACK_END)
				//{
				//	m_Parts[PART_BODY].pPartySlot->Set_RootTranslation(m_Parts[PART_BODY].pPartySlot->Get_PrevRootTranslation());
				//}

				//if (bPerification == FALSE)		// 석화중이 아니라면
				//	m_pFiniteStateMachineCom->Change_ForceState(CBrute::Get_State(eReceiveState));	// 자기 자신의 상태를 현재 공통피격상태에 따라 변경

				m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);
			}
		}

		// 바디가 죽었다면 
		if (m_Parts[PART_BODY].pPartySlot->Get_Dead() == TRUE)
		{
			if (m_bDestroy == FALSE)
			{
				if (FAILED(CCreature::Drop_Item(124, 1000)))
					return E_FAIL;

				if (FAILED(CCreature::Drop_Item(m_pGameInstance->Get_Int_Random(147, 172), 1)))
					return E_FAIL;

				if (FAILED(CCreature::Drop_Item(m_pGameInstance->Get_Int_Random(147, 172), 1)))
					return E_FAIL;

				if (FAILED(CCreature::Drop_Item(m_pGameInstance->Get_Int_Random(0, 4), 1)))
					return E_FAIL;

				m_pGameInstance->Add_InteractionCount(L"BattleBGM", -1);
				Set_Destroy(TRUE);
			}
		}
		FSM_BRUTE_STATE m_eCurrentState = static_cast<FSM_BRUTE_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		// 체력이 음수라면
		if (m_fHp <= 0.f && m_eCurrentState != FSM_BRUTE_DIE)
		{
			m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);
			m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
			m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
			m_Parts[PART_WEAPON].pPartySlot->Release_ColliderID();
			m_pFiniteStateMachineCom->Change_ForceState(FSM_BRUTE_DIE);
			
			return S_OK;
		}

		m_pHpBar->PreviousTick(fTimeDelta);
		m_pElement_OHD->PreviousTick(fTimeDelta);

		m_bInitPerification = bPerification;
	}

	return S_OK;
}

HRESULT CBrute::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CBrute::Pattern(fTimeDelta)))
			return E_FAIL;

		Collision_State();

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
			pPart.pPartySlot->Tick(fTimeDelta);

		// 체력바 
		m_pHpBar->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f, 0.f);
		m_pHpBar->Tick(fTimeDelta);
		_float2 vPos = m_pHpBar->Get_WindowPos();
		m_pElement_OHD->Update_Matrix(vPos.x, vPos.y + 45.f, 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);

		if(m_fHp > 0.f)
			m_pController->Apply_Gravity(fTimeDelta);
	}

	return S_OK;
}

HRESULT CBrute::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		Clear_Target();

		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
			pPart.pPartySlot->LateTick(fTimeDelta);

		// 플레이어의 위치에 따라서 체력바, 미니맵 업데이트
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
		_vector vToPlayer = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fDistanceToPlayer = XMVector3Length(vToPlayer).m128_f32[0];
		if (fDistanceToPlayer <= 10.f)
		{
			if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0)
			{
				CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0));
				CPage_Main* m_pPage_Main = static_cast<CPage_Main*>(pUIManager->Get_UI_Page(PAGE_MAIN));

				m_pPage_Main->Register_On_MiniMap(m_pTransformCom, 0);
				m_pHpBar->LateTick(fTimeDelta);
				m_pElement_OHD->LateTick(fTimeDelta);
			}
		}
	}

	return S_OK;
}

HRESULT CBrute::Render()
{
	return S_OK;
}

CBrute* CBrute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrute* pInstance = new CBrute(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBrute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrute::Clone(void* pArg)
{
	CBrute* pInstance = new CBrute(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBrute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrute::Free()
{
	__super::Free();
	
	if (m_bCloned == true)
	{
		Safe_Release(m_pHpBar);
		Safe_Release(m_pElement_OHD);
	}

	for (auto& pPart : m_Parts)
		Safe_Release(pPart.pPartySlot);

	m_Parts.clear();
}