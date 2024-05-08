#include "stdafx.h"
#include "Effigy_Fire.h"

#include "Part_Body.h"
#include "Part_Status.h"
#include "Camera_Main.h"

#include "GameInstance.h"
#include "Hp_Bar.h"
#include "Skill_FireCore.h"

#include "Body_Effigy_Fire.h"
#include "SkillObj.h"

#include "Effect.h"
#include "Slash_Trail.h"
#include "Eff_Impact_Glow.h"

#include "SkillObj.h"
#include "BossChest.h"

#include "FX_EffigyFireExplode.h"

CEffigy_Fire::CEffigy_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CEffigy_Fire::CEffigy_Fire(const CEffigy_Fire& rhs)
	: CCreature(rhs)
{
}

HRESULT CEffigy_Fire::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = MONSTER_EFFIGY_FIRE;
	tBodyDesc.eType = CTY_MONSTER;

	CREATUREID eCreatureID = MONSTER_EFFIGY_FIRE;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	//pPartObject->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	tSlots = { };



	// Status 안만들었음!!! @@@@@@@@@@@@@@@@@@@@@@@@@!@@@@@@@@@@@@

	/* For. Part_Status */
	CPart_Status::PARTSTATUS_DESC tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	//pPartObject = CPart_Status::Create_Status(MONSTER_EFFIGY_FIRE, tStatusDesc);
	//if (pPartObject == nullptr)
	//	return E_FAIL;

	//tSlots.pPartySlot[0] = pPartObject;
	//m_Parts.push_back(tSlots);

	return S_OK;
}

