#include "stdafx.h"
#include "Body_Dvalin.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

#include "SkillObj.h"
#include "FX_Meteorite.h"

#include "Dvalin.h"
#include "DvalinPlatform.h"
#include "Player.h"

CBody_Dvalin::CBody_Dvalin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Dvalin::CBody_Dvalin(const CBody_Dvalin& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Dvalin::Invalidate_Animation(_float fTimeDelta)
{
	if (bAnimStop == TRUE)
		return S_OK;

	FSM_DVALIN_STATE eState = static_cast<FSM_DVALIN_STATE>(m_pParentFSMCom->Get_CurrentState());

	_float fRate = 0.f;

	//fTimeDelta *= 2.f;

	switch (eState)
	{
	case Client::FSM_DVALIN_THINK:
		m_bIsFirstFrame = TRUE;
		break;
	case Client::FSM_DVALIN_FIRST:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_FIRST);
		//fRate = m_pModelCom->Play_DvalinRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_FIRST) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_IDLE);
		}
	}
	break;
	case Client::FSM_DVALIN_IDLE2:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_IDLE2);
		//fRate = m_pModelCom->Play_DvalinRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_IDLE2) * 0.95f)
		{
			//m_pParentFSMCom->Change_ForceState(FSM_DVALIN_ATTACK_GNAW_LEFT);
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_IDLE:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_IDLE);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_IDLE) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_GNAW_LEFT:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_GNAW_LEFT);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		//fRate = m_pModelCom->Play_DvalinRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_GNAW_LEFT) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_GNAW_RIGHT:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_GNAW_RIGHT);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_GNAW_RIGHT) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_BREATHELEFT:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_BREATHELEFT);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		//fRate = m_pModelCom->Play_DvalinRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		_matrix BreatheMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Breathe")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		BreatheMatrix.r[0] = XMVector3Normalize(BreatheMatrix.r[0]);
		BreatheMatrix.r[1] = XMVector3Normalize(BreatheMatrix.r[1]);
		BreatheMatrix.r[2] = XMVector3Normalize(BreatheMatrix.r[2]);
		XMStoreFloat4x4(&m_BreatheMatrix, BreatheMatrix);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_BREATHELEFT) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_BREATHERIGHT:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_BREATHERIGHT);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		//fRate = m_pModelCom->Play_DvalinRootAnimation(TRUE, m_pParentTransformCom, fT	imeDelta, m_vRootTranslation);

		_matrix BreatheMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Breathe")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		BreatheMatrix.r[0] = XMVector3Normalize(BreatheMatrix.r[0]);
		BreatheMatrix.r[1] = XMVector3Normalize(BreatheMatrix.r[1]);
		BreatheMatrix.r[2] = XMVector3Normalize(BreatheMatrix.r[2]);
		XMStoreFloat4x4(&m_BreatheMatrix, BreatheMatrix);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_BREATHERIGHT) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		CDvalin* pDvalin = static_cast<CDvalin*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss"));
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT) * 0.40f)
			pDvalin->Set_CanRot(TRUE);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT) * 0.42f)
			pDvalin->Set_CanRot(FALSE);
	
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT) * 0.99f)
		{ 
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		CDvalin* pDvalin = static_cast<CDvalin*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss"));
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK) * 0.40f)
			pDvalin->Set_CanRot(TRUE);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK) * 0.42f)
			pDvalin->Set_CanRot(FALSE);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_1:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		CDvalin* pDvalin = static_cast<CDvalin*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss"));
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK) * 0.40f)
			pDvalin->Set_CanRot(TRUE);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK) * 0.42f)
			pDvalin->Set_CanRot(FALSE);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK) * 0.85f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_2);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_2:
	{
		if (m_bIsFirstFrame == TRUE)
		{
			m_bIsFirstFrame = FALSE;
			m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT);
			m_pModelCom->Set_AnimationCurrentDuration(830.f);
			m_bRenderEnable = FALSE;
		}

		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT) * 0.3f)
			m_bRenderEnable = TRUE;


		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		
		if (m_iCameraSceneIndex == 0)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DVALIN, SCENE_ONE, 30.f, 10.f);
				m_iCameraSceneIndex += 1;
			}
		}
		
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
			m_iCameraSceneIndex = 0;
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_PULSE_BOMBS:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_PULSE_BOMBS);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_PULSE_BOMBS) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_FLOORCOLLAPSE:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_FLOORCOLLAPSE);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		
		if (m_iCameraSceneIndex == 0)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DVALIN, SCENE_TWO, 0.f, 55.f);
				m_iCameraSceneIndex += 1;
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_FLOORCOLLAPSE) * 0.9f)
		{
			CDvalin* pDvalin = static_cast<CDvalin*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss"));
			if (m_iIsFloorCollapseCount == 0 && bColl_1 == FALSE)
			{
				bColl_1 = TRUE;
				m_iIsFloorCollapseCount++;
				pDvalin->Set_IsFloorCollapse_1(TRUE);
			}
			else if (m_iIsFloorCollapseCount == 1 && bColl_2)
			{
				bColl_2 = TRUE;
				m_iIsFloorCollapseCount++;
				pDvalin->Set_IsFloorCollapse_2(TRUE);
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_FLOORCOLLAPSE) * 0.95f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
			}
			m_iCameraSceneIndex = 0;

			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_OMEGA_BOMB:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_OMEGA_BOMB);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_OMEGA_BOMB) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_STARFALL:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_ATTACK_STARFALL);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_ATTACK_STARFALL) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}
	}
	break;
	case Client::FSM_DVALIN_GROGGY_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_GROGGY_LOOP);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_GROGGY_LOOP) * 0.99f)
		{
			m_iAnimCount++;
		}

		if (m_iAnimCount >= 10.f)
		{
			m_iAnimCount = 0;
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
				m_iCameraSceneIndex = 0;
			}
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_GROGGY_AS);
		}

		if (false == m_bOnPlatform) 
		{
			m_bOnPlatform = true;

			_float4 vPlatformPos;
			XMStoreFloat4(&vPlatformPos, Get_BoneWorldPosition("Bone_Neck_03"));

			_float4 vDirection;
			XMStoreFloat4(&vDirection, Get_BoneWorldPosition("Climb03"));

			if (FAILED(m_pPlatform->Move_ToPlatform(vPlatformPos, vDirection)))
				return E_FAIL;
		}
	}
	break;
	case Client::FSM_DVALIN_GROGGY_AS:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_GROGGY_AS);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_GROGGY_AS) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_THINK);
		}

		if (true == m_bOnPlatform) 
		{
			m_bOnPlatform = false;

			_float4 vPlatformPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
			_float4 vDirection = _float4(0.0f, 0.0f, 1.0f, 1.0f);

			if (FAILED(m_pPlatform->Move_ToPlatform(vPlatformPos, vDirection)))
				return E_FAIL;
		}
	}
	break;
	case Client::FSM_DVALIN_GROGGY_BS:
	{
		m_pModelCom->Set_Animation(FSM_DVALIN_GROGGY_BS);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (m_iCameraSceneIndex == 0)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DVALIN, SCENE_THREE, 0.f, 10.f);
				m_iCameraSceneIndex += 1;
			}

			_vector vCenterPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			_vector vDvalinLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector vDvalinRight = m_pParentTransformCom->Get_State(CTransform::STATE_RIGHT);

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			//CCharacterController* pPlayerController = static_cast<CCharacterController*>(pPlayer->Get_Controller());

			_vector vFinalPos = vCenterPos + (vDvalinLook * 100.f) + (vDvalinRight * 3.f);

			_float4 f4FinalPos = {};
			XMStoreFloat4(&f4FinalPos, vFinalPos);

			//if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_UP))
			//pPlayer->Get_PlayerInfo(iIndex).pController->Set_Position(_float4(-101.f, 0.f, 3.f, 1.0f));
			pPlayer->Get_PlayerInfo(iIndex).pController->Set_Position(f4FinalPos);
			cout << "플레이어 순간이동" << endl;
		}
		else if (m_iCameraSceneIndex == 1 && fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_GROGGY_BS) * 0.48f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DVALIN, SCENE_FOUR, 0.f, 40.f);
				m_iCameraSceneIndex += 1;
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_GROGGY_BS) * 0.99f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_DVALIN_GROGGY_LOOP);
			m_iCameraSceneIndex = 0;
		}
	}
	break;
	case Client::FSM_DVALIN_DIE:
	{
		CDvalin* pDvalin = static_cast<CDvalin*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss"));
		m_pModelCom->Set_Animation(FSM_DVALIN_DIE);
		fRate = m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);

		if (m_iCameraSceneIndex == 0 /*&& fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_DIE) * 0.4f*/)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DVALIN, SCENE_FIVE, 0.f, 5.f);
				m_iCameraSceneIndex += 1;
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_DVALIN_DIE) * 0.99f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
				m_iCameraSceneIndex = 0;
			}
			pDvalin->Set_IsDying(TRUE);
		}

		m_bOnPlatform = false;

		_float4 vPlatformPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		_float4 vDirection = _float4(0.0f, 0.0f, 1.0f, 1.0f);

		if (FAILED(m_pPlatform->Move_ToPlatform(vPlatformPos, vDirection)))
			return E_FAIL;
	}
	break;
	case Client::FSM_DVALIN_END:
		break;
	default:
		break;
	}

	//// m_bInitRRAnimation 변수가 TRUE일 때 실행
	//if (m_bInitRRAnimation == TRUE)
	//{
	//	// 모델의 애니메이션 변환 비율이 1.0보다 큰 경우
	//	if (m_pModelCom->Get_ConvertRatio() > 1.f)
	//		m_bInitRRAnimation = FALSE; // 애니메이션이 초기화되었음을 나타내는 변수를 FALSE로 설정

	//	// "Bone001" 뼈의 현재 회전을 얻어옴
	//	_vector vReturunRot = m_pModelCom->Get_CurrBoneTransform("Bone001", ROOTBONETRANSFORMTYPE::ROTATION);

	//	// "Bone001" 뼈의 이전 프레임 회전 정보를 얻어옴
	//	KEYFRAME ePrevFrame = m_pModelCom->Get_PrevKeyFrame(m_iRRPrevState, "Bone001");

	//	// 현재 회전과 이전 프레임의 회전 간의 각도를 계산
	//	_float fRadian = CalculateQuaternionRotationAngle(vReturunRot, XMLoadFloat4(&ePrevFrame.vRotation));

	//	// 회전 방향에 따라 부모 오브젝트를 회전
	//	if (m_eRotationDir == ROT_LEFT)
	//		m_pParentTransformCom->RotationFromCurrentState(m_pParentTransformCom->Get_State(CTransform::STATE_UP), (m_fAccumulateRRRadian - fRadian)); // 왼쪽으로 회전
	//	else if (m_eRotationDir == ROT_RIGHT)
	//		m_pParentTransformCom->RotationFromCurrentState(m_pParentTransformCom->Get_State(CTransform::STATE_UP), (fRadian - m_fAccumulateRRRadian)); // 오른쪽으로 회전

	//	// 누적된 회전 각도를 업데이트
	//	m_fAccumulateRRRadian = fRadian;

	//	// 부모 오브젝트의 방향을 재설정
	//	m_pParentTransformCom->Reset_Look_At_Direction();
	//}

	return S_OK;
}

