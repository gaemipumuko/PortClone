#include "stdafx.h"
#include "Hili_Normal.h"
#include "Part_Body.h"
#include "Part_Weapon.h"

#include "GameInstance.h"
#include "Player.h"

//UI
#include "Hp_Bar.h"
#include "UI_Manager.h"
#include "Page_Main.h"

#include "Effect.h"

CHili_Normal::CHili_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CHili_Normal::CHili_Normal(const CHili_Normal& rhs)
	: CCreature(rhs)
{
}

HRESULT CHili_Normal::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = MONSTER_HILI_NORMAL;
	tBodyDesc.pController = m_pController;
	tBodyDesc.eType = CTY_MONSTER;

	CREATUREID eCreatureID = MONSTER_HILI_NORMAL;
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

	pPartObject = CPart_Weapon::Create_Weapon(WEAPON_CLUB, tWeaponDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CHili_Normal::Pattern(_float fTimeDelta)
{
	_bool bPerification = m_Parts[PART_BODY].pPartySlot->Get_Perification();
	if (bPerification == TRUE)
		return S_OK;

	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	if (eState == FSM_HILI_HIT_H || eState == FSM_HILI_HIT_L || eState == FSM_HILI_DIE_1 || eState == FSM_HILI_DIE_2
		|| eState == FSM_HILI_HIT_THROW_START || eState == FSM_HILI_HIT_THROW || eState == FSM_HILI_HIT_THROW_STAND || eState == FSM_HILI_HIT_THROW_DIE)
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
		for (size_t Index = 0; Index < pPlayer->Get_TotalPlayer() ; Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(static_cast<_int>(Index)).pTransform;
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			
			vSetDir = vPlayerPos - vPos;
			fDistance = XMVector3Length(vSetDir).m128_f32[0];
			vSetDir.m128_f32[0]; vSetDir = XMVector3Normalize(vSetDir);

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

	CHili_Normal::Return();

	if (m_bReturn && !m_bWalk && fDistance > 9.f) // 원래 자리로 돌아감
	{
		_vector vSetDir = m_vInitPos - vPos;
		vSetDir.m128_f32[0]; vSetDir = XMVector3Normalize(vSetDir);
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		_float4 vDirection;
		XMStoreFloat4(&vDirection, vSetDir);

		m_pController->Movement(fTimeDelta * 1.f, vDirection);

		//m_pTransformCom->Move_To(m_vInitPos, fTimeDelta * 0.5f, 0.f);

		m_fAttackTime = 0.f;

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;

		m_bAttackReady = FALSE;
		m_bAttack = FALSE;

		m_bPlayerLook = FALSE;
		m_iAlertState = 0;

		m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_WALKFORWARD);
	}

	if ((fDistance <= 9.f || static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState())  == FSM_HILI_RUN)
		&& fDistance > 3.f && !m_bAttack && !m_bAttackReady)
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		if (m_bPlayerLook == FALSE && m_iAlertState == 0)
			m_bPlayerLook = TRUE;
		
		if (m_bPlayerLook == TRUE && m_iAlertState == 0)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_ALERT);
			m_bReturn = FALSE;
			m_iAlertState += 1;

			m_pGameInstance->PlaySoundW(L"Hili_Alert.mp3");
		}

		if (static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState()) == FSM_HILI_RUN)
		{
			_float4 vDirection;
			XMStoreFloat4(&vDirection, vSetDir);

			m_pController->Movement(fTimeDelta * 1.5f, vDirection);

			// m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 1.2f, 2.f);
		}

		m_ePrevState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
	}
	else if (m_bAttackReady == TRUE || m_bAttack == TRUE)
	{
		CHili_Normal::Attack(fTimeDelta);

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
	}
	else if (fDistance <= 3.f)
	{
		m_bAttackReady = TRUE;
		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
	}
	else if(!m_bReturn)
	{
		if (m_iIdlePattern == 1)
			CHili_Normal::Walk(fTimeDelta);
		else if (m_iIdlePattern == 2)
			CHili_Normal::Dance(fTimeDelta);
		else if (m_iIdlePattern == 3)
			CHili_Normal::Sit(fTimeDelta);
		else if (m_iIdlePattern == 4)
			CHili_Normal::Sleep(fTimeDelta);
	}

	if (m_bPlayerLook)
	{
		m_bPlayerLook = FALSE;
		// 플레이어 처음발견했을때 UI 떠야함
	}

	return S_OK;
}

