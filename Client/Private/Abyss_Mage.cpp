#include "stdafx.h"
#include "Abyss_Mage.h"
#include "Part_Body.h"
#include "Part_Weapon.h"

#include "GameInstance.h"
#include "Player.h"
#include "Skill_Abyss_Mage_Shield.h"

#include "Body_Abyss_Mage.h"

//UI
#include "Hp_Bar.h"
#include "UI_Manager.h"
#include "Page_Main.h"

#include "Effect.h"

CAbyss_Mage::CAbyss_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CCreature(pDevice, pContext)
{
}

CAbyss_Mage::CAbyss_Mage(const CAbyss_Mage& rhs)
	: CCreature(rhs)
{
}

HRESULT CAbyss_Mage::Add_Parts(void* pArg)
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

	//pPartObject = CPart_Weapon::Create_Weapon(WEAPON_STAFF_ELEC, tWeaponDesc);

	if (m_eCreatureID == MONSTER_ABYSS_MAGE_ELEC)
		pPartObject = CPart_Weapon::Create_Weapon(WEAPON_STAFF_ELEC, tWeaponDesc);
	else if(m_eCreatureID == MONSTER_ABYSS_MAGE_ICE)
		pPartObject = CPart_Weapon::Create_Weapon(WEAPON_STAFF_ICE, tWeaponDesc);
	else if(m_eCreatureID == MONSTER_ABYSS_MAGE_WATER)
		pPartObject = CPart_Weapon::Create_Weapon(WEAPON_STAFF_WATER, tWeaponDesc);
	else if(m_eCreatureID == MONSTER_ABYSS_MAGE_FIRE)
		pPartObject = CPart_Weapon::Create_Weapon(WEAPON_STAFF_FIRE, tWeaponDesc);

	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CAbyss_Mage::Pattern(_float fTimeDelta)
{
	// 현재 내 상태 가져오기
	FSM_ABYSS_MAGE_STATE m_eCurrentState = static_cast<FSM_ABYSS_MAGE_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	// 내 정보들 구해놓기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vSelfRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	// 플레이어 정보 구해놓기
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

	// 플레이어를 인지하지 못한 상태일때 플레이어가 근처에 온다면? 플레이어 인지
	if (m_bIsEnemyDetected == FALSE)
	{
		if (VerifyIsPlayerNearby(9.f))
			m_bIsEnemyDetected = TRUE;
	}
	else
	{
		if (m_eCurrentState != FSM_ABYSS_MAGE_STANDBY_DIZZY
			|| m_eCurrentState != FSM_ABYSS_MAGE_GATHER_BS
			|| m_eCurrentState != FSM_ABYSS_MAGE_GATHER_LOOP
			|| m_eCurrentState != FSM_ABYSS_MAGE_GATHER_AS)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 16.f);
		}
	}

	// 원소 보호막이 만약 깨졌다면
	if (m_fElementShield_Hp <= 0.f && m_bIsElementShieldActive == TRUE)
	{
		m_fElementShield_Hp = 0.f;
		m_bIsElementShieldActive = FALSE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_GETUP);
		cout << "심메 보호막 부서짐" << endl;

		m_pGameInstance->Release_StatusEffect(SE_ELEMENT_SHILED, m_strStatusTag);
	}
	
	switch (m_eCurrentState)
	{
	case Client::FSM_ABYSS_MAGE_THINK:
	{
		m_bIsFirstFrame = TRUE;

		if (m_bIsEnemyDetected == TRUE)
		{
			if (m_bIsAttacking == FALSE)
			{
				if (m_PrevPatternWasAttack == TRUE)
				{
					m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_STANDBY_FIGHT);
				}
				else
				{
					Identify_Abyss_Mage_AttackType();
				}
				break;
			}
			else
			{
				Abyss_Mage_Attack_Pattern();
				break;
			}
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_BLINK_AS:
	{
		if (m_bIsFirstFrame)
		{
			m_bIsFirstFrame = FALSE;

			_vector vRandomPos = m_pGameInstance->Get_RandomPos_InCircle(m_vInitPos, 6.f, vSelfPos);

			_float4 fRandomPos = {};
			XMStoreFloat4(&fRandomPos, vRandomPos);
			m_pController->Set_Position(fRandomPos);
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vRandomPos);

			CSkillObj::SKILLOBJ_DESC tLightningDesc = {};
			tLightningDesc.vScale = XMVectorSet(1.5f, 0.7f, 1.5f, 0.f);
			tLightningDesc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tLightningDesc.vSpawnPos.m128_f32[1] += 1.5f;
			tLightningDesc.fLifeTime = 1.f;

			tLightningDesc.vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
				return E_FAIL;

			//tLightningDesc.vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			//	return E_FAIL;

			//tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			//	return E_FAIL;

			//tLightningDesc.vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			//	return E_FAIL;

			//tLightningDesc.vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
			//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			//	return E_FAIL;
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_BLINK_BS:
	{
		m_fPatternTimer += fTimeDelta;
	
		if (m_fPatternTimer >= 4.f)
		{
			m_fPatternTimer = 0.f;

			m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_BLINK_AS);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_STANDBY_BRAINWASHING:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_DROWN:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_FALLING_DIE:
		break;
	case Client::FSM_ABYSS_MAGE_GATHER_AS:
		m_bIsGatherSkill = FALSE;
		break;
	case Client::FSM_ABYSS_MAGE_GATHER_BS:
		break;
	case Client::FSM_ABYSS_MAGE_GATHER_LOOP:
		m_bIsGatherSkill = TRUE;
	break;
	case Client::FSM_ABYSS_MAGE_GETUP:
		break;
	case Client::FSM_ABYSS_MAGE_HIT_HITHEAVY:
		break;
	case Client::FSM_ABYSS_MAGE_HIT_HITLIGHT:
		break;
	case Client::FSM_ABYSS_MAGE_HIT_HITSHAKE:
		break;
	case Client::FSM_ABYSS_MAGE_HIT_HITTHROW:
		break;
	case Client::FSM_ABYSS_MAGE_HIT_HITTHROWDROP:
		break;
	case Client::FSM_ABYSS_MAGE_HIT_HITTHROWTOGROUND:
		break;
	case Client::FSM_ABYSS_MAGE_ATTACK_NORMAL_ATTACK:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_RANDOMACTION_2:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_RANDOMACTION_3:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_RANDOMACTION_1:
		break;
	case Client::FSM_ABYSS_MAGE_ATTACK_SPECIAL_ATTACK:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_DIZZY:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_FIGHT:
		m_bIsAttacking = FALSE;
		m_PrevPatternWasAttack = FALSE;
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_FREE:
		if (m_bIsEnemyDetected == TRUE)
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_WARNING);
		break;
	case Client::FSM_ABYSS_MAGE_WALK:
		break;
	case Client::FSM_ABYSS_MAGE_WARNING:
		break;
	case Client::FSM_ABYSS_MAGE_DIE:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_LOOP:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_AS:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_BS:
		break;
	case Client::FSM_ABYSS_MAGE_END:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CAbyss_Mage::Pattern_Gather(_float fTimeDelta)
{
	switch (m_eCreatureID)
	{
	case Client::MONSTER_ABYSS_MAGE_ELEC:
	{
		
	}
	break;
	case Client::MONSTER_ABYSS_MAGE_ICE:
	{
		m_fRandomHailTimer += fTimeDelta;
		if (m_fRandomHailTimer >= 1.f)
		{
			m_fRandomHailTimer = 0.f;

			_float fRandomAngle = m_pGameInstance->Get_Float_Random(0.f, XM_2PI);
			_float fRandomRadius = m_pGameInstance->Get_Float_Random(0.f, 10.f);

			// 랜덤한 위치를 계산
			_vector vRandomHailPos = XMVectorSet
			(
				fRandomRadius * cos(fRandomAngle),	// X 축
				0.f,								// Y 축
				fRandomRadius * sin(fRandomAngle),  // Z 축
				0.f							        // W
			);

			// m_vFieldCenterPos에 이 랜덤한 위치를 더하여 최종 월드 좌표
			vRandomHailPos = XMVectorAdd(m_vInitPos, vRandomHailPos);

			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_ABYSS_ICECONE;
			desc.fLifeTime = 2.f;
			desc.vSpawnPos = vRandomHailPos;
			desc.vScale = { 0.2f, 0.2f, 0.2f, 0.f };

			// vRandomHailPos 에 고드름 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_IceCone", L"Layer_Skill", &desc)))
				return E_FAIL;
		}
	}
	break;
	case Client::MONSTER_ABYSS_MAGE_WATER:
	{

	}
	break;
	case Client::MONSTER_ABYSS_MAGE_FIRE:
	{

	}
	break;
	}

	return S_OK;
}

/// <summary>
/// 어떤 타입의 공격을 할것인지
/// </summary>
/// <returns></returns>
HRESULT CAbyss_Mage::Identify_Abyss_Mage_AttackType()
{
	_int iRandomAttack = 0;

	if (m_bIsElementShieldActive == FALSE)
	{
		iRandomAttack = 4;
	}
	else
	{
		rotate(PatternOrder.begin(), PatternOrder.begin() + 1, PatternOrder.end());
		iRandomAttack = PatternOrder[0];
	}

	switch (iRandomAttack)
	{
	case 1:
		m_eAttack_Type = ABYSS_MAGE_ATTACK_NORMAL;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_ATTACK_NORMAL_ATTACK);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		//cout << "심메 패턴 : 평타" << endl;
		break;
	case 2:
		m_eAttack_Type = ABYSS_MAGE_ATTACK_SPECIAL;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_ATTACK_SPECIAL_ATTACK);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		//cout << "심메 패턴 : 스킬" << endl;
		break;
	case 3:
		m_eAttack_Type = ABYSS_MAGE_ATTACK_BLINK;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_BLINK_BS);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		cout << "심메 패턴 : 순간이동" << endl;
		break;
	case 4:
		m_eAttack_Type = ABYSS_MAGE_ATTACK_GATHER;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_GATHER_BS);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		cout << "심메 패턴 : 쉴드 없을때 발작" << endl;
		break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

/// <summary>
/// 공격 제어 함수
/// </summary>
/// <returns></returns>
HRESULT CAbyss_Mage::Abyss_Mage_Attack_Pattern()
{
	switch (m_eAttack_Type)
	{
	case Client::CAbyss_Mage::ABYSS_MAGE_ATTACK_NORMAL:
		m_bIsAttacking = FALSE;
		break;
	case Client::CAbyss_Mage::ABYSS_MAGE_ATTACK_SPECIAL:
		m_bIsAttacking = FALSE;
		break;
	case Client::CAbyss_Mage::ABYSS_MAGE_ATTACK_BLINK:
		m_bIsAttacking = FALSE;
		break;
	case Client::CAbyss_Mage::ABYSS_MAGE_ATTACK_GATHER:
		m_bIsAttacking = FALSE;
		break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

/// <summary> 
/// /// 플레이어가 왼쪽에 있는지, 오른쪽에 있는지 판별하는 함수, // True면 왼쪽, False 면 오른쪽
/// </summary>
/// <returns></returns>
_bool CAbyss_Mage::VerifyIsPlayerOnLeft()
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
_bool CAbyss_Mage::VerifyIsPlayerNearby(_float _fDistance)
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
_bool CAbyss_Mage::VerifyIsPlayerInFront()
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

		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		_float fDotResult = XMVectorGetX(XMVector3Dot(vSelfLook, vDirToPlayer));

		return fDotResult > 0 ? TRUE : FALSE;
	}
}

void CAbyss_Mage::Collision_State()
{
	//if (m_fHp <= 0.f)
	//	return;

	//CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_WORLD, TEXT("Layer_Player")));

	//FSM_ABYSS_MAGE_STATE eState = static_cast<FSM_ABYSS_MAGE_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	//_int iWeaponID = -1;
	//vector<CGameObject*> vecPlayer;

	//if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	//{
	//	if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
	//	{
	//		m_bHitPlayer = FALSE;
	//		_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
	//		m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
	//	}
	//}
	//else /* Player 전체 */
	//{
	//	if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
	//	{
	//		m_bHitPlayer = FALSE;

	//		for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
	//		{
	//			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
	//			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
	//		}
	//	}
	//}

	//if ((eState >= FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE && eState <= FSM_BRUTE_AXE_ATTACK_1) || eState == FSM_BRUTE_WHIRL_LOOP_WITH_AXE)
	//{
	//	iWeaponID = static_cast<CPart_Body*>(m_Parts[PART_WEAPON].pPartySlot)->Get_CollisionID();
	//	vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);
	//}
	//else
	//	m_bHitPlayer = TRUE;

	//if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	//{
	//	/* 플레이어가 충돌했을 때 */
	//	if (m_bHitPlayer == FALSE && vecPlayer.size() != 0)
	//	{
	//		m_bHitPlayer = TRUE;

	//		_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
	//		m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);

	//		pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(521, 1021));
	//	}
	//}
	//else /* Player 전체 */
	//{
	//	/* 플레이어가 충돌했을 때 */
	//	if (m_bHitPlayer == FALSE && vecPlayer.size() != 0)
	//	{
	//		m_bHitPlayer = TRUE;

	//		for (auto& pPlayerBody : vecPlayer)
	//		{
	//			PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
	//			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID(eIndex);
	//			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);

	//			pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(521, 1021));
	//		}
	//	}
	//}
}

void CAbyss_Mage::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CAbyss_Mage::Set_Damage(fDamage * 2.f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CAbyss_Mage::Set_Damage(fDamage * 1.5f, ELEMENT_FIRE);
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

void CAbyss_Mage::Set_ElementUI(ELEMENT eElement)
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

void CAbyss_Mage::Set_Damage(_float fDamage, ELEMENT eElement)
{
	if (m_Parts[PART_BODY].pPartySlot->Get_RenderEnable() == TRUE)
	{
		if (m_bIsElementShieldActive == TRUE)
		{
			m_fElementShield_Hp -= fDamage;
			if (m_fElementShield_Hp < 0.f)
				m_fElementShield_Hp = 0.f;
		}
		else
		{
			m_fHp -= fDamage;
			if (m_fHp < 0.f)
				m_fHp = 0.f;
			m_pHpBar->Set_CurrentHp(m_fHp);
		}

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
}

void CAbyss_Mage::Set_ElementShield_Active()
{
	if (m_bIsElementShieldActive == FALSE)
	{
		m_fElementShield_Hp = m_fElementShield_MaxHp;
		cout << "심메 보호막 생성" << endl;
		
		CSkill_Abyss_Mage_Shield::ABYSS_MAGE_SHILED_DESC tDesc = {};
		tDesc.strTag = m_strStatusTag;
		tDesc.tSkillDesc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		tDesc.tSkillDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_Abyss_Mage_Shield", L"Layer_Skill", &tDesc)))
			return;

		m_bIsElementShieldActive = TRUE;
	}
}

void CAbyss_Mage::Set_DeadState()
{
	Clear_Target();
	Set_Destroy(TRUE);

	m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);
	m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
	m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
	m_Parts[PART_WEAPON].pPartySlot->Release_ColliderID();
	m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_DIE);

	m_pGameInstance->Release_StatusEffect(SE_ELEMENT_SHILED, m_strStatusTag);
}

HRESULT CAbyss_Mage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAbyss_Mage::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);
	m_vInitPos = ObjDesc.vInitPos;
	m_eCreatureID = static_cast<CREATUREID>(ObjDesc.iObjectType);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_Controller(2.0f, 0.5f, 0.0f, m_pTransformCom)))
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
	
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_GETUP);
	m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_STANDBY_FREE);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_STANDBY_BRAINWASHING);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_WALK);