/// <summary>
/// Effigy_Fire 의 패턴을 제어하는 함수
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CEffigy_Fire::Pattern(_float fTimeDelta)
{
	// 현재 내 상태 가져오기
	FSM_EFFIGY_STATE m_eCurrentState = static_cast<FSM_EFFIGY_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	//cout << m_eCurrentState << endl;
	
	// 내 정보들 구해놓기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

	// 플레이어 정보 구해놓기
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	_vector vPlayerPos;
	_vector vDirectionToPlayer;
	_float fDistance;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 1명만 온필드 */
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_vPlayerPosss = vPlayerPos;

		vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);
		fDistance = XMVector3Length(vDirectionToPlayer).m128_f32[0];
		vDirectionToPlayer.m128_f32[1] = 10.9661f;
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
			m_vPlayerPosss = vPlayerPos;

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
		m_vPlayerPosss = vPlayerPos;
		vDirectionToPlayer = vNearSetDir;
		fDistance = fNearDistance;
	}

	// 플레이어를 인지하지 못한 상태일때 플레이어가 근처에 온다면? 플레이어 인지
	if (m_bIsEnemyDetected == FALSE)
	{
		if (VerifyIsPlayerNearby(9.f))
			m_bIsEnemyDetected = TRUE;
	}

	// 불 보호막이 만약 깨졌다면
	if (m_fFireShield_Hp <= 0.f && m_bIsFireShieldActive == TRUE)
	{
		m_fFireShield_Hp = 0.f;
		m_bIsFireShieldActive = FALSE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_SHAKE_SHAKE);
		m_pGameInstance->PlaySoundW(L"EffigyFire_ShieldBreak.mp3");
	}
	
	// 현재 내 상태에 따라서 패턴 처리
	switch (m_eCurrentState)
	{
	case Client::FSM_EFFIGY_THINK:
	{
		if (m_bIsEnemyDetected == TRUE)		// 적을 인지한 상태라면
		{
			if (m_bIsAttacking == FALSE)		// 공격중이 아니라면
			{
				if (m_PrevPatternWasAttack == TRUE)	// 이전 패턴이 공격 패턴이었다면
				{
					m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_REBRON_CUBES_REAPPEAR);
				}
				else		// 이전 패턴이 공격 패턴이 아니었다면
				{
					CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
					pEffigyBody->Set_HasMoved(false);

					Identify_Effigy_AttackType();
				}
				break;
			}
			else if (m_bIsAttacking == TRUE)	// 스윙 공격중이라면 패턴에 맞게 이어서 공격
			{
				Effigy_Attack_Pattern();
				break;
			}
		}
	}
	break;
	case Client::FSM_EFFIGY_STANDBY_FREE:
	{
		if (m_bIsEnemyDetected == TRUE)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_STANDBY_ALERT);
			m_bIsFireShieldActive = TRUE;
		}
	}
	break;
	case Client::FSM_EFFIGY_STANDBY_BS:
		break;
	case Client::FSM_EFFIGY_STANDBY_AS:
		break;
	case Client::FSM_EFFIGY_STANDBY_SOFT:
		break;
	case Client::FSM_EFFIGY_ATTACK_CLAP:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);
		}
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE_LOOP:
		break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE_AS:
		break;
	case Client::FSM_EFFIGY_ATTACK_RANGED_BS:
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);
		break;
	case Client::FSM_EFFIGY_ATTACK_RANGED_LOOP:
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);

		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 5.5f)
		{
			m_fPatternTimer = 0.f;
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_RANGED_AS);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_RANGED_AS:
		break;
	case Client::FSM_EFFIGY_MOVE_RUSHMOVE_LOOP:
	{
		if (m_bIsFirstFrame == TRUE)
		{
			m_fPatternTimer = 0.f;
			m_bIsFirstFrame = FALSE;
			m_PrevPatternWasAttack = FALSE;
			m_bIsAttacking = FALSE;
			m_vRandomPos = m_pGameInstance->Get_RandomPos_InCircle(m_vInitPos, 20.f, vSelfPos);
		}

		_vector vDirToTarget = XMVector3Normalize(m_vRandomPos - vSelfPos);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirToTarget);
		//m_pTransformCom->Go_Straight(fTimeDelta * 1.5f);
		m_pTransformCom->Move_To(m_vRandomPos, fTimeDelta * 1.5f, 0.1f);

		// 목표지점에 도달했는지 확인
		if (XMVectorGetX(XMVector3Length(vSelfPos - m_vRandomPos)) < 0.2f)
		{
			vSelfPos = m_vRandomPos;
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_THINK);
			m_bIsFirstFrame = TRUE;		// 첫 프레임에만 실행되도록 관리해주는 bool 변수 초기화

			CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
			pEffigyBody->Set_HasMoved(TRUE);
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_REBRON_CUBES_REAPPEAR);
		}
	}
	break;
	case Client::FSM_EFFIGY_REBRON_CUBES_REAPPEAR:
		m_bIsFirstFrame = TRUE;
		break;
	case Client::FSM_EFFIGY_STANDBY_LOOP:
		m_bIsAttacking = FALSE;
		break;
	case Client::FSM_EFFIGY_BLINK:
		break;
	case Client::FSM_EFFIGY_IGNITE_BS:
	{
		if (m_bIsFirstFrame == TRUE)
		{
			m_bIsFirstFrame = FALSE;

			_vector vDirSelfToCore2 = XMVector3Rotate(vSelfLook, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(120.f)));
			_vector vDirSelfToCore3 = XMVector3Rotate(vSelfLook, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-120.f)));

			_vector CorePos = vSelfPos + vSelfLook * 10.f;
			_vector CorePos2 = vSelfPos + vDirSelfToCore2 * 10.f;
			_vector CorePos3 = vSelfPos + vDirSelfToCore3 * 10.f;

			// =====================

			CSkill_FireCore::FIRECORE_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_FIRE_CORE;
			desc.vLinkPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			desc.vScale = { 1.3f, 1.3f, 1.3f, 0.f };
			//desc.vScale = { 0.007f, 0.007f, 0.007f, 0.f };

			desc.vSpawnPos = CorePos;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_FireCore", L"Layer_Skill_Core", &desc)))
				return E_FAIL;

			desc.vSpawnPos = CorePos2;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_FireCore", L"Layer_Skill_Core", &desc)))
				return E_FAIL;

			desc.vSpawnPos = CorePos3;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_FireCore", L"Layer_Skill_Core", &desc)))
				return E_FAIL;
		}
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_AS:		// 점화에 실패
	{
		// 다음 애니메이션 제어를 위한 설정
		CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
		pEffigyBody->Set_IsIgnite(FALSE);

		// 충전 크리스탈이 모두 파괴되었다 bool = FALSE
		m_bIsRechargeCrystal_Destroy = FALSE;
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_SUCCESS:	// 점화에 성공
	{
		// 다음 애니메이션 제어를 위한 설정
		CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
		pEffigyBody->Set_IsIgnite(FALSE);

		// 쉴드를 최대 체력으로
		m_fFireShield_Hp = m_fFireShield_Recharge_Rate_Max;	

		// 쉴드 활성화
		m_bIsFireShieldActive = TRUE;
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_LOOP:
	{
		m_fFireShield_Recharge_Rate_Current += fTimeDelta * 300.f;
		cout << "FireShield_Recharge_Rate : " << m_fFireShield_Recharge_Rate_Current << endl;

		// 코어의 레이어가 모두 사라졌다면?
		CSkill_FireCore* pFireCore = static_cast<CSkill_FireCore*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Skill_Core"), 0));
		if (pFireCore == nullptr)
			m_bIsRechargeCrystal_Destroy = TRUE;

		// 충전 크리스탈이 모두 파괴되었다면
		if (m_bIsRechargeCrystal_Destroy == TRUE)		
		{
			m_bIsRechargeCrystal_Destroy = FALSE;

			// 다음 애니메이션 제어를 위해 점화에 성공했는지 실패했는지
			CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
			pEffigyBody->Set_IsIgniteSuccess(FALSE);

			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_IGNITE_BS02);
		}

		// 재충전 수치가 재충전 최대치를 넘었다면
		if (m_fFireShield_Recharge_Rate_Current >= m_fFireShield_Recharge_Rate_Max)
		{
			m_fFireShield_Recharge_Rate_Current = 0.f;                                                                                                                                                                                       


			CSkill_FireCore* pFireCore = static_cast<CSkill_FireCore*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Skill_Core"), 0));
			CSkill_FireCore* pFireCore2 = static_cast<CSkill_FireCore*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Skill_Core"), 1));
			CSkill_FireCore* pFireCore3 = static_cast<CSkill_FireCore*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Skill_Core"), 2));

			if (pFireCore != nullptr)
				pFireCore->SetIgniteSuccess(TRUE);
			if (pFireCore2 != nullptr)
				pFireCore2->SetIgniteSuccess(TRUE);
			if (pFireCore3 != nullptr)
				pFireCore3->SetIgniteSuccess(TRUE);

			// 다음 애니메이션 제어를 위해 점화에 성공했는지 실패했는지
			CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
			pEffigyBody->Set_IsIgniteSuccess(TRUE);

			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_IGNITE_BS02);
		}
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_BS02:
		m_bIsFirstFrame = TRUE;
		break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE2_AS:
	{
		CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
		pEffigyBody->Set_IsIgnite(TRUE);
	}
	break;	
	case Client::FSM_EFFIGY_ATTACK_PILLAR:
	{
		if (m_bIsFirstFrame == TRUE)
		{
			m_bIsFirstFrame = FALSE;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);
			// 기둥 패턴 ON
		}
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE2_LOOP:
	{
		m_bIsFirstFrame = TRUE;
		m_bIsPillarPattern = TRUE;
	}
	break;
	case Client::FSM_EFFIGY_SHAKE_SHAKE:
	{
		m_bIsFirstFrame = TRUE;
		m_PrevPatternWasAttack = FALSE;
		m_iAttackIndex = 0;
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_BS:
		// 플레이어 쪽을 바라보도록
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
		}
		break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_AIMING:
	{
		m_fPatternTimer = 0.f;
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);

		_float fAngleDifference = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vSelfLook, vDirectionToPlayer)));
		if (fAngleDifference <= 10.f)	// 플레이어를 거의 정확히 바라봤다면
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_ROLLING_LOOP);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_LOOP:
	{
		if (m_eRollingSoundChannel == MAXCHANNEL)
		{
			m_eRollingSoundChannel = m_pGameInstance->Get_NextChannel();
			m_pGameInstance->PlayBGM(L"EffigyFire_Rolling.mp3", 1.f, m_eRollingSoundChannel);
		}

		CImpact_Glow::IMPACT_DESC descc = {};
		descc.eEffectID = EFFECT_IMPACT_SINGLE;
		_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vSelfRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_int	iRotationRand = {};
		_float	fHeight = m_pGameInstance->Get_Float_Random(0.f, 1.5f);
		descc.pParent = (CGameObject*)this;
		vSelfPos.m128_f32[1] += fHeight;
		vSelfPos += (vSelfLook * 2.f);
		vSelfPos += vSelfRight * 1.0f;
		descc.vSpawnPos = vSelfPos;
		descc.vScale = { 1.f, 1.5f, 1.f, 1.f };
		descc.eELEMENT = ELEMENT_FIRE;
		iRotationRand = m_pGameInstance->Get_Int_Random(-80,-10);
		descc.iRotation = iRotationRand;
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
			return E_FAIL;

		vSelfPos -= vSelfRight * 2.f;
		descc.vSpawnPos = vSelfPos;
		iRotationRand = m_pGameInstance->Get_Int_Random(10, 80);
		descc.iRotation = iRotationRand;


		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
			return E_FAIL;

		if (VerifyIsPlayerInFront())
		{
			m_fLerpSpeed = 15.f;
			m_bPlayerInFront = TRUE;
		}
		else
		{
			if (VerifyIsPlayerNearby(10.f) == TRUE)
			{
				m_fLerpSpeed = 11.f;	
			}
			else
			{
				if (m_iRollingIndex >= 4)
				{
					m_pGameInstance->StopSound(m_eRollingSoundChannel);
					m_eRollingSoundChannel = MAXCHANNEL;

					m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_ROLLING_AS);
					m_iRollingIndex = 0;
				}

				m_fLerpSpeed = 13.f;

				// 플레이어가 앞에 있다가 뒤로 이동할 때만 횟수를 증가
				if (m_bPlayerInFront == TRUE)
				{
					m_iRollingIndex++;
					m_bPlayerInFront = FALSE;
				}
			}
		}

		// 플레이어를 바라보기
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, m_fLerpSpeed);
		m_pTransformCom->Go_Straight(fTimeDelta * 3.f);
	}	
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_AS:
		break;
	case Client::FSM_EFFIGY_ATTACK_TRANSFORM_1:
	{
		if (m_bIsFirstFrame)
		{
			m_bIsFirstFrame = FALSE;

			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 20.f);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_TRANSFORM_2:
	{
		if (m_bIsFirstFrame)
		{
			m_bIsFirstFrame = FALSE;
				
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 20.f);

		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_TRANSFORM_3:
	{
		if (m_bIsFirstFrame)
		{
			m_bIsFirstFrame = FALSE;
			
			vPlayerPos.m128_f32[1] = 10.9661f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
		}
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSE:
		break;
	case Client::FSM_EFFIGY_ATTACK_PUNCH:
	{
		// 플레이어 쪽을 바라보도록
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);
		}

		// 땅 쾅쾅쾅 패턴
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SNAKE_BS:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.1f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SNAKE_LOOP:
		break;
	case Client::FSM_EFFIGY_ATTACK_SNAKE_AS:
		break;
	case Client::FSM_EFFIGY_ATTACK_SELF_EXPLOSION_BS:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);
		// 폭발 이펙트 필요 (순간이동)
		break;
	case Client::FSM_EFFIGY_ATTACK_SELF_EXPLOSION_LOOP:
	{
		if (m_fSelfExplosionTimer < 10.f)
			m_fSelfExplosionTimer += fTimeDelta;
		CImpact_Glow::IMPACT_DESC descc = {};
		descc.eEffectID = EFFECT_IMPACT_SINGLE;
		_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vSelfPos.m128_f32[1] += 1.4f;
		descc.vSpawnPos = vSelfPos;
		descc.vScale = { 0.6f * m_fSelfExplosionTimer, 0.9f * m_fSelfExplosionTimer, 1.f, 1.f };
		descc.iRotAxisY = m_pGameInstance->Get_Int_Random(0, 360);

		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
			return E_FAIL;
		break;
	}
	case Client::FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE:
		m_fSelfExplosionTimer = 0.f;
		m_bIsAttacking = FALSE;
		break;
	case Client::FSM_EFFIGY_STANDBY_ALERT:
		break;
	case Client::FSM_EFFIGY_HIT:
		break;
	case Client::FSM_EFFIGY_TRASH:
		break;
	case Client::FSM_EFFIGY_TRASH2:
		break;
	case Client::FSM_EFFIGY_END:
		break;
	}
	
	return S_OK;
}

