#include "stdafx.h"
#include "Body_Brute.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

CBody_Brute::CBody_Brute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Brute::CBody_Brute(const CBody_Brute& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Brute::Invalidate_Animation(_float fTimeDelta)
{
	FSM_BRUTE_STATE eState = static_cast<FSM_BRUTE_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	_float fTime = fTimeDelta;
	if (m_bPetrification == TRUE && (eState != FSM_HILI_DIE_2 && eState != FSM_HILI_DIE_1 && eState != FSM_HILI_RUNATTACK_END))
		return S_OK;

	_vector vParentSelfLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);

	// 스위치문
	switch (eState)
	{
	case Client::FSM_BRUTE_THINK:
		break;
	case Client::FSM_BRUTE_SIT_TALK:
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;
	case Client::FSM_BRUTE_SLEEP:
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;
	case Client::FSM_BRUTE_SIT_IDLE:
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;
	case Client::FSM_BRUTE_SIT:
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;
	case Client::FSM_BRUTE_STANDUP:
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;
	case Client::FSM_BRUTE_KNEEL_AND_NOD:		// 무릎 꿇고 고개 떨구기 ?? 뭔지 잘 모르겠음
		break;
#pragma region 쉴드
	case Client::FSM_BRUTE_WALK_RIGHT_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_WALK_LEFT_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_WALK_FORWARD_WITH_SHIELD:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_BRUTE_WALK_BACKWARD_WITH_SHIELD:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_WALK_BACKWARD_WITH_SHIELD);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_BRUTE_TURN_RIGHT_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_TURN_LEFT_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_THROW_SOMETHING_WITH_SHIELD:
		break;
	case Client::FSM_BRUTE_PUNCH_WITH_SHIELD:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_PUNCH_WITH_SHIELD);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_PUNCH_WITH_SHIELD) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_WALK_FORWARD_WITH_SHIELD);
	}
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
		m_pModelCom->Set_Animation(FSM_BRUTE_IDLE_WITH_SHIELD);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
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
#pragma endregion
#pragma region 맨손
	case Client::FSM_BRUTE_WALK_RIGHT_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_WALK_LEFT_WITHHANDS:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_WALK_LEFT_WITHHANDS);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
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
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_RUN_WITH_HANDS);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
		break;
	case Client::FSM_BRUTE_POWER_UP_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_POWER_DOWN_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_JUMP_ATTACK_ON_GROUND_WITH_HANDS:
		break;
	case Client::FSM_BRUTE_IDLE_WITH_HANDS:
	{
		m_pModelCom->Set_Animation(45);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
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
#pragma endregion
	case Client::FSM_BRUTE_DROWNING_LOOP:
		break;
	case Client::FSM_BRUTE_DROWNING:
		break;
	case Client::FSM_BRUTE_DIE:
	{
		if (m_bFirstFrame == TRUE)
		{
			m_bFirstFrame = FALSE;
			m_pParentController->Sleep_Controller();
		}

		m_pModelCom->Set_Animation(FSM_BRUTE_DIE);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_DIE) * 0.99f)
			m_bDead = TRUE;
	}
	break;