HRESULT CHili_Normal::Attack(_float fTimeDelta)
{
	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (m_bAttack && eState == FSM_HILI_WALKBACK)
	{
		m_bStoreDistance = FALSE;
		m_bAttackReady = TRUE;
		m_bAttack = FALSE;
	}

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 1명만 온필드 */
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		m_vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		m_vSetDir = m_vPlayerPos - vPos;
		m_fDistance = XMVector3Length(m_vSetDir).m128_f32[0];
		m_fInitDistance = m_fDistance;

	}
	else /* Player 전체 */
	{
		_vector vNearPlayerPos; _vector vNearSetDir;
		_float  fNearDistance;
		for (size_t Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(static_cast<_int>(Index)).pTransform;
			m_vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			m_vSetDir = m_vPlayerPos - vPos;
			m_fDistance = XMVector3Length(m_vSetDir).m128_f32[0];

			if (Index == 0)
			{
				vNearPlayerPos = m_vPlayerPos;
				vNearSetDir = m_vSetDir;
				fNearDistance = m_fDistance;
			}
			else if (m_fDistance <= fNearDistance)
			{
				vNearPlayerPos = m_vPlayerPos;
				vNearSetDir = m_vSetDir;
				fNearDistance = m_fDistance;
			}
		}

		m_vPlayerPos = vNearPlayerPos;
		m_vSetDir = vNearSetDir;
		m_fDistance = fNearDistance;
	}
	

	if (m_bAttackReady && eState == FSM_HILI_WALKBACK)
	{
		if (m_bStoreDistance == FALSE)
		{
			m_vInitPlayerPos = m_vPlayerPos;
			m_bStoreDistance = TRUE;
		}
		m_vSetDir = m_vInitPlayerPos - vPos;
	}

	m_vSetDir.m128_f32[1] = 0.f; m_vSetDir = XMVector3Normalize(m_vSetDir);
	
	m_pTransformCom->Reset_Look_At_Direction(); 
	m_pTransformCom->Set_Look_At_Direction(m_vSetDir);

	_float fDistance;
	_vector InitDir = m_vInitPlayerPos - vPos;
	fDistance = XMVector3Length(InitDir).m128_f32[0];

	if (fDistance < 8.f && m_bAttackReady == TRUE) /* 뒤로 가서 공격 준비 */
	{
		_float4 vDirection;
		XMStoreFloat4(&vDirection, m_vSetDir * -1.f);

		m_pController->Movement(fTimeDelta, vDirection, 2.0f);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Look_At_Position(m_vPlayerPos);

		// m_pTransformCom->Go_Backward(fTimeDelta * 0.5f, vSetDir);
		m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_WALKBACK);
	}
	else
	{
		m_bAttack = TRUE;
		m_bAttackReady = FALSE;
	}

	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(m_vSetDir);

	if (m_bAttack 
		&& eState != FSM_HILI_RUNATTACK
		&& eState != FSM_HILI_RUNATTACK_END)
	{
		m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_RUNATTACK_START);
	}
	else if (m_bAttack 
		&& m_fDistance < 2.f && eState != FSM_HILI_RUNATTACK_END)
	{
		m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_RUNATTACK_END);
		m_bInitReady = FALSE;
		m_ePrevState = eState;
	}

	return S_OK;
}

HRESULT CHili_Normal::Return()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPos - m_vInitPos;
	vSetDir.m128_f32[1] = 0.f;
	_float fDistance = XMVector3Length(vSetDir).m128_f32[0];

	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance > 15.f && eState != FSM_HILI_ALERT
		&& eState != FSM_HILI_RUNATTACK_START && eState != FSM_HILI_RUNATTACK)
	{
		m_bStoreDistance = FALSE;
		m_bPlayerLook = FALSE;
		m_bReturn = TRUE;
	}
	else if (fDistance <= 0.1f && eState == FSM_HILI_WALKFORWARD)
	{
		m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_IDLE);
		m_bReturn = FALSE;
	}

	return S_OK;
}