HRESULT CEffigy_Fire::Pattern_Pillar(_float fTimeDelta)
{
	m_fPillarTimer += fTimeDelta;
	m_fPlayerPillarTimer += fTimeDelta;
	m_fRandomPillarTimer += fTimeDelta;

	if (m_fPillarTimer >= 0.f)
	{
		m_fPillarTimer = 0.f;
		m_bIsPillarPattern = FALSE;
	}

	if (m_fPlayerPillarTimer >= 3.f)
	{
		m_fPlayerPillarTimer = 0.f;

		// 플레이어 정보 구해놓기
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));

		// 플레이어 FSM 가져오기
		CFiniteStateMachine* pPlayerFSM
			= static_cast<CFiniteStateMachine*>((pPlayer->Get_PlayerInfo(iIndex).pFiniteStateMachineCom));

		// 플레이어 현재 FSM 상태 가져오기
		FSM_STATE m_eCurrentPlayerState = static_cast<FSM_STATE>(pPlayerFSM->Get_CurrentState());

		_float fDistanceValue = 0.f;
		switch (m_eCurrentPlayerState)		// 플레이어의 현재 상태에서 따라서 미리 나오는 부분의 위치를 조정
		{
		case Client::FSM_WALK:
			fDistanceValue = 2.5f;
			break;
		case Client::FSM_RUN:
			fDistanceValue = 3.5f;
			break;
		case Client::FSM_SPLINT:
			fDistanceValue = 5.5f;
			break;
		default:
			fDistanceValue = 0.f;
			break;
		}

		_vector vPlayerPillarPos = vPlayerPos + vPlayerLook * fDistanceValue;
		vPlayerPillarPos.m128_f32[1] = 10.9661f;

		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_FIRE_PILLAR;
		desc.fLifeTime = 3.5f;
		desc.vSpawnPos = vPlayerPillarPos;
		desc.vScale = { 0.007f, 0.007f, 0.007f, 0.f };

		// vPlayerPillarPos 에 기둥 생성
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_FirePillar", L"Layer_Skill", &desc)))
			return E_FAIL;
	}

	if (m_fRandomPillarTimer >= 1.f)
	{
		m_fRandomPillarTimer = 0.f;

		_float fRandomAngle = m_pGameInstance->Get_Float_Random(0.f, XM_2PI);
		_float fRandomRadius = m_pGameInstance->Get_Float_Random(0.f, 20.f);

		// 랜덤한 위치를 계산
		_vector vRandomPillarPos = XMVectorSet
		(
			fRandomRadius * cos(fRandomAngle),	// X 축
			0.f,								// Y 축
			fRandomRadius * sin(fRandomAngle),  // Z 축
			0.f							        // W
		);

		// m_vFieldCenterPos에 이 랜덤한 위치를 더하여 최종 월드 좌표
		vRandomPillarPos = XMVectorAdd(m_vInitPos, vRandomPillarPos);

		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_FIRE_PILLAR;
		desc.fLifeTime = 3.5f;
		desc.vSpawnPos = vRandomPillarPos;
		desc.vScale = { 0.007f, 0.007f, 0.007f, 0.f };

		// vRandomPillarPos 에 고드름 생성
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_FirePillar", L"Layer_Skill", &desc)))
			return E_FAIL;
	}

	return S_OK;
}

