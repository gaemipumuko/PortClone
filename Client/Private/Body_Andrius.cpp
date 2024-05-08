#include "stdafx.h"
#include "Body_Andrius.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

CBody_Andrius::CBody_Andrius(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Andrius::CBody_Andrius(const CBody_Andrius& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Andrius::Invalidate_Animation(_float fTimeDelta)
{
	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	switch (eState)
	{
	case Client::FSM_ANDRIUS_THINK:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_IDLE:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_WALK_FIRST:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_WALK);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));

		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_WALK) - 1)
			m_iCameraSceneIndex++;

		if(m_iCameraSceneIndex < 2)
			pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_ONE, 10.f, 0.2f, TRUE);
		else if (m_iCameraSceneIndex >= 2)
			pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_TWO, 10.f, 0.2f, TRUE);
	}
	break;
	case Client::FSM_ANDRIUS_WALK_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_WALK);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		
		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
	}
	break;
	case Client::FSM_ANDRIUS_WALK_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_WALK);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
	}
	break;
	case Client::FSM_ANDRIUS_SCRATCH_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRATCH_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRATCH_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRATCH_LEFT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_THINK);
			//m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_RIGHT);
		}
	}
	break;
	case Client::FSM_ANDRIUS_SCRATCH_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRATCH_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRATCH_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRATCH_RIGHT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_THINK);
			//m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
		}
	}
	break;
	case Client::FSM_ANDRIUS_SWEEP_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SWEEP_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SWEEP_LEFT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_THINK);
			//m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
	}
	break;
	case Client::FSM_ANDRIUS_SWEEP_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SWEEP_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SWEEP_RIGHT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_THINK);
			//m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_RIGHT);
	}
	break;
	case Client::FSM_ANDRIUS_JUMP:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_JUMP);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		// 애니메이션 진행률을 계산
		_float fAnimationDuration = m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_JUMP);
		_float fAnimationProgress = fRate / fAnimationDuration;

		if (fAnimationProgress < 0.2f)
			m_pModelCom->Set_ConvertSpeed(20.f);// 빠르게
		else
			m_pModelCom->Set_ConvertSpeed(5.f); // 정상 속도

		if (fAnimationProgress >= 0.99f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
	}
	break;
	case Client::FSM_ANDRIUS_BACKJUMP:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_BACKJUMP);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_BACKJUMP) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_END);
	}
	break;
	case Client::FSM_ANDRIUS_RUSH:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_RUSH);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_ANDRIUS_RUSH_READY:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_JUMP_READY_FRONT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_JUMP_READY_FRONT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_RUSH);
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_RUSH);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_RUSH);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_RUSH);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_JUMP);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bReturnRotation == TRUE)
		{
			m_fAccumulateRRRadian = 0.f;

			m_bReturnRotation = FALSE;
			m_bInitRRAnimation = TRUE;
		}

		// 애니메이션 진행률을 계산
		_float fAnimationDuration = m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_JUMP);
		_float fAnimationProgress = fRate / fAnimationDuration;

		if (fAnimationProgress < 0.2f)	// 초반 20% 는 빠르게 
			m_pModelCom->Set_ConvertSpeed(20.f);// 빠르게
		else
			m_pModelCom->Set_ConvertSpeed(5.f); // 정상 속도

		if (fAnimationProgress >= 0.99f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE_VARIATION_1);
	}
	break;
	case Client::FSM_ANDRIUS_ICICLESHOOT_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_ICICLESHOOT_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_ICICLESHOOT_RIGHT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
	}
	break;
	case Client::FSM_ANDRIUS_ICICLESHOOT_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_ICICLESHOOT_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_ICICLESHOOT_LEFT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_RIGHT);
	}
	break;
	case Client::FSM_ANDRIUS_JUMP_READY_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_JUMP_READY_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_JUMP_READY_LEFT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_JUMP);
	}
	break;
	case Client::FSM_ANDRIUS_JUMP_READY_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_JUMP_READY_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_JUMP_READY_RIGHT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_JUMP);
	}
	break;
	case Client::FSM_ANDRIUS_JUMP_READY_FRONT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_JUMP_READY_FRONT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_JUMP_READY_FRONT) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_JUMP);
	}
	break;
	case Client::FSM_ANDRIUS_SCRAM_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_LEFT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;
			
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
		}
	}
	break;
	case Client::FSM_ANDRIUS_SCRAM_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_RIGHT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WALK_RIGHT);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_START:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_LEFT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_ARRIVE:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_LEFT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_LEFT);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_START:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_RIGHT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_ARRIVE:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_RIGHT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_RIGHT);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_JUMP:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_LEFT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_JUMP:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SCRAM_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SCRAM_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_SCRAM_RIGHT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_FAST_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_TURN_FAST_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_TURN_FAST_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_TURN_FAST_LEFT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_FAST_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_TURN_FAST_RIGHT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_TURN_FAST_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_TURN_FAST_RIGHT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN180_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_TURN180_LEFT);
		m_pModelCom->Play_Animation(FALSE, m_pParentTransformCom, fTimeDelta);
	}
	break;
	case Client::FSM_ANDRIUS_TURN180_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_TURN180_RIGHT);
		m_pModelCom->Play_Animation(FALSE, m_pParentTransformCom, fTimeDelta);
	}
	break;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	case Client::FSM_ANDRIUS_PHAZE2_ENTER_HOWL_START:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWL_START);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_HOWL_START) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_PHAZE2_ENTER_HOWLING);
	}
	break;
	case Client::FSM_ANDRIUS_PHAZE2_ENTER_HOWLING:
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_FOUR, 10.f, 0.2f, TRUE);

		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWLING);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_ANDRIUS_PHAZE2_ENTER_HOWL_END:
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_TWO, 10.f, 0.2f, TRUE);

		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWL_END);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_HOWL_END) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_HAIL_SHAKE_START);
	}
	break;
	case Client::FSM_ANDRIUS_ICE_SHOCK_WAVE_READY:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_ICE_SHOCK_WAVE);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		_float fAnimationDuration = m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_ICE_SHOCK_WAVE);
		_float fTransitionPoint = fAnimationDuration * 0.4f;  // 애니메이션 길이의 40%
		if (fRate >= fTransitionPoint)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_ICE_SHOCK_WAVE);
	}
	break;
	case Client::FSM_ANDRIUS_ICE_SHOCK_WAVE:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_ICE_SHOCK_WAVE);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_ICE_SHOCK_WAVE) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SHOCK_WAVE_READY:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_WIND_SHOCK_WAVE);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		_float fAnimationDuration = m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_WIND_SHOCK_WAVE);
		_float fTransitionPoint = fAnimationDuration * 0.25f;  // 애니메이션 길이의 40%
		if (fRate >= fTransitionPoint)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_WIND_SHOCK_WAVE);
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SHOCK_WAVE:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_WIND_SHOCK_WAVE);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_WIND_SHOCK_WAVE) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SCRATCH_LEFT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_ICE_SCRATCH_LEFT);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_ICE_SCRATCH_LEFT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_ICE_SCRATCH_LEFT;
			m_eRotationDir = ROT_RIGHT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
		}
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SCRATCH_RIGHT:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_ICE_SCRATCH_RIGHT);		// RIght임 ;;
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_ICE_SCRATCH_RIGHT) * 0.95f)
		{
			m_eRRPrevState = ANIM_ANDRIUS_ICE_SCRATCH_RIGHT;
			m_eRotationDir = ROT_LEFT;
			m_bReturnRotation = TRUE;

			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
		}
	}
	break;
	case Client::FSM_ANDRIUS_ROAR_START:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWL_START);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_HOWL_START) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_ROARING);
	}
	break;
	case Client::FSM_ANDRIUS_ROARING:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWLING);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_ANDRIUS_ROAR_END:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWL_END);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_HOWL_END) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_END);
	}
	break;
	case Client::FSM_ANDRIUS_IDLE_VARIATION_1:
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_THREE, 10.f, 0.2f, TRUE);

		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE_BATTLE);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_IDLE_BATTLE) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_PHAZE2_ENTER_HOWL_START);
	}
	break;
	case Client::FSM_ANDRIUS_IDLE_VARIATION_2:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE_VARIATION_1);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_IDLE_VARIATION_1) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
	}
	break;
	case Client::FSM_ANDRIUS_IDLE_VARIATION_3:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_IDLE_VARIATION_2);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_IDLE_VARIATION_2) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE);
	}
	break;
	case Client::FSM_ANDRIUS_HAIL_SHAKE_START:
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);

		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SHAKE_START);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		//fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SHAKE_START) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_HAIL_SHAKING);
	}
	break;
	case Client::FSM_ANDRIUS_HAIL_SHAKING:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SHAKE);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		//fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SHAKE) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_HAIL_SHAKE_END);
	}
	break;
	case Client::FSM_ANDRIUS_HAIL_SHAKE_END:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_SHAKE_END);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		//fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_SHAKE_END) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_IDLE_VARIATION_3);
	}
	break;
	case Client::FSM_ANDRIUS_DEAD_HOWL_START:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWL_START);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(ANIM_ANDRIUS_HOWL_START) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_ANDRIUS_DEAD_HOWLING);
	}
	break;
	case Client::FSM_ANDRIUS_DEAD_HOWLING:
	{
		m_pModelCom->Set_Animation(ANIM_ANDRIUS_HOWLING);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_ANDRIUS_END:
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

		// "Bone_Root" 뼈의 현재 회전을 얻어옴
		_vector vReturunRot = m_pModelCom->Get_CurrBoneTransform("Bone_Root", ROOTBONETRANSFORMTYPE::ROTATION);

		// "Bone_Root" 뼈의 이전 프레임 회전 정보를 얻어옴
		KEYFRAME ePrevFrame = m_pModelCom->Get_PrevKeyFrame(m_eRRPrevState, "Bone_Root");

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

	if (eState == FSM_ANDRIUS_DEAD_HOWL_START || eState == FSM_ANDRIUS_DEAD_HOWLING)
	{
		m_fDissolve += 0.005f;
		m_vDissolveColor = _float3(0.f, 0.f, 1.f);
	}

	return S_OK;
}