HRESULT CHili_Normal::Walk(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vSetDir = vPlayerPos - vPos;

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

		if (m_bInitWalk && fDistance <= 0.1f)
		{
			m_bInitWalk = FALSE;
			m_bWalk = FALSE;
			m_fWalkTime = 0.f;
			return S_OK;
		}

		if (m_fWalkTime > 10.f)
		{
			vSetDir = m_vInitPos - vPos;
			vSetDir.m128_f32[1] = 0.f;
			m_bInitWalk = TRUE;
		}

		vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);

		if (m_bInitWalk == TRUE && m_fWalkTime < 15.f)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_IDLE);
		}
		else if (m_bInitWalk == TRUE)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir);

			_float4 vDirection;
			XMStoreFloat4(&vDirection, vSetDir);

			m_pController->Movement(fTimeDelta * 1.f, vDirection);

			// m_pTransformCom->Move_To_Dir(vSetDir, fTimeDelta * 0.3f, 0.f);
			m_pFiniteStateMachineCom->Change_State(FSM_HILI_WALKFORWARD);
		}
		else
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir);

			_float4 vDirection;
			XMStoreFloat4(&vDirection, vSetDir);

			m_pController->Movement(fTimeDelta * 1.f, vDirection);

			// m_pTransformCom->Go_Straight(fTimeDelta * 0.3f, vSetDir);
			m_pFiniteStateMachineCom->Change_State(FSM_HILI_WALKFORWARD);
		}
	}
	else
		m_pFiniteStateMachineCom->Change_State(FSM_HILI_IDLE);

	return S_OK;
}

HRESULT CHili_Normal::Dance(_float fTimeDelta)
{
	m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_DANCE);

	return S_OK;
}

HRESULT CHili_Normal::Sit(_float fTimeDelta)
{
	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if(eState != FSM_HILI_SIT_START && eState != FSM_HILI_SIT && eState !=FSM_HILI_SIT_IDLE)
		m_pFiniteStateMachineCom->Change_State(FSM_HILI_SIT_START);

	return S_OK;
}

HRESULT CHili_Normal::Sleep(_float fTimeDelta)
{
	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (eState != FSM_HILI_SLEEP_START && eState != FSM_HILI_SLEEP_1 
		&& eState != FSM_HILI_SLEEP_2 && eState != FSM_HILI_SLEEP_IDLE)
		m_pFiniteStateMachineCom->Change_State(FSM_HILI_SLEEP_START);

	return S_OK;
}

void CHili_Normal::Collision_State()
{
	if (m_fHp <= 0.f)
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
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
		else
			m_bHitPlayer = TRUE;
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
		else
			m_bHitPlayer = TRUE;
	}
	
	if (eState == FSM_HILI_RUNATTACK_START || eState == FSM_HILI_RUNATTACK_END)
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

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(50, 100));
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

				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(50, 100));
			}
		}
	}
}

FSM_HILI_STATE CHili_Normal::Get_State(FSM_OBJECT_STATE eForceState)
{
	FSM_HILI_STATE eHiliState = FSM_HILI_END;

	switch (eForceState)
	{
	case FSM_OBJECT_HIT_H:
		eHiliState = FSM_HILI_HIT_H;
		break;
	case FSM_OBJECT_HIT_L:
		eHiliState = FSM_HILI_HIT_L;
		break;
	case FSM_OBJECT_HIT_THROW:
		eHiliState = FSM_HILI_HIT_THROW_START;
		break;
	}

	return eHiliState;
}

void CHili_Normal::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CHili_Normal::Set_Damage(fDamage * 2.f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CHili_Normal::Set_Damage(fDamage * 1.5f, ELEMENT_FIRE);
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
		m_Parts[PART_BODY].pPartySlot->Set_Freezing(TRUE);
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

void CHili_Normal::Set_ElementUI(ELEMENT eElement)
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

void CHili_Normal::Set_Damage(_float fDamage, ELEMENT eElement)
{
	m_fHp -= fDamage;
	if (m_fHp < 0.f)
	{
		m_fHp = 0.f;
	}
	m_pHpBar->Set_CurrentHp(m_fHp);

	__super::Create_DamageFont(fDamage, eElement); /* 데미지 폰트 */

	CEffect::EFFECT_DESC descc = {};
	descc.eEffectID = EFFECT_HITTING_STAR;
	descc.fLifeTime = 5.f;
	descc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.vSpawnPos.m128_f32[1] += 1.f;
	descc.vScale = { 5.f, 5.f, 1.f, 0.f };

	m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc);
}