HRESULT CBody_Dvalin::Bind_ShaderResources_Trail()
{
	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTrailTextureCom->Bind_ShaderCurrentTexture(m_pTrailShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Dvalin::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	/*CBounding_SPHERE::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.fRadius = 1.5f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius * 0.5f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;*/

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Dvalin_EvillTex"),
		TEXT("Com_Texture_Dvalin"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	CBounding_SPHERE::BOUNDING_SPHERE_DESC		tSphereDesc = {};
	tSphereDesc.fRadius = 1.f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_LHAND"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_LHAND], &tSphereDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iBodyColliderID[DVALIN_COLL_LHAND], m_pBodyColliderCom[DVALIN_COLL_LHAND], this);

	tSphereDesc.fRadius = 1.f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_RHAND"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_RHAND], &tSphereDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iBodyColliderID[DVALIN_COLL_RHAND], m_pBodyColliderCom[DVALIN_COLL_RHAND], this);

	/*tSphereDesc.fRadius = 1.f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_HEAD"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_HEAD], &tSphereDesc)))
		return E_FAIL;*/
	BoundingDesc.vExtents = _float3(1.25f, 1.2f, 1.8f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB_HEAD"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_HEAD], &BoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iBodyColliderID[DVALIN_COLL_HEAD], m_pBodyColliderCom[DVALIN_COLL_HEAD], this);

	/*tSphereDesc.fRadius = 1.f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_NECK"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_NECK], &tSphereDesc)))
		return E_FAIL;*/
	BoundingDesc.vExtents = _float3(2.5f, 1.f, 2.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB_NECK"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_NECK], &BoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iBodyColliderID[DVALIN_COLL_NECK], m_pBodyColliderCom[DVALIN_COLL_NECK], this);

	BoundingDesc.vExtents = _float3(3.f, 1.5f, 1.5f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB_BODY"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_BODY], &BoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iBodyColliderID[DVALIN_COLL_BODY], m_pBodyColliderCom[DVALIN_COLL_BODY], this);
	
	BoundingDesc.vExtents = _float3(8.f, 2.1f, 3.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB_RUSH"), (CComponent**)&m_pBodyColliderCom[DVALIN_COLL_RUSH], &BoundingDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iBodyColliderID[DVALIN_COLL_RUSH], m_pBodyColliderCom[DVALIN_COLL_RUSH], this);

	return S_OK;
}

_vector CBody_Dvalin::Get_BoneWorldPosition(const _char* strBoneTag)
{
	_matrix Matrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix(strBoneTag)) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix);
	Matrix.r[0] = XMVector3Normalize(Matrix.r[0]);
	Matrix.r[1] = XMVector3Normalize(Matrix.r[1]);
	Matrix.r[2] = XMVector3Normalize(Matrix.r[2]);

	return Matrix.r[3];
}


HRESULT CBody_Dvalin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Dvalin::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pPlatform = CDvalinPlatform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pPlatform)
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Dvalin::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Dvalin::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	m_vTextureOffset.x -= fTimeDelta * 0.1f;
	m_vTextureOffset.y += fTimeDelta * 0.1f;

	//_matrix ColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bip001 L Finger2")) *
	//	XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
	//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
	//ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
	//ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
	//ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);

#pragma region 인게임에서 뼈 위치 보기 ( 디버그용 )
#ifdef _DEBUG
	//if (m_pGameInstance->Key_Pressing(VK_CONTROL)) /* Ctrl */
	//{
	//	if (m_pGameInstance->Key_Down('S'))
	//	{
	//		bAnimStop = !bAnimStop;
	//	}
	//	else if (m_pGameInstance->Key_Pressing(VK_OEM_PLUS))
	//	{
	//		_uint iNumBones = m_pModelCom->Get_NumBones();
	//		++m_iBoneIndex;
	//		if (m_iBoneIndex >= iNumBones)
	//			m_iBoneIndex = 0;
	//		cout << "Index : " << m_iBoneIndex << "/" << iNumBones - 1 << ",  Bone Name = " << m_pModelCom->Get_BoneName(m_iBoneIndex) << endl;
	//	}
	//	else if (m_pGameInstance->Key_Pressing(VK_OEM_MINUS))
	//	{
	//		_uint iNumBones = m_pModelCom->Get_NumBones();
	//		if (m_iBoneIndex == 0)
	//			m_iBoneIndex = iNumBones - 1;
	//		else
	//			--m_iBoneIndex;
	//		cout << "Index : " << m_iBoneIndex << "/" << iNumBones - 1 << ",  Bone Name = " << m_pModelCom->Get_BoneName(m_iBoneIndex) << endl;
	//	}
	//}
	//else
	//{
	//	if (m_pGameInstance->Key_Down(VK_OEM_PLUS))
	//	{
	//		_uint iNumBones = m_pModelCom->Get_NumBones();
	//		++m_iBoneIndex;
	//		if (m_iBoneIndex >= iNumBones)
	//			m_iBoneIndex = 0;
	//		cout << "Index : " << m_iBoneIndex << "/" << iNumBones - 1 << ",  Bone Name = " << m_pModelCom->Get_BoneName(m_iBoneIndex) << endl;
	//	}
	//	else if (m_pGameInstance->Key_Down(VK_OEM_MINUS))
	//	{
	//		_uint iNumBones = m_pModelCom->Get_NumBones();
	//		if (m_iBoneIndex == 0)
	//			m_iBoneIndex = iNumBones - 1;
	//		else
	//			--m_iBoneIndex;
	//		cout << "Index : " << m_iBoneIndex << "/" << iNumBones - 1 << ",  Bone Name = " << m_pModelCom->Get_BoneName(m_iBoneIndex) << endl;
	//	}
	//}

	//
	//_matrix ColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix(m_iBoneIndex)) *
	//	XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
	//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
	//ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
	//ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
	//ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);

	//const char* szName = m_pModelCom->Get_BoneName(m_iBoneIndex);

	//_float4 vDirection;
	//XMStoreFloat4(&vDirection, Get_BoneWorldPosition(szName));

	//m_pPlatform->Look_Platform(vDirection);

	//m_pColliderCom->Update(ColliderMatrix);
#endif
#pragma endregion

#pragma region 콜라이더 위치 업데이트
	_matrix ColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("hitbox_Seal01")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix);
	ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
	ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
	ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);

	m_pColliderCom->Update(ColliderMatrix);

	FSM_DVALIN_STATE eState = static_cast<FSM_DVALIN_STATE>(m_pParentFSMCom->Get_CurrentState());
	if (eState == FSM_DVALIN_GROGGY_LOOP)
	{
		_vector vPos = ColliderMatrix.r[3];
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
		CFiniteStateMachine* pPlayerFSM = static_cast<CFiniteStateMachine*>(pPlayer->Get_PlayerInfo(iIndex).pFiniteStateMachineCom);
		FSM_STATE eState = static_cast<FSM_STATE>(pPlayerFSM->Get_CurrentState());
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));

		_vector vSetDir = vPos - vPlayerPos;
		_float fDistance = XMVector3Length(vSetDir).m128_f32[0];

		if (fDistance > 1.f && fDistance < 3.f && 
			eState >= FSM_ATTACK_READY && eState <= FSM_ELEMENTAL_BURST_IDLE 
				&& eState != FSM_ELEMENTAL_BURST && eState != FSM_ELEMENTAL_BURST_IDLE && eState != FSM_SHOW)
		{
			vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);

			pPlayerTransform->Reset_Look_At_Direction();
			pPlayerTransform->Set_Look_At_Direction(vSetDir);
		}
	}
	if (m_bActivatedCollider == TRUE)
	{
		_matrix BodyColliderMatrix = {};
		BodyColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bip001 L Finger2")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pBodyColliderCom[DVALIN_COLL_LHAND]->Update(BodyColliderMatrix);


		BodyColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bip001 R Finger2")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pBodyColliderCom[DVALIN_COLL_RHAND]->Update(BodyColliderMatrix);


		BodyColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_Head_01")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pBodyColliderCom[DVALIN_COLL_HEAD]->Update(BodyColliderMatrix);


		BodyColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_Neck_02")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pBodyColliderCom[DVALIN_COLL_NECK]->Update(BodyColliderMatrix);


		BodyColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_Body_04")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pBodyColliderCom[DVALIN_COLL_BODY]->Update(BodyColliderMatrix);

		BodyColliderMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone_Body_04")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pBodyColliderCom[DVALIN_COLL_RUSH]->Update(BodyColliderMatrix);
	}
	else
	{
		_matrix BodyColliderMatrix = XMMatrixIdentity();
		m_pBodyColliderCom[DVALIN_COLL_LHAND]->Update(BodyColliderMatrix);
		m_pBodyColliderCom[DVALIN_COLL_RHAND]->Update(BodyColliderMatrix);
		m_pBodyColliderCom[DVALIN_COLL_HEAD]->Update(BodyColliderMatrix);
		m_pBodyColliderCom[DVALIN_COLL_NECK]->Update(BodyColliderMatrix);
		m_pBodyColliderCom[DVALIN_COLL_BODY]->Update(BodyColliderMatrix);
		m_pBodyColliderCom[DVALIN_COLL_RUSH]->Update(BodyColliderMatrix);
	}