HRESULT CBody_Andrius::Bind_ShaderResources_Trail()
{
	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTrailTextureCom->Bind_ShaderCurrentTexture(m_pTrailShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_RawValue("g_vOriginalColor", &m_vTrailOriginalColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_RawValue("g_vAttenuationColor", &m_vTrailAttenuationColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Andrius::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Texture_BoreasLightMap */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LupiBoreas_LightMap"),
		TEXT("Com_Texture_BoreasLightMap"), (CComponent**)&m_pTexture_BoreasLightMap)))
		return E_FAIL;

	/* For.Com_Texture_BoreasFluid */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LupiBoreas_Fluid"),
		TEXT("Com_Texture_BoreasFluid"), (CComponent**)&m_pTexture_BoreasFluid)))
		return E_FAIL;

	/* For.Com_Texture_BoreasIce */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ice_Floor"),
		TEXT("Com_Texture_BoreasIce"), (CComponent**)&m_pTexture_BoreasIce)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.5f, 2.f, 3.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 2.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f , 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	CBounding_SPHERE::BOUNDING_SPHERE_DESC tWeaponBoundDesc = {};

	tWeaponBoundDesc.fRadius = 2.2f;
	tWeaponBoundDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_LHAND"), (CComponent**)&m_pWeaponColliderCom[WEAPON_LHAND], &tWeaponBoundDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iWeaponColliionID[WEAPON_LHAND], m_pWeaponColliderCom[WEAPON_LHAND], this);

	tWeaponBoundDesc.fRadius = 2.2f;
	tWeaponBoundDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_RHAND"), (CComponent**)&m_pWeaponColliderCom[WEAPON_RHAND], &tWeaponBoundDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iWeaponColliionID[WEAPON_RHAND], m_pWeaponColliderCom[WEAPON_RHAND], this);


	tWeaponBoundDesc.fRadius = 6.f;
	tWeaponBoundDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_SWEEP"), (CComponent**)&m_pWeaponColliderCom[WEAPON_SWEEP], &tWeaponBoundDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iWeaponColliionID[WEAPON_SWEEP], m_pWeaponColliderCom[WEAPON_SWEEP], this);

	

	return S_OK;
}