#pragma region 도끼
	case Client::FSM_BRUTE_WHIRL_LOOP_WITH_AXE:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_WHIRL_LOOP_WITH_AXE);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_iAttackIndex < 10.f)
		{
			if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_WHIRL_LOOP_WITH_AXE) * 0.95f)
				m_iAttackIndex++;
		}
		else
		{
			if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_WHIRL_LOOP_WITH_AXE) * 0.65f)
			{
				m_pParentFSMCom->Change_ForceState(FSM_BRUTE_FINISH_AXE_WHIRL);
				m_iAttackIndex = 0;
			}
		}

		//cout << m_iAttackIndex << endl;
	}
	break;
	case Client::FSM_BRUTE_WHIRL_CRASH_WALL_WITH_AXE:
		break;
	case Client::FSM_BRUTE_START_AXE_WHIRL:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_START_AXE_WHIRL);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_START_AXE_WHIRL) * 0.9f)
			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_WHIRL_LOOP_WITH_AXE);
	}
	break;
	case Client::FSM_BRUTE_FINISH_AXE_WHIRL:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_FINISH_AXE_WHIRL);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_FINISH_AXE_WHIRL) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
	}
	break;
	case Client::FSM_BRUTE_WALK_FORWARD_WITH_AXE:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_WALK_FORWARD_WITH_AXE);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_WALK_FORWARD_WITH_AXE) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
	}
	break;
	case Client::FSM_BRUTE_WALK_BACKWARD_WITH_AXE:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_WALK_BACKWARD_WITH_AXE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_BRUTE_TURN_RIGHT_WITH_AXE:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_TURN_RIGHT_WITH_AXE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
		break;
	case Client::FSM_BRUTE_TURN_LEFT_WITH_AXE:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_TURN_LEFT_WITH_AXE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
		break;
	case Client::FSM_BRUTE_SLIME_PROPERTY_TO_AXE:
		break;
	case Client::FSM_BRUTE_THROW_SLIME_WITH_AXE:
		break;
	case Client::FSM_BRUTE_RUN_WITH_AXE:
	{
		m_pModelCom->Set_Animation(FSM_BRUTE_RUN_WITH_AXE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
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
		m_pModelCom->Set_Animation(FSM_BRUTE_IDLE_WITH_AXE);
		m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
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
		//m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		//cout << fRate << "총 프레임 : " << m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE) << endl;

		if (fRate >= 93.f && fRate <= 120.f)
		{
			_float4 fParentSelfLook = {};
			XMStoreFloat4(&fParentSelfLook, vParentSelfLook);
			m_pParentController->Movement(fTimeDelta, fParentSelfLook, 2.f);
			//m_pParentTransformCom->Go_Straight(fTimeDelta / 2.f);
		}
		else if (fRate > 120.f && fRate <= 220.f)
		{
			_float4 fParentSelfLook = {};
			XMStoreFloat4(&fParentSelfLook, vParentSelfLook);
			m_pParentController->Movement(fTimeDelta, fParentSelfLook, 4.f);
			//m_pParentTransformCom->Go_Straight(fTimeDelta);
		}

		if (fRate >= 222 && bSound_1 == FALSE)
		{
			bSound_1 = TRUE;
			//m_pGameInstance->PlaySoundW(L"Brute_Chopping.mp3", 100.f);
		}


		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_CHOP_AND_BUTT_WITH_AXE) * 0.95f)
		{

			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
		}

		//_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		//vCurrentTranslation.m128_f32[1] = 0.f;
		//Set_EtcTranslation(vCurrentTranslation);
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_4:
	{
		//m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(FSM_BRUTE_AXE_ATTACK_4);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		
		if (fRate >= 115.f && fRate <= 162.f)
		{
			_float4 fParentSelfLook = {};
			XMStoreFloat4(&fParentSelfLook, vParentSelfLook);
			m_pParentController->Movement(fTimeDelta, fParentSelfLook, 4.f);
			//m_pParentTransformCom->Go_Straight(fTimeDelta);
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_ATTACK_4) * 0.75f)
		{
			m_iRRPrevState = FSM_BRUTE_AXE_ATTACK_4;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);

			//m_pGameInstance->PlaySoundW(L"Brute_Swing.mp3", 100.f);
		}

		//_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		//vCurrentTranslation.m128_f32[1] = 0.f;
		//Set_EtcTranslation(vCurrentTranslation);
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_3:
	{
		//m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(FSM_BRUTE_AXE_ATTACK_3);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= 45.f && fRate <= 105.f)
		{
			_float4 fParentSelfLook = {};
			XMStoreFloat4(&fParentSelfLook, vParentSelfLook);
			m_pParentController->Movement(fTimeDelta, fParentSelfLook, 2.f);
			//m_pParentTransformCom->Go_Straight(fTimeDelta / 2.f);
		}

		if (m_bIsSwing_Type_3)
		{
			if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_ATTACK_3) * 0.5f)
			{
				m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
			}
		}
		else
		{
			if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_ATTACK_3) * 0.95f)
			{
				m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
				//m_pGameInstance->PlaySoundW(L"Brute_Swing.mp3", 100.f);

			}
		}

		//_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		//vCurrentTranslation.m128_f32[1] = 0.f;
		//Set_EtcTranslation(vCurrentTranslation);
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_2:
	{
		//m_vPrevRootTranslation = m_vRootTranslation;
		
		m_pModelCom->Set_Animation(FSM_BRUTE_AXE_ATTACK_2);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= 95.f && fRate <= 110.f)
		{
			_float4 fParentSelfLook = {};
			XMStoreFloat4(&fParentSelfLook, vParentSelfLook);
			m_pParentController->Movement(fTimeDelta, fParentSelfLook, 4.f);
			//m_pParentTransformCom->Go_Straight(fTimeDelta);
		}

		if (fRate >= 111 && bSound_2 == FALSE)
		{
			bSound_2 = TRUE;
			//m_pGameInstance->PlaySoundW(L"Brute_Chopping.mp3", 100.f);
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_ATTACK_2) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
		}	

		//_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		//vCurrentTranslation.m128_f32[1] = 0.f;
		//Set_EtcTranslation(vCurrentTranslation);
	}
	break;
	case Client::FSM_BRUTE_AXE_ATTACK_1:
	{
		//m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(FSM_BRUTE_AXE_ATTACK_1);
		fRate = m_pModelCom->Play_BruteRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= 100.f && fRate <= 138.f)
		{
			_float4 fParentSelfLook = {};
			XMStoreFloat4(&fParentSelfLook, vParentSelfLook);
			m_pParentController->Movement(fTimeDelta, fParentSelfLook, 2.f);
			//m_pParentTransformCom->Go_Straight(fTimeDelta / 2.f);
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_BRUTE_AXE_ATTACK_1) * 0.59f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_BRUTE_THINK);
			m_pGameInstance->PlaySoundW(L"Brute_Swing.mp3", 100.f);

		}

		//_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		//vCurrentTranslation.m128_f32[1] = 0.f;
		//Set_EtcTranslation(vCurrentTranslation);
	}
	break;