#pragma endregion

	return S_OK;
}

HRESULT CBody_Dvalin::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	m_bPetrification = FALSE;

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CBody_Dvalin::Render()
{
	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++) 
	{
		if (siMesh == 4)
		{
			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_EFFECT_DVALIN_BODY)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP)))
				return E_FAIL;
			//if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
			//	return E_FAIL;

		}

		if (siMesh == 0 || siMesh == 1)
		{
			continue;
		}

		//m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)


		if (siMesh != 4)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
				continue;
			//return E_FAIL;
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bFaceMesh", &(bFaceMesh = (m_iFaceMeshIndex == siMesh)), sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
		{
			if (m_fDissolve == 0.f)
			{

				if (siMesh == 4)
				{
					if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_EvillTexture")))
						return E_FAIL;

					if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexutreOffset", &m_vTextureOffset, sizeof(_float2))))
						return E_FAIL;

					if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_EFFECT_DVALIN_BODY)))
						return E_FAIL;
				}
				else
				{
					if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
						return E_FAIL;

				}
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

		//if (siMesh != 4)
		//{
		////	if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
		////		return E_FAIL;
		//}
		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;

	}

#ifdef _DEBUG
	//m_pBodyColliderCom[DVALIN_COLL_LHAND]->Render();
	//m_pBodyColliderCom[DVALIN_COLL_RHAND]->Render();
	//m_pBodyColliderCom[DVALIN_COLL_HEAD]->Render();
	//m_pBodyColliderCom[DVALIN_COLL_NECK]->Render();
	//m_pBodyColliderCom[DVALIN_COLL_BODY]->Render();
	//m_pBodyColliderCom[DVALIN_COLL_RUSH]->Render();

	//if (FAILED(m_pPlatform->Render()))
	//	return E_FAIL;
#endif

	return S_OK;
}

HRESULT CBody_Dvalin::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_SHADOW)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

void CBody_Dvalin::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);

	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iBodyColliderID[DVALIN_COLL_LHAND]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iBodyColliderID[DVALIN_COLL_RHAND]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iBodyColliderID[DVALIN_COLL_HEAD]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iBodyColliderID[DVALIN_COLL_NECK]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iBodyColliderID[DVALIN_COLL_BODY]);
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iBodyColliderID[DVALIN_COLL_RUSH]);

}

CBody_Dvalin* CBody_Dvalin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Dvalin* pInstance = new CBody_Dvalin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Dvalin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Dvalin::Clone(void* pArg)
{
	CBody_Dvalin* pInstance = new CBody_Dvalin(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Dvalin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Dvalin::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	for (_uint i = 0; i < DVALIN_COLL_END; ++i)
	{
		Safe_Release(m_pBodyColliderCom[i]);
	}

	Safe_Release(m_pPlatform);
}