/// <summary>
/// 어떤 타입의 공격을 할것인지 판별
/// </summary>
/// <returns></returns>
HRESULT CEffigy_Fire::Identify_Effigy_AttackType()
{
	_int iRandomAttack = 0;

	if (m_bIsFireShieldActive == FALSE)
	{
		iRandomAttack = 8;
	}
	else
	{
		if (VerifyIsPlayerNearby(7.f) == TRUE)
		{
			rotate(nearPatternOrder.begin(), nearPatternOrder.begin() + 1, nearPatternOrder.end());
			iRandomAttack = nearPatternOrder[0];
		}
		else if (XMVectorGetX(XMVector3Length(m_vPlayerPos - m_vInitPos)) < 3.f && m_fHp <= m_fMaxHp / 2.f)
		{
			iRandomAttack = 4;
		}
		else
		{
			rotate(farPatternOrder.begin(), farPatternOrder.begin() + 1, farPatternOrder.end());
			iRandomAttack = farPatternOrder[0];
		}
	}

	if(m_bPatternControl == TRUE)
		iRandomAttack = m_iPattern;

	switch (iRandomAttack)
	{
	case 1:		 // 박수
		m_eAttack_Type = EFFIGY_ATTACK_CLAP;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_CLAP);
		m_bIsAttacking = TRUE;
		break;
	case 2:		// 변신
		m_eAttack_Type = EFFIGY_ATTACK_TRANSFORM;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_TRANSFORM_1);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		m_iAttackIndex++;
		break;
	case 3:		// 펀치
		m_eAttack_Type = EFFIGY_ATTACK_PUNCH;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_PUNCH);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		break;
	case 4:		// 자폭
		m_eAttack_Type = EFFIGY_ATTACK_SELF_EXPLOSION;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_BLINK);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		m_iAttackIndex++;
		break;
	case 5:		// 구르기
		m_eAttack_Type = EFFIGY_ATTACK_ROLLING;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_ROLLING_BS);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		break;
	case 6:		// 원거리 큐브
		m_eAttack_Type = EFFIGY_ATTACK_RANGED_CUBE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_RANGED_BS);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		break;
	case 7:		// 뱀 원거리
		m_eAttack_Type = EFFIGY_ATTACK_SNAKE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_SNAKE_BS);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		break;
	case 8:		// 기둥 패턴
		m_eAttack_Type = EFFIGY_ATTACK_PILLAR;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_BLINK);
		m_PrevPatternWasAttack = TRUE;
		m_bIsAttacking = TRUE;
		m_iAttackIndex++;
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
HRESULT CEffigy_Fire::Effigy_Attack_Pattern()
{
	switch (m_eAttack_Type)
	{
	case Client::CEffigy_Fire::EFFIGY_ATTACK_CLAP:
		break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_ROLLING:
		break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_TRANSFORM:
	{
		if (m_iAttackIndex == 1)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_TRANSFORM_2);
			m_iAttackIndex++;
			m_bIsFirstFrame = TRUE;
		}
		else if (m_iAttackIndex == 2)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_TRANSFORM_3);
			m_iAttackIndex = 0;
			m_bIsAttacking = FALSE;
			m_bIsFirstFrame = TRUE;
		}
	}
	break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_RANGED_CUBE:
		break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_PUNCH:
		break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_SNAKE:
		break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_SELF_EXPLOSION:
	{
		if (m_iAttackIndex == 1)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_BS);	
			m_iAttackIndex = 0;
		}
	}
	break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_PILLAR:
	{
		if (m_iAttackIndex == 1)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_ATTACK_PILLAR);
			m_iAttackIndex = 0;
		}
	}
	break;
	case Client::CEffigy_Fire::EFFIGY_ATTACK_END:
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
_bool CEffigy_Fire::VerifyIsPlayerOnLeft()
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
_bool CEffigy_Fire::VerifyIsPlayerNearby(_float _fDistance)
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
_bool CEffigy_Fire::VerifyIsPlayerInFront()
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