HRESULT CBody_Andrius::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Andrius::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBody_Andrius::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Andrius::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_pWeaponColliderCom[WEAPON_LHAND]->Update(
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_L_Hand")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix)
	);

	m_pWeaponColliderCom[WEAPON_RHAND]->Update(
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_R_Hand")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix)
	);

	m_pWeaponColliderCom[WEAPON_SWEEP]->Update(
		m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()
	);


	return S_OK;
}

HRESULT CBody_Andrius::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;
	
	m_bPetrification = FALSE;

	/* Motion Trail */
	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_ANDRIUS_TURN_RUSH_LEFT ||
		eState == FSM_ANDRIUS_TURN_RUSH_RIGHT ||
		eState == FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_START ||
		eState == FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_START)
	{
		m_fMTrailFrame -= fTimeDelta;
		if (m_fMTrailFrame < 0.f)
		{
			m_fMTrailFrame = 0.05f;

			CModel* pModel = static_cast<CModel*>(m_pModelCom->Clone(nullptr));
			m_MTrailList.push_front({ m_CombinedWorldMatrix, pModel });

			if (m_MTrailList.size() > 10)
			{
				Safe_Release(m_MTrailList.back().pModelComponent);
				m_MTrailList.pop_back();
			}
		}
	}
	else
	{
		if (m_MTrailList.size() > 0)
		{
			m_fMTrailFrame -= fTimeDelta;
			if (m_fMTrailFrame < 0.f)
			{
				m_fMTrailFrame = 0.1f;

				Safe_Release(m_MTrailList.back().pModelComponent);
				m_MTrailList.pop_back();
			}
		}
	}

	// 안드리우스 트레일 임시 테스트