#pragma endregion
	case Client::FSM_BRUTE_UNKNOWN_ACTION:
		break;
	case Client::FSM_BRUTE_END:
		break;
	default:
		break;
	}

	// m_bInitRRAnimation 변수가 TRUE일 때 실행
	if (m_bInitRRAnimation == TRUE)
	{
		// 모델의 애니메이션 변환 비율이 1.0보다 큰 경우
		if (m_pModelCom->Get_ConvertRatio() > 1.f)
			m_bInitRRAnimation = FALSE; // 애니메이션이 초기화되었음을 나타내는 변수를 FALSE로 설정

		// "Bone001" 뼈의 현재 회전을 얻어옴
		_vector vReturunRot = m_pModelCom->Get_CurrBoneTransform("Bone001", ROOTBONETRANSFORMTYPE::ROTATION);

		// "Bone001" 뼈의 이전 프레임 회전 정보를 얻어옴
		KEYFRAME ePrevFrame = m_pModelCom->Get_PrevKeyFrame(m_iRRPrevState, "Bone001");

		// 현재 회전과 이전 프레임의 회전 간의 각도를 계산
		_float fRadian = CalculateQuaternionRotationAngle(vReturunRot, XMLoadFloat4(&ePrevFrame.vRotation));

		// 회전 방향에 따라 부모 오브젝트를 회전
		if (m_eRotationDir == ROT_LEFT)
			m_pParentTransformCom->RotationFromCurrentState(m_pParentTransformCom->Get_State(CTransform::STATE_UP), (m_fAccumulateRRRadian - fRadian)); // 왼쪽으로 회전
		else if (m_eRotationDir == ROT_RIGHT)
			m_pParentTransformCom->RotationFromCurrentState(m_pParentTransformCom->Get_State(CTransform::STATE_UP), (fRadian - m_fAccumulateRRRadian)); // 오른쪽으로 회전

		// 누적된 회전 각도를 업데이트
		m_fAccumulateRRRadian = fRadian;

		// 부모 오브젝트의 방향을 재설정
		m_pParentTransformCom->Reset_Look_At_Direction();
	}

	return S_OK;
}

HRESULT CBody_Brute::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.4f, 0.6f, 0.4f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBody_Brute::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Brute::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Brute::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Brute::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Brute::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Brute::Render()
{
	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		/* Exception Effect Eye */
		if (m_eTypeID == CTY_PLAYER && (m_iFaceMeshIndex - 1 == siMesh || m_iFaceMeshIndex - 2 == siMesh))
			continue;

		if (m_eTypeID == CTY_MONSTER && m_iFaceMeshIndex - 1 == siMesh)
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_LightMapTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_LIGHTMAP)))
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = TRUE), sizeof _bool)))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bFaceMesh", &(bFaceMesh = (m_iFaceMeshIndex == siMesh)), sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
		{
			if (m_fDissolve == 0.f)
			{
				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fDissolve, sizeof _float)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveSize", &m_fDissolveSize, sizeof _float)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveColor", &m_vDissolveColor, sizeof _float3)))
					return E_FAIL;

				if (FAILED(m_pDissolveCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_DissolveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DISSOLVE)))
					return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pPetrificationCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_PetrificationTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PETRIFICATION)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;

		m_pColliderCom->Render();
	}

	return S_OK;
}

void CBody_Brute::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);
}

CBody_Brute* CBody_Brute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Brute* pInstance = new CBody_Brute(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Brute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Brute::Clone(void* pArg)
{
	CBody_Brute* pInstance = new CBody_Brute(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Brute"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Brute::Free()
{
	__super::Free();
}