void CEffigy_Fire::Collision_State()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	_int iWeaponID = -1;
	vector<CGameObject*> vecPlayer;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	{
		if (static_cast<CPart_Body*>(m_Parts[0].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;
			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);

			if (eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE) /* 폭발 이펙트 */
			{
				_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Zhongli_Sp_Explosion", L"Layer_Effect", &vSelfPos)))
					return;

				CSkillObj::SKILLOBJ_DESC desc = {};
				desc.vSpawnPos = vSelfPos;
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_BigExplosion", L"Layer_Skill", &desc)))
					return;
			}

		}
		else if (eState == FSM_EFFIGY_ATTACK_PUNCH || eState == FSM_EFFIGY_ATTACK_RANGED_LOOP ||
			eState == FSM_EFFIGY_ATTACK_SNAKE_LOOP || eState == FSM_EFFIGY_ATTACK_PILLAR)
		{
			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
		}
	}
	else
	{
		if (static_cast<CPart_Body*>(m_Parts[0].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;

			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}

			if (eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE) /* 폭발 이펙트 */
			{
				_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Zhongli_Sp_Explosion", L"Layer_Effect", &vSelfPos)))
					return;

				CSkillObj::SKILLOBJ_DESC desc = {};
				desc.vSpawnPos = vSelfPos;
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_BigExplosion", L"Layer_Skill", &desc)))
					return;
			}

		}
		else if (eState == FSM_EFFIGY_ATTACK_PUNCH || eState == FSM_EFFIGY_ATTACK_RANGED_LOOP ||
			eState == FSM_EFFIGY_ATTACK_SNAKE_LOOP || eState == FSM_EFFIGY_ATTACK_PILLAR)
		{
			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}
		}
	}


	/* 상태에 따라 충돌 처리할 콜라이더를 정하고 플레이어와의 충돌 검사 */
	if (eState == FSM_EFFIGY_ATTACK_CLAP)
	{
		iWeaponID = static_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot)->Get_WeaponCollider(CBody_Effigy_Fire::EFFIGY_WEAPON_CLAP);
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);
	}
	else if (eState == FSM_EFFIGY_ATTACK_TRANSFORM_1 || eState == FSM_EFFIGY_ATTACK_TRANSFORM_2 || eState == FSM_EFFIGY_ATTACK_TRANSFORM_3 || eState == FSM_EFFIGY_ATTACK_ROLLING_LOOP)
	{
		iWeaponID = static_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot)->Get_WeaponCollider(CBody_Effigy_Fire::EFFIGY_WEAPON_BODY);
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);
	}
	else if (eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE)
	{
		iWeaponID = static_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot)->Get_WeaponCollider(CBody_Effigy_Fire::EFFIGY_WEAPON_BODY);
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

			_float4 vDir = {};
			XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			vDir.w = 1.f;

			if (eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE)
			{
				pPlayer->Set_Damage(99999.f, ELEMENT_FIRE, FSM_HIT_THROW, 6.f, 6.f, vDir);
			}
			else
				pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_END, FSM_HIT_THROW, 6.f, 3.f, vDir);


		}
		else if (eState == FSM_EFFIGY_ATTACK_CLAP || eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE) // 1 tick 만 맞게 할때
		{
			m_bHitPlayer = TRUE;
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

				_float4 vDir = {};
				XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
				vDir.w = 1.f;

				if (eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE)
				{
					pPlayer->Set_AutoDamage(eIndex, 99999.f, ELEMENT_FIRE, FSM_HIT_THROW, 6.f, 6.f, vDir);
				}
				else
					pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_END, FSM_HIT_THROW, 6.f, 3.f, vDir);
			}
		}
		else if (eState == FSM_EFFIGY_ATTACK_CLAP || eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE) // 1 tick 만 맞게 할때
		{
			m_bHitPlayer = TRUE;
		}
	}
}