#pragma region UI
	COrtho::ORTHO_DESC tOrthoDesc;
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
	tOrthoDesc.fPosition = { static_cast<_float>(0.f - g_iWinSizeX / 2), static_cast<_float>(-(0.f - g_iWinSizeY / 2)) };
	tOrthoDesc.fSize = { 100.f, 5.f };
	tOrthoDesc.ePage = PAGE_MAIN;

	m_pHpBar = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));
	m_pHpBar->Set_MaxHp(m_fHp);
	m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_MONSTER);
	m_pHpBar->Set_CurrentHp(m_fHp);
	m_pHpBar->Set_OwnerInfo(TEXT("LV.19"), TEXT(""), 0);
#pragma endregion

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_strStatusTag = Get_ParticleTag(L"Abyss_Mage");

	Set_ElementShield_Active();

	return S_OK;
}

HRESULT CAbyss_Mage::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		if (m_Parts[PART_BODY].pPartySlot->Get_RenderEnable() == TRUE)
		{
			// 절두체 안에있다면, m_MonsterTarget에 자기자신을 추가
			_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CombinedMatrix();
			if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&CombinedWorldMatrix).r[3], 5.f) && m_fHp > 0.f)
				Add_Target(this);
		}

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

			CAbyss_Mage::Set_Damage(fDamage, eElement);

			if (eReceiveState != FSM_OBJECT_END && m_fHp > 0.f)		// 피격중이고, 체력이 0 이상이라면
			{
				//if (static_cast<FSM_HILI_STATE>(m_pFiniteStateMachineCom->Get_CurrentState()) == FSM_HILI_RUNATTACK_END)
				//{
				//	m_Parts[PART_BODY].pPartySlot->Set_RootTranslation(m_Parts[PART_BODY].pPartySlot->Get_PrevRootTranslation());
				//}

				//if (bPerification == FALSE)		// 석화중이 아니라면
				//	m_pFiniteStateMachineCom->Change_ForceState(CAbyss_Mage::Get_State(eReceiveState));	// 자기 자신의 상태를 현재 공통피격상태에 따라 변경

				m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);
			}
		}

		// 바디가 죽었다면 
		if (m_Parts[PART_BODY].pPartySlot->Get_Dead() == TRUE)
		{
			if (m_bDestroy == FALSE)
			{
				m_pGameInstance->Add_InteractionCount(L"Abyss_Mage");
				Set_Destroy(TRUE);
			}
		}

		// 체력이 음수라면
		if (m_fHp <= 0.f)
		{
			m_Parts[PART_WEAPON].pPartySlot->Set_Petrification(FALSE);
			m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
			m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
			m_Parts[PART_WEAPON].pPartySlot->Release_ColliderID();
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ABYSS_MAGE_DIE);
			
			return S_OK;
		}

		if (m_Parts[PART_BODY].pPartySlot->Get_RenderEnable() == TRUE)
		{
			m_pHpBar->PreviousTick(fTimeDelta);
			m_pElement_OHD->PreviousTick(fTimeDelta);
		}

		m_bInitPerification = bPerification;
	}

	return S_OK;
}