HRESULT CHili_Normal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHili_Normal::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ObjDesc.vInitPos);
	m_vInitPos = ObjDesc.vInitPos;
	m_vInitPos.m128_f32[2] += 0.1f;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(pPlayer->Get_CurrentIndex()).pTransform);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPlayerPos - vPos;
	vSetDir.m128_f32[1] = 0.f;
	vSetDir = XMVector3Normalize(vSetDir);
	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(vSetDir); //

	if (FAILED(Initialize_Controller(0.75f, 0.25f, 0.5f, m_pTransformCom)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ FSM_HILI_ALERT, FSM_HILI_RUN, 
																FSM_HILI_RUNATTACK_START, FSM_HILI_RUNATTACK, FSM_HILI_RUNATTACK_END,
																FSM_HILI_HIT_L, FSM_HILI_HIT_H, FSM_HILI_HIT_THROW_START, FSM_HILI_HIT_THROW, 
																FSM_HILI_HIT_THROW_STAND, FSM_HILI_HIT_THROW_DIE,
																FSM_HILI_DIE_1, FSM_HILI_DIE_2});

	/* 츄츄종류 */
	if (static_cast<HILI_TYPE>(ObjDesc.iObjectType) == HILI_NORMAL)
		m_iIdlePattern = 1;
	else if (static_cast<HILI_TYPE>(ObjDesc.iObjectType) == HILI_DANCE)
		m_iIdlePattern = 2;
	else if (static_cast<HILI_TYPE>(ObjDesc.iObjectType) == HILI_SIT)
		m_iIdlePattern = 3;
	else if (static_cast<HILI_TYPE>(ObjDesc.iObjectType) == HILI_SLEEP)
		m_iIdlePattern = 4;


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
	m_pHpBar->Set_OwnerInfo(TEXT("LV.19"), TEXT(""), 0);

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_strStatusTag = Get_ParticleTag(L"Hili_Normal");

	return S_OK;
}

HRESULT CHili_Normal::PreviousTick(_float fTimeDelta)
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
				m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
			}

			if (bPerification == TRUE && m_bInitPerification == FALSE)
				m_Parts[PART_WEAPON].pPartySlot->Set_Petrification();
			else if (bPerification == TRUE && m_bInitPerification == TRUE)
				m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);

			CHili_Normal::Set_Damage(fDamage, eElement);

			if (eReceiveState != FSM_OBJECT_END && m_fHp > 0.f)
			{
				if (static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState()) == FSM_HILI_RUNATTACK_END)
				{
					m_Parts[PART_BODY].pPartySlot->Set_RootTranslation(m_Parts[PART_BODY].pPartySlot->Get_PrevRootTranslation());
				}
				if (bPerification == FALSE)
				{
					m_pFiniteStateMachineCom->Change_ForceState(CHili_Normal::Get_State(eReceiveState));
				}

				CModel* pModel = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot->Find_Component(TEXT("Com_Model")));
				m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);
			}
		}

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

				Set_Destroy(TRUE);
				m_pGameInstance->Add_InteractionCount(L"Hili_Normal");
				m_pGameInstance->Add_InteractionCount(L"BattleBGM", -1);
			}
		}

		FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		if (m_fHp <= 0.f)
		{
			if (eState == FSM_HILI_HIT_H || eState == FSM_HILI_HIT_L
				|| eState == FSM_HILI_HIT_THROW_START || eState == FSM_HILI_HIT_THROW
				|| eState == FSM_HILI_HIT_THROW_STAND || eState == FSM_HILI_HIT_THROW_DIE)
			{
				m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
				m_Parts[PART_WEAPON].pPartySlot->Release_ColliderID();
				m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_DIE_2);
			}
			else
			{
				m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
				m_Parts[PART_WEAPON].pPartySlot->Release_ColliderID();
				m_pFiniteStateMachineCom->Change_ForceState(FSM_HILI_DIE_2);
			}
			return S_OK;
		}

		m_pHpBar->PreviousTick(fTimeDelta);
		m_pElement_OHD->PreviousTick(fTimeDelta);

		m_bInitPerification = bPerification;
	}

	return S_OK;
}

HRESULT CHili_Normal::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CHili_Normal::Pattern(fTimeDelta)))
			return E_FAIL;

		CHili_Normal::Collision_State();

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}

		m_pHpBar->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f, 0.f);
		m_pHpBar->Tick(fTimeDelta);
		_float2 vPos = m_pHpBar->Get_WindowPos();
		m_pElement_OHD->Update_Matrix(vPos.x, vPos.y + 45.f , 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);

		m_pController->Apply_Gravity(fTimeDelta);
	}

	return S_OK;
}

HRESULT CHili_Normal::LateTick(_float fTimeDelta)
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

HRESULT CHili_Normal::Render()
{
	return S_OK;
}

CHili_Normal* CHili_Normal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHili_Normal* pInstance = new CHili_Normal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CHili_Normal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHili_Normal::Clone(void* pArg)
{
	CHili_Normal* pInstance = new CHili_Normal(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CHili_Normal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHili_Normal::Free()
{
	__super::Free();
	
	if (m_bCloned == true)
	{
		Safe_Release(m_pHpBar);
		Safe_Release(m_pElement_OHD);
	}

	for (auto& pPart : m_Parts)
	{
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();
}