void CEffigy_Fire::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CEffigy_Fire::Set_Damage(fDamage * 2.f, ELEMENT_FIRE, TRUE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CEffigy_Fire::Set_Damage(fDamage * 1.5f, ELEMENT_FIRE, TRUE);
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

void CEffigy_Fire::Set_ElementUI(ELEMENT eElement)
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

void CEffigy_Fire::Set_Damage(_float fDamage, ELEMENT eElement, _bool bElementalReaction)
{
	ELEMENT eImune = ELEMENT_END;
	if (eElement == ELEMENT_FIRE && m_fFireShield_Hp > 0.f && bElementalReaction == FALSE)
	{
		fDamage = 0.f;
		eImune = ELEMENT_FIRE;
	}
	else if (m_fFireShield_Hp > 0.f)
	{
		m_fFireShield_Hp -= fDamage;
		if (m_fFireShield_Hp < 0)
		{
			m_pHpBar->Set_CurrentHp(m_fHp += m_fFireShield_Hp); /* 체력바 */
			m_fFireShield_Hp = 0.f;
		}
	}
	else
	{
		if (m_bIsCoreExposure == TRUE)
			m_pHpBar->Set_CurrentHp(m_fHp -= fDamage); /* 체력바 */
		else
			return;
	}

	__super::Create_DamageFont(fDamage, eElement, {}, eImune); /* 데미지 폰트 */
}

HRESULT CEffigy_Fire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffigy_Fire::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);
	m_vInitPos = ObjDesc.vInitPos;
	m_eCreatureID = static_cast<CREATUREID>(ObjDesc.iObjectType);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>
	{
	});

	m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_STANDBY_FREE);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_MOVE_RUSHMOVE_LOOP);

	COrtho::ORTHO_DESC tOrthoDesc;

	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
	tOrthoDesc.fPosition = { (_float)(771.f - (_float)g_iWinSizeX / 2.f), (_float)(-(90.f - (_float)g_iWinSizeY / 2.f)) };
	tOrthoDesc.fSize = { 500.f,8.f };
	tOrthoDesc.ePage = PAGE_MAIN;

	m_pHpBar = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));
	m_pHpBar->Set_MaxHp(m_fMaxHp);
	m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_BOSS);
	m_pHpBar->Set_CurrentHp(m_fHp);
	m_pHpBar->Set_OwnerInfo(TEXT("무상의 불"), TEXT(""), 95);

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_pHpBar->Set_CurrentShield(m_fFireShield_Hp);
	m_pHpBar->Set_MaxShield(m_fFireShield_Hp);

	m_strStatusTag = Get_ParticleTag(L"Effigy_Fire");

	return S_OK;
}