HRESULT CAbyss_Mage::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('6'))
	{
		m_fElementShield_Hp -= 10000.f;
	}

	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CAbyss_Mage::Pattern(fTimeDelta)))
			return E_FAIL;

		if (m_bIsGatherSkill == TRUE)
		{
			if (FAILED(CAbyss_Mage::Pattern_Gather(fTimeDelta)))
				return E_FAIL;
		}

		Collision_State();

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
			pPart.pPartySlot->Tick(fTimeDelta);
		
		if (m_Parts[PART_BODY].pPartySlot->Get_RenderEnable() == TRUE)
		{
			// 체력바	
			m_pHpBar->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f, 0.f);
			m_pHpBar->Tick(fTimeDelta);
			_float2 vPos = m_pHpBar->Get_WindowPos();
			m_pElement_OHD->Update_Matrix(vPos.x, vPos.y + 45.f, 30.f, 30.f);
			m_pElement_OHD->Tick(fTimeDelta);
		}

		if (m_fHp > 0.f)
			m_pController->Apply_Gravity(fTimeDelta);
	}

	return S_OK;
}

HRESULT CAbyss_Mage::LateTick(_float fTimeDelta)
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
		_vector vToPlayer = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_float fDistanceToPlayer = XMVector3Length(vToPlayer).m128_f32[0];
		if (fDistanceToPlayer <= 10.f)
		{
			if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0)
			{
				CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0));
				CPage_Main* m_pPage_Main = static_cast<CPage_Main*>(pUIManager->Get_UI_Page(PAGE_MAIN));

				m_pPage_Main->Register_On_MiniMap(m_pTransformCom, 0);
				if (m_Parts[PART_BODY].pPartySlot->Get_RenderEnable() == TRUE)
				{
					m_pHpBar->LateTick(fTimeDelta);
					m_pElement_OHD->LateTick(fTimeDelta);
				}
			}
		}
	}

	return S_OK;
}

HRESULT CAbyss_Mage::Render()
{
	return S_OK;
}

CAbyss_Mage* CAbyss_Mage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAbyss_Mage* pInstance = new CAbyss_Mage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CAbyss_Mage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAbyss_Mage::Clone(void* pArg)
{
	CAbyss_Mage* pInstance = new CAbyss_Mage(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CAbyss_Mage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAbyss_Mage::Free()
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