#pragma region 안드리우스 임시 트레일 테스트
	///* Render Trail */
	//_matrix TrailMatrix = { };
	//_vector	vLow = { }, vHigh = { };
	//_vector	vLow_RUSH = { }, vHigh_RUSH = { };

	//TrailMatrix =
	//	XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_L_Claw_02")) *
	//	XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
	//	XMMatrixTranslation(0.f, 0.f, 0.f) *
	//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
	//vLow = TrailMatrix.r[3];

	//TrailMatrix =
	//	XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_L_Claw_08")) *
	//	XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
	//	XMMatrixTranslation(0.f, 0.f, 0.f) *
	//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
	//vHigh = TrailMatrix.r[3];

	//TrailMatrix =
	//	XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_R_Wing_05")) *
	//	XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
	//	XMMatrixTranslation(0.f, 0.f, 0.f) *
	//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
	//vLow_RUSH = TrailMatrix.r[3];

	//TrailMatrix =
	//	XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_R_Wing_09")) *
	//	XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
	//	XMMatrixTranslation(0.f, 0.f, 0.f) *
	//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
	//vHigh_RUSH = TrailMatrix.r[3];

	//FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	//if (eState == FSM_ANDRIUS_SCRATCH_LEFT ||
	//	eState == FSM_ANDRIUS_SCRATCH_RIGHT ||
	//	eState == FSM_ANDRIUS_SWEEP_LEFT ||
	//	eState == FSM_ANDRIUS_SWEEP_RIGHT ||
	//	eState == FSM_ATTACK_END)
	//{
	//	m_pTrailBufferCom->Set_PlayTrail(TRUE);
	//	m_pTrailBufferCom->Append_TrailVertex(fTimeDelta, vLow, vHigh);
	//}
	//else
	//	m_pTrailBufferCom->Set_PlayTrail(FALSE);

	//if (eState == FSM_ANDRIUS_RUSH||
	//	eState == FSM_ANDRIUS_JUMP ||
	//	eState == FSM_ANDRIUS_TURN_RUSH_LEFT ||
	//	eState == FSM_ANDRIUS_TURN_RUSH_RIGHT ||
	//	eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH ||
	//	eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP ||
	//	eState == FSM_ATTACK_END)
	//{
	//	m_pTrailBufferCom->Set_PlayTrail(TRUE);
	//	m_pTrailBufferCom->Append_TrailVertex(fTimeDelta, vLow_RUSH, vHigh_RUSH);
	//}
	//else
	//	m_pTrailBufferCom->Set_PlayTrail(FALSE);

	//m_pTrailBufferCom->Invalidate_VertexBuff(fTimeDelta);