HRESULT CEffigy_Fire::PreviousTick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('X')) // 패턴 컨트롤, 쉬프트 누른상태에서 X 누르면 제어 가능, 그냥 X 만 누르면 해제
	{
		if (m_pGameInstance->Key_Pressing(VK_SHIFT))
		{
			if (m_bPatternControl == FALSE)
			{
				m_bPatternControl = TRUE;
			}
			else
			{
				if (m_iPattern < 8)
					++m_iPattern;
				else
					m_iPattern = 1;

				m_PrevPatternWasAttack = FALSE;
				m_bIsAttacking = FALSE;
				m_bIsFirstFrame = TRUE;
				m_iAttackIndex = 0;
			}

			switch (m_iPattern)
			{
			case 1:
				cout << "Pattern 1 : 박수" << endl;
				break;
			case 2:
				cout << "Pattern 2 : 변신" << endl;
				break;
			case 3:
				cout << "Pattern 3 : 주먹" << endl;
				break;
			case 4:
				cout << "Pattern 4 : 자폭" << endl;
				break;
			case 5:
				cout << "Pattern 5 : 구르기" << endl;
				break;
			case 6:
				cout << "Pattern 6 : 원거리 큐브" << endl;
				break;
			case 7:
				cout << "Pattern 7 : 뱀" << endl;
				break;
			case 8:
				cout << "Pattern 8 : 기둥" << endl;
				break;
			default:
				break;
			}
		}
		else
		{
			m_bPatternControl = FALSE;
			cout << "Pattern Control OFF" << endl;
		}
		

		//cout << m_iPattern << endl;
	}


	if (m_pGameInstance->Key_Down('N'))
	{
		m_bIsRechargeCrystal_Destroy = TRUE;
	}

	if (m_pGameInstance->Key_Down('M'))
	{
		m_bIsRechargeCrystal_Destroy = FALSE;
	}

	if (m_pGameInstance->Key_Down('8'))
	{
		m_fFireShield_Hp -= 1000.f;
	}

	m_pElement_OHD->Zero_Elements();
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;


		//FSM_EFFIGY_STATE m_eCurrentState = static_cast<FSM_EFFIGY_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		// 절두체 안에있다면, m_MonsterTarget에 자기자신을 추가
		_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CombinedMatrix();
		if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&CombinedWorldMatrix).r[3], 20.f) && m_fHp > 0.f)
		{
			_vector vDirPlayerToSelf = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_vPlayerPosss;
			_float flengthPlayerToSelf = XMVectorGetX(XMVector3Length(vDirPlayerToSelf));

			if (flengthPlayerToSelf < 30.f)
			{
				if(m_Parts[PART_BODY].pPartySlot->Get_RenderEnable() == TRUE)
					Add_Target(this);
			}
		}

		for (_uint i = 0; i < ELEMENT_END; ++i)
		{
			if (m_Parts[PART_BODY].pPartySlot->isHostElement((ELEMENT)i) == TRUE)
				CEffigy_Fire::Set_ElementUI((ELEMENT)i);
		}

		if (m_fHp <= 0.f)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_EFFIGY_CORE_EXPOSURE2_LOOP);
			m_pTransformCom->Go_Up(fTimeDelta / 7.f);

			m_fDieTimer += fTimeDelta;
			if (m_fDieTimer >= 2.f)
			{
				if (m_bDropChest == FALSE)
				{
					CBossChest::BOSS_CHEST	tBossChest = { };
					tBossChest.eLevel = static_cast<LEVELID>(m_pGameInstance->Get_CurrentLevel());
					XMStoreFloat4(&tBossChest.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					tBossChest.vPosition.y = 10.9661f;
					tBossChest.vReservePosition = _float4{ };

					tBossChest.DropList.push_back({ 124, 10000 });
					tBossChest.DropList.push_back({ 124, 10000 });
					tBossChest.DropList.push_back({ 124, 10000 });
					tBossChest.DropList.push_back({ 124, 10000 });
					tBossChest.DropList.push_back({ 125, 100 });
					tBossChest.DropList.push_back({ 125, 100 });

					tBossChest.DropList.push_back({ 139, 2 });
					tBossChest.DropList.push_back({ 140, 2 });
					tBossChest.DropList.push_back({ 141, 2 });
					tBossChest.DropList.push_back({ 142, 2 });

					if (FAILED(m_pGameInstance->Add_Object_Layer(tBossChest.eLevel, TEXT("Prototype_GameObject_Boss_Chest"), L"Layer_BossChest", &tBossChest)))
						return E_FAIL;

					m_bDropChest = TRUE;
				}

				// 사망 폭발 이펙트
				_float m_fScale = 10.f;
				CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
				tExplodeDesc.fExplodeExpand = 3.f;
				tExplodeDesc.fScale = m_fScale;
				tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
				tExplodeDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 1.f, 0.f, 0.f };;

				m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

				CBody_Effigy_Fire* pEffigyBody = dynamic_cast<CBody_Effigy_Fire*>(m_Parts[0].pPartySlot);
				pEffigyBody->Release_ColliderID();
				m_pGameInstance->Add_InteractionCount(L"Effigy_Fire");
				Set_Destroy(TRUE);
			}
		}

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);

			/* 데미지 받았을 때 */
			_float fDamage = pPart.pPartySlot->Get_Damage();
			if (fDamage != 0)
			{
				ELEMENT eElement = pPart.pPartySlot->Get_ReceiveElement();
				Set_ElementUI(eElement);
				ELEMENTAL_REACTION eReaction = pPart.pPartySlot->Get_ElementalReaction(); /* 원소반응 */
				ElementalReaction(fDamage, eReaction);

				CEffigy_Fire::Set_Damage(fDamage, eElement);

				CModel* pModel = static_cast<CModel*>(pPart.pPartySlot->Find_Component(TEXT("Com_Model")));
				m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, pModel->Get_CombinedMatrix("HitBox"), m_pTransformCom, 0.5f);

			}
		}

		if (m_bHPAlert == TRUE)
		{
			m_pHpBar->PreviousTick(fTimeDelta);
			m_pElement_OHD->PreviousTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CEffigy_Fire::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CEffigy_Fire::Pattern(fTimeDelta)))
			return E_FAIL;

		if (m_bIsPillarPattern == TRUE)		// 기둥 패턴 중이라면
		{
			if (FAILED(CEffigy_Fire::Pattern_Pillar(fTimeDelta)))
				return E_FAIL;
		}


		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}

		CEffigy_Fire::Collision_State();

		if (m_bHPAlert == TRUE)
		{
			m_pHpBar->Tick(fTimeDelta);
			//m_pElement_OHD->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f, 20.f);
			m_pHpBar->Set_CurrentShield(m_fFireShield_Hp);
			m_pElement_OHD->Update_Matrix((_float)(551.f - (_float)g_iWinSizeX / 2.f), (_float)(-(124.f - (_float)g_iWinSizeY / 2.f)), 30.f, 30.f);
			m_pElement_OHD->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CEffigy_Fire::LateTick(_float fTimeDelta)
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

		if (m_bHPAlert == TRUE)
		{
			m_pHpBar->LateTick(fTimeDelta);
			m_pElement_OHD->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CEffigy_Fire::Render()
{
	return S_OK;
}

CEffigy_Fire* CEffigy_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffigy_Fire* pInstance = new CEffigy_Fire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEffigy_Fire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffigy_Fire::Clone(void* pArg)
{
	CEffigy_Fire* pInstance = new CEffigy_Fire(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEffigy_Fire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffigy_Fire::Free()
{
	__super::Free();
	
	if (m_bCloned == true)
	{
		Safe_Release(m_pHpBar);
		Safe_Release(m_pElement_OHD);
	}

	for (auto& pPart : m_Parts)
	{
		pPart.pPartySlot->Release_ColliderID();
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();

}