#pragma endregion

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_MTRAIL, this);

	return S_OK;
}

HRESULT CBody_Andrius::Render()
{
	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE))) 
		{
			if (1 == siMesh) 
			{
				if (FAILED(m_pTexture_BoreasLightMap->Bind_ShaderResource(m_pShaderCom, "g_BoreasBodyLightMap")))
					return E_FAIL;

				if (FAILED(m_pTexture_BoreasIce->Bind_ShaderResource(m_pShaderCom, "g_BoreasBodyIce")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_EFFECT_BOREAS_BODY_1)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
					return E_FAIL;

				continue;
			}

			else if (2 == siMesh) 
			{
				continue;
			}

			else if (3 == siMesh)
			{
				// 허리 이펙트 메시
				//if (FAILED(m_pTexture_BoreasFluid->Bind_ShaderResource(m_pShaderCom, "g_BoreasFluid")))
				//	return E_FAIL;

				//if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_EFFECT_BOREAS_BODY_2)))
				//	return E_FAIL;

				//if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
				//	return E_FAIL;

				continue;
			}

			else if (5 == siMesh)
			{
				continue;
			}
		}

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

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

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

			if (FAILED(m_pDissolveCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_BoreasBodyIce")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DISSOLVE)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Andrius::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (2 == siMesh || 3 == siMesh || 5 == siMesh)
			continue;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_SHADOW)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Andrius::Render_MTrail()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	_float	fStep = 0;
	for (auto& iter = m_MTrailList.begin(); iter != m_MTrailList.end(); ++iter)
	{
		if (iter == m_MTrailList.begin())
			continue;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", iter->WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fFactor", &fStep, sizeof(_float))))
			return E_FAIL;

		_uint		iNumMeshes = iter->pModelComponent->Get_NumMeshes();

		for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
		{
			if (FAILED(iter->pModelComponent->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
				continue;

				if (FAILED(iter->pModelComponent->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
					return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_MTRAIL)))
				return E_FAIL;

			if (FAILED(iter->pModelComponent->Render(static_cast<_uint>(siMesh))))
				return E_FAIL;
		}

		++fStep;
	}

	return S_OK;
}

void CBody_Andrius::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iWeaponColliionID[WEAPON_LHAND]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iWeaponColliionID[WEAPON_RHAND]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iWeaponColliionID[WEAPON_SWEEP]);
}

CBody_Andrius* CBody_Andrius::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Andrius* pInstance = new CBody_Andrius(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Andrius"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Andrius::Clone(void* pArg)
{
	CBody_Andrius* pInstance = new CBody_Andrius(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Andrius"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Andrius::Free()
{
	__super::Free();

	Safe_Release(m_pTexture_BoreasLightMap);
	Safe_Release(m_pTexture_BoreasFluid);
	Safe_Release(m_pTexture_BoreasIce);

	Safe_Release(m_pWeaponColliderCom[WEAPON_LHAND]);
	Safe_Release(m_pWeaponColliderCom[WEAPON_RHAND]);
	Safe_Release(m_pWeaponColliderCom[WEAPON_SWEEP]);
}