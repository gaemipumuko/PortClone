#include "stdafx.h"
#include "Body_Zhongli.h"

#include "SkillObj.h"
#include "Camera_Main.h"
#include "FX_ShockWave.h"
#include "Part_Status.h"
#include "GameInstance.h"

#define	HAIR_MESH_INDEX 0

CBody_Zhongli::CBody_Zhongli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Zhongli::CBody_Zhongli(const CBody_Zhongli& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Zhongli::Invalidate_Animation(_float fTimeDelta)
{
	if (m_bFreezing == TRUE)
	{
		return S_OK;
	}

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	FSM_STATE ePrevState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState());

	PLAYERINDEX iIndex = static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_ZHONGLI);
	if (static_cast<CPlayer*>(m_pParent)->Get_PlayerInfo(iIndex).bAutoMode == TRUE) /* ÀÚµ¿ÀüÅõ ¸ðµå */
	{
		_bool bRun = static_cast<CPlayer*>(m_pParent)->Get_PlayerInfo(iIndex).bRun;
		if ((eState < FSM_ATTACK_READY || eState > FSM_ATTACK6) && bRun == TRUE)
			m_pParentFSMCom->Change_ForceState(FSM_RUN);

		switch (eState)
		{
#pragma region ¿òÁ÷ÀÓ
		case FSM_IDLE:
		{
			if (m_bLerpIdle)
				m_pModelCom->Set_ConvertSpeed(2.f);
			else
				m_pModelCom->Set_ConvertSpeed(5.f);

			m_pModelCom->Set_Animation(20);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.03f);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(20) * 0.5f))
				m_bLerpIdle = FALSE;

			if (fRate >= (m_pModelCom->Get_AnimationDuration(20) * 0.9f))
			{
				m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(22) * 0.f);
				m_pParentFSMCom->Change_ForceState(FSM_IDLE);
				m_bLerpIdle = FALSE;
			}
		}
		break;

		case FSM_ATTACK_IDLE:
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(70);
			m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
			m_bLerpIdle = TRUE;
			break;

		case FSM_RUN:
		{
			m_pModelCom->Set_ConvertSpeed(15.f);
			m_pModelCom->Set_Animation(21);
			m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		}
		break;
#pragma endregion

#pragma region ÆòÅ¸
		case FSM_ATTACK_READY:
		case FSM_ATTACK1:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(23);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(23) * 0.01f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(23)) * 0.18f)
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK2);
			}
		}
		break;

		case FSM_ATTACK2:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(24);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(24) * 0.01f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(24) * 0.18f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK3);
			}
		}
		break;

		case FSM_ATTACK3:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(25);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(25) * 0.01f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(25) * 0.18f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK4);
			}
		}
		break;

		case FSM_ATTACK4:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(26);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(26) * 0.01f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if ( fRate >= (m_pModelCom->Get_AnimationDuration(26) * 0.15f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK5);
			}
		}
		break;

		case FSM_ATTACK5:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(27);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(27) * 0.01f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}
			if (fRate >= (m_pModelCom->Get_AnimationDuration(27) * 0.4f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK6);
			}
		}
		break;

		case FSM_ATTACK6:
		{
			m_pModelCom->Set_ConvertSpeed(3.f);
			m_pModelCom->Set_Animation(28);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.01f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (bRun == FALSE && 
				fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.25f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
			else if (bRun == TRUE && 
				fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.25f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_RUN);
			}
		}
		break;
#pragma endregion

#pragma region ³«°ø
		case FSM_FALL_ATTACK:
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(37);
			m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 2.);
			
			if (m_pParentController->Get_InGround() == TRUE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_END1);
			}
			break;

		case FSM_FALL_ATTACK_END1:
		{
			m_pModelCom->Set_ConvertSpeed(100.f);
			m_pModelCom->Set_Animation(38);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta * 1.5f, m_vRootTranslation);

			if (fRate == 0.f)
			{
				CFX_ShockWave::SW_DESC tSWDesc = { };
				tSWDesc.fScale = 0.5f;
				tSWDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
				tSWDesc.vInitPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
				tSWDesc.fDuration = 0.5f;

				m_pGameInstance->Play_AssetFX(L"ShockWave", Get_ParticleTag(L"ShockWave"), &tSWDesc);

				m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(38) * 0.3f);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(38) * 0.15f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_END2);
				m_fFlyMargin = 2.5f;
			}
		}
		break;

		case FSM_FALL_ATTACK_END2:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(39);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(39) * 0.4f))
			{
				m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(39)))
			{
				m_pParentFSMCom->Change_ForceState(FSM_IDLE);
			}
		}
		break;
#pragma endregion

#pragma region ÇÇ°Ý
		case FSM_HIT_L:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(43);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(43)))
			{
				m_pModelCom->Set_ConvertSpeed(5.f);
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
		}
		break;

		case FSM_HIT_H:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(65);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(65) * 0.f && fRate < m_pModelCom->Get_AnimationDuration(65) * 0.1f)
			{
				m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
			}


			if (fRate >= m_pModelCom->Get_AnimationDuration(65))
			{
				m_pModelCom->Set_ConvertSpeed(5.f);
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
		}
		break;

		case FSM_HIT_THROW:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(86);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(86) || m_pParentController->Get_InAir() == FALSE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_HIT_THROW_FLY);
			}
		}
		break;

		case FSM_HIT_THROW_FLY:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(87);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (m_pParentController->Get_InGround() == TRUE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_HIT_ROLLING);
				break;
			}
		}
		break;

		case FSM_HIT_ROLLING:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(85);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate == 0.f)
				m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(85) * 0.1f);

			if (fRate >= m_pModelCom->Get_AnimationDuration(85))
			{
				m_pModelCom->Set_ConvertSpeed(5.f);
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
		}

		break;

#pragma endregion

#pragma region Á×À½
		case FSM_DEAD1:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(4);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(4))
			{
				if (m_fDissolve < 1.f)
					m_fDissolve += 0.01f;
				else
					Set_Dead(TRUE);
			}
		}

		break;


		case FSM_DEAD2:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(63);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(63))
			{
				if (m_fDissolve < 1.f)
					m_fDissolve += 0.01f;
				else
					Set_Dead(TRUE);
			}
		}

#pragma endregion
		}
		return S_OK;
	}

	if ((ePrevState == FSM_RUN || ePrevState == FSM_WALK) && eState == FSM_SPLINT)
	{
		m_bChangeAnim = TRUE;
		eState = ePrevState;
		m_pParentFSMCom->Change_ForceState(ePrevState);
	}
	else if (ePrevState == FSM_SPLINT && (eState == FSM_SPLINT_RUN || eState == FSM_RUN_WALK))
	{
		m_bChangeAnim = TRUE;
		eState = ePrevState;
		m_pParentFSMCom->Change_ForceState(ePrevState);
	}
	else if (ePrevState == FSM_WALK && eState == FSM_WALK_RUN)
	{
		m_bChangeAnim = TRUE;
		eState = ePrevState;
		m_pParentFSMCom->Change_ForceState(ePrevState);
	}
	else if (ePrevState == FSM_RUN && eState == FSM_RUN_WALK)
	{
		m_bChangeAnim = TRUE;
		eState = ePrevState;
		m_pParentFSMCom->Change_ForceState(ePrevState);
	}
	else if (ePrevState == FSM_DASH && (eState == FSM_SPLINT_RUN || eState == FSM_RUN_WALK))
	{
		m_bChangeAnim = TRUE;
		/*m_pParentFSMCom->Change_ForceState(eState);
		ePrevState = FSM_DASH;*/
		eState = ePrevState;
		m_pParentFSMCom->Change_ForceState(ePrevState);
	}

	switch (eState)
	{
#pragma region ¿òÁ÷ÀÓ
	case FSM_IDLE:
	{
		if(m_bLerpIdle)
			m_pModelCom->Set_ConvertSpeed(2.f);
		else
			m_pModelCom->Set_ConvertSpeed(5.f);

		m_pModelCom->Set_Animation(20);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.03f);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(20) * 0.5f))
			m_bLerpIdle = FALSE;

		if (fRate >= (m_pModelCom->Get_AnimationDuration(20) * 0.9f))
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(22) * 0.f);
			m_pParentFSMCom->Change_ForceState(FSM_IDLE);
			m_bLerpIdle = FALSE;
		}
	}
	break;

	case FSM_WALK:
	{
		m_pModelCom->Set_ConvertSpeed(10.f);
		m_pModelCom->Set_Animation(22);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		m_bLerpIdle = TRUE;

		if (m_bChangeAnim == TRUE &&
			fRate >= m_pModelCom->Get_AnimationDuration(22) * 0.11f && fRate <= m_pModelCom->Get_AnimationDuration(22) * 0.13f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(22) * 0.89f))
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(22) * 0.f);
			m_pModelCom->Reset_EventCaller();
			m_pParentFSMCom->Change_ForceState(FSM_WALK);
		}
	}
	break;

	case FSM_RUN:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(21);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_SPLINT &&
			fRate >= m_pModelCom->Get_AnimationDuration(21) * 0.2f && fRate <= m_pModelCom->Get_AnimationDuration(21) * 0.22f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
		if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_RUN_WALK &&
			fRate >= m_pModelCom->Get_AnimationDuration(21) * 0.82f && fRate <= m_pModelCom->Get_AnimationDuration(75) * 0.84f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
	}
	break;

	case FSM_SPLINT:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(14);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE &&
			fRate >= m_pModelCom->Get_AnimationDuration(14) * 0.3f && fRate <= m_pModelCom->Get_AnimationDuration(14) * 0.32f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
	}
	break;

	case FSM_DASH:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(13);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_SPLINT_RUN &&
			fRate >= m_pModelCom->Get_AnimationDuration(13) * 0.26f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
		else if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_RUN_WALK &&
			fRate >= m_pModelCom->Get_AnimationDuration(13) * 0.3f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}

		if (fRate == 0.f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
			{
				pCamera->Set_ActionMode(2.f, FALSE);
			}
		}
		m_bChangeAnim = FALSE;
		if ((m_bChangeAnim == FALSE) && fRate >= m_pModelCom->Get_AnimationDuration(13) * 0.3f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_RUN_STOP);
		}
	}
	break;

	case FSM_JUMP:
		m_pModelCom->Set_ConvertSpeed(10.f);
		m_pModelCom->Set_Animation(88);
		m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.f);
	break;

	case FSM_WALK_RUN:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(10);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(10) * 0.78f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_RUN);
		}
	}
	break;

	case FSM_WALK_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(19);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(19)))
		{
			m_bLerpIdle = TRUE;
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;

	case FSM_RUN_WALK:
	{
		m_pModelCom->Set_ConvertSpeed(10.f);
		m_pModelCom->Set_Animation(12);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(12) * 0.3f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_WALK);
		}
	}
	break;

	case FSM_RUN_STOP:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(11);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		
		if (fRate <= (m_pModelCom->Get_AnimationDuration(11) * 0.1f))
		{
			_vector vPlayerLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);
			vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);

			_float4 PlayerLook;
			XMStoreFloat4(&PlayerLook, vPlayerLook);

			m_pParentController->Movement(fTimeDelta, PlayerLook, 2.0f);
		}
		
		if (fRate >= (m_pModelCom->Get_AnimationDuration(11)))
		{
			m_bLerpIdle = TRUE;
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;

	case FSM_SPLINT_RUN:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(15);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(15) * 0.68f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_RUN);
		}
	}
	break;
#pragma endregion

#pragma region °øÁß
	case FSM_FALL_LOW:
	{
		m_pModelCom->Set_ConvertSpeed(20.f);
		m_pModelCom->Set_Animation(72);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.f);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(72) * 0.5f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;

	case FSM_FLY_START:
	{
		m_pModelCom->Set_ConvertSpeed(60.f);
		m_pModelCom->Set_Animation(82);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(82) * 0.2f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_FLY_NORMAL);
		}
	}
	break;

	case FSM_FLY_NORMAL:
		m_pModelCom->Set_ConvertSpeed(2.f);
		m_pModelCom->Set_Animation(79);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (m_pGameInstance->Key_Down(VK_SPACE))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_DROP_DOWN);
		}
		if (m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_pModelCom->Set_ConvertSpeed(10.f);
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_START1);
		}
	break;

	case FSM_FLY_LEFT:
		m_pModelCom->Set_ConvertSpeed(2.f);
		m_pModelCom->Set_Animation(83);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (m_pGameInstance->Key_Down(VK_SPACE))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_DROP_DOWN);
		}
		if (m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_pModelCom->Set_ConvertSpeed(10.f);
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_START1);
		}
		break;

	case FSM_FLY_RIGHT:
		m_pModelCom->Set_ConvertSpeed(2.f);
		m_pModelCom->Set_Animation(84);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (m_pGameInstance->Key_Down(VK_SPACE))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_DROP_DOWN);
		}
		if (m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_pModelCom->Set_ConvertSpeed(10.f);
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_START1);
		}
	break;

	case FSM_DROP_DOWN:
		m_pModelCom->Set_ConvertSpeed(2.f);
		m_pModelCom->Set_Animation(5);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	break;

	case FSM_FALL_ATTACK_START1:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(40);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(40) * 0.5f))
		{
			m_pModelCom->Set_ConvertSpeed(15.f);
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_START2);
		}
	}
	break;
	case FSM_FALL_ATTACK_START2:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(36);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(36)))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK);
		}
	}
	break;

	case FSM_FALL_ATTACK:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(37);
		m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.5f);
	break;

	case FSM_FALL_ATTACK_END1:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(38);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta * 1.5f, m_vRootTranslation);

		if (fRate <= (m_pModelCom->Get_AnimationDuration(38) * 0.3f))
			m_fFlyMargin -= fTimeDelta * 10.f;
		else if (fRate >= (m_pModelCom->Get_AnimationDuration(38) * 0.3f) || m_fFlyMargin <= 0.8f)
		{
			//m_fFlyMargin = 0.8f;
			m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);
		}

		/*if (fRate >= (m_pModelCom->Get_AnimationDuration(38) * 0.3f))
		{
			m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation, 0.8f);
		}*/

		if (fRate >= (m_pModelCom->Get_AnimationDuration(38) * 0.15f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_END2);
			m_fFlyMargin = 2.5f;
		}
	}
	break;

	case FSM_FALL_ATTACK_END2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(39);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(39) * 0.4f))
		{
			m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(39)))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_IDLE);
		}
	}
	break;
#pragma endregion

#pragma region ÆòÅ¸, °­°ø°Ý
	case FSM_ATTACK_IDLE:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(70);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		m_bLerpIdle = TRUE;
		break;

	case FSM_ATTACK_READY:
	case FSM_ATTACK1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(23);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(23) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}
		
		if (fRate == 0.f && fRate <= (m_pModelCom->Get_AnimationDuration(23) * 0.18f) && m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedReady = true;
		else if (fRate > 0.f && m_bChargedReady && fRate <= (m_pModelCom->Get_AnimationDuration(23) * 0.18f) && m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedReady = true;
		else if (fRate <= (m_pModelCom->Get_AnimationDuration(23) * 0.18f) && !m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedReady = false;

		if (fRate >= (m_pModelCom->Get_AnimationDuration(23)) * 0.18f)
		{
			if (m_bAttack && !m_bChargedReady)
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK2);
				m_bAttack = FALSE;
			}
			else if(m_bChargedReady)
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED1);
				m_bAttack = FALSE;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(23) * 0.8f))
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
	}
	break;

	case FSM_ATTACK2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(24);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(24) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(24) * 0.18f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK3);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(24) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK3:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(25);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(25) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(25) * 0.18f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK4);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(25) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK4:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(26);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(26) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(26) * 0.15f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK5);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(26) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK5:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(27);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(27) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(27) * 0.4f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK6);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(27) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK6:
	{
		m_pModelCom->Set_ConvertSpeed(3.f);
		m_pModelCom->Set_Animation(28);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.25f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.8f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK_CHARGED1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(34);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bFallingSpear == FALSE)
		{
			m_bFallingSpear = TRUE;
			PxQueryFilterData tQueryFilterData = PxQueryFilterData();
			tQueryFilterData.flags = PxQueryFlag::eSTATIC;

			CSkillObj::SKILLOBJ_DESC tDesc = {};
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
			_vector vStartPos = pPlayer->Get_Position(PLAYER_TWO);
			_vector vDir = XMVector3Normalize(pPlayer->Get_Look(PLAYER_TWO));
			for (_uint i = 0; i < 5; ++i)
			{
				_vector vPos = vStartPos + vDir * 1.6f * (_float)i + XMVectorSet(0.f, 5.f, 0.f, 0.f);
				_float3	vTestPos = {};
				XMStoreFloat3(&vTestPos, vPos);
				m_pGameInstance->RayCast_Position(vTestPos, _float3(0.f, -1.f, 0.f), 10.f, &vTestPos, tQueryFilterData);
				tDesc.vSpawnPos = XMVectorSet(vTestPos.x, vTestPos.y, vTestPos.z, 1.f);
				tDesc.fCreateDelay = 0.1f * (_float)i;
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_FallingSpear", L"Layer_Skill", &tDesc)))
					return E_FAIL;
			}
		}
		

		if (fRate >= (m_pModelCom->Get_AnimationDuration(34) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate == 0.f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ActionMode(2.f);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(34)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED2);
			m_bFallingSpear = FALSE;
		}
	}
	break;

	case FSM_ATTACK_CHARGED2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(35);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		/*if (fRate >= (m_pModelCom->Get_AnimationDuration(35) * 0.01f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}*/

		if (fRate >= (m_pModelCom->Get_AnimationDuration(35) * 0.95f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_END);
		}
	}
	break;

	case FSM_ATTACK_END:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(66);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(66) * 0.95f))
		{
			m_bLerpIdle = TRUE;
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;
#pragma endregion

#pragma region ¿ø¼ÒÀüÅõ
	case FSM_ELEMENTAL_SKILL1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(31);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(31)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL1_HOLD);
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL1_HOLD:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(29);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bCameraControl && fRate >= m_pModelCom->Get_AnimationDuration(29) * 0.16f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				CSkillObj::SKILLOBJ_DESC tDesc = {};
				tDesc.fDamage = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_ElementDamage();
				tDesc.vDir = XMVector3Normalize(XMVectorSet(m_CombinedWorldMatrix._31, 0.f, m_CombinedWorldMatrix._33, 0.f));
				tDesc.vSpawnPos = XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, m_CombinedWorldMatrix._44) + tDesc.vDir * 5.f;
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_Pillar", L"Layer_Skill", &tDesc)))
					return E_FAIL;

				tDesc = {};
				tDesc.vDir = XMVector3Normalize(XMVectorSet(m_CombinedWorldMatrix._31, 0.f, m_CombinedWorldMatrix._33, 0.f));
				tDesc.vSpawnPos = XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, m_CombinedWorldMatrix._44);
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_Zhongli_Shield", L"Layer_Skill", &tDesc)))
					return E_FAIL;

				pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				m_bCameraControl = FALSE;

				_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
				static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(29) * 0.85f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL1_IDLE);
			m_bCameraControl = TRUE;
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL1_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(30);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(30) * 0.2f))
		{
			if (m_pGameInstance->Key_Down(VK_LBUTTON))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(30) * 0.4f))
		{
			if (m_pGameInstance->Key_Pressing('W') == TRUE 
				|| m_pGameInstance->Key_Pressing('A') == TRUE
				|| m_pGameInstance->Key_Pressing('S') == TRUE
				|| m_pGameInstance->Key_Pressing('D') == TRUE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_WALK);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(30)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;

	case FSM_ELEMENTAL_BURST:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(32);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		_float Epsilon = 0.01f;

		if (m_iCameraScene == 0)
		{
			// ±Ã ÄÆ¾À
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				_vector vPlayerLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);
				_vector vPlayerRight = m_pParentTransformCom->Get_State(CTransform::STATE_RIGHT);
				_vector vLook = XMVector3Normalize(vPlayerLook);
				_vector vRight = XMVector3Normalize(vPlayerRight);

				CSkillObj::SKILLOBJ_DESC tDesc = {};
				tDesc.fDamage = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_BurstDamage();
				//tDesc.vDir = XMVector3Normalize(XMVectorSet(m_CombinedWorldMatrix._31, 0.f, m_CombinedWorldMatrix._33, 0.f));
				tDesc.vDir = XMVector3Normalize(vPlayerLook)*-1.f;
				vLook.m128_f32[1] = 0.f;
				tDesc.vSpawnPos = XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, m_CombinedWorldMatrix._44);
				tDesc.vSpawnPos += vLook*4.f;
				//tDesc.vSpawnPos -= vRight * 3.f;
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_FallingStone", L"Layer_Skill", &tDesc)))
					return E_FAIL;
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_FallingStoneTrail", L"Layer_Skill", &tDesc)))
					return E_FAIL;


				tDesc = {};
				tDesc.vDir = XMVector3Normalize(vPlayerLook);
				vLook.m128_f32[1] = 0.f;
				tDesc.vSpawnPos = XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42+35.f, m_CombinedWorldMatrix._43, m_CombinedWorldMatrix._44);
				tDesc.vSpawnPos -= vLook * 6.f;
				tDesc.vSpawnPos -= vRight * 3.f;
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_BurstBackGround", L"Layer_Skill", &tDesc)))
					return E_FAIL;

				pCamera->Set_SceneMode(CAMSCENEID_ZHONGLI, SCENE_ONE, 0.2f, 0.2f);
				m_iCameraScene += 1;
			}
		}
		else if (m_iCameraScene == 1 && fRate >= m_pModelCom->Get_AnimationDuration(32) * 0.64f)
		{
			// ±Ã ÄÆ¾À
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_ZHONGLI, SCENE_TWO, 2.f, 1.f);
				m_iCameraScene += 1;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(33) * 0.75f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_BURST_IDLE);
			m_iCameraScene = 0;
		}
	}
	break;

	case FSM_ELEMENTAL_BURST_IDLE: 
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(33);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(33) * 0.9f))
		{
			if (m_pGameInstance->Key_Pressing('W') == TRUE
				|| m_pGameInstance->Key_Pressing('A') == TRUE
				|| m_pGameInstance->Key_Pressing('S') == TRUE
				|| m_pGameInstance->Key_Pressing('D') == TRUE)
			{
				m_pModelCom->Set_ConvertSpeed(5.f);
				m_pParentFSMCom->Change_ForceState(FSM_WALK);
			}

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
			{
				pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
			}

			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;
#pragma endregion

#pragma region ÇÇ°Ý
	case FSM_HIT_L:
	{
		m_bCameraControl = TRUE;
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(43);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(43) * 0.9f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;

	case FSM_HIT_H:
	{
		m_bCameraControl = TRUE;
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(65);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(65) * 0.f && fRate < m_pModelCom->Get_AnimationDuration(65) * 0.1f)
		{
			m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		}


		if (fRate >= m_pModelCom->Get_AnimationDuration(65) * 0.9f)
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;

	case FSM_HIT_THROW:
	{
		m_bCameraControl = TRUE;
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(86);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(86) || m_pParentController->Get_InAir() == FALSE)
		{
			m_pParentFSMCom->Change_ForceState(FSM_HIT_THROW_FLY);
		}
	}
	break;

	case FSM_HIT_THROW_FLY:
	{
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(87);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_pParentController->Get_InGround() == TRUE)
		{
			m_pParentFSMCom->Change_ForceState(FSM_HIT_ROLLING);
			break;
		}
	}
	break;

	case FSM_HIT_ROLLING:
	{
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(85);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate == 0.f)
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(85) * 0.1f);

		if (fRate >= m_pModelCom->Get_AnimationDuration(85))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}

	break;
	
#pragma endregion

#pragma region Á×À½
	case FSM_DEAD1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(4);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(4))
		{
			if (m_fDissolve == 0.f)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Zhongli_Fallen_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}

			if (m_fDissolve < 1.f)
				m_fDissolve += 0.01f;
			else
				Set_Dead(TRUE);
		}
	}

	break;


	case FSM_DEAD2:
	{
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(63);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(63))
		{
			if (m_fDissolve < 1.f)
				m_fDissolve += 0.01f;
			else
				Set_Dead(TRUE);
		}
	}

	break;

#pragma endregion

	case FSM_SHOW:
	{
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(41);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.6f, m_vRootTranslation);

		if (fRate == 0.f)
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(41) * 0.66f);

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
			{
				pCamera->Set_SceneMode(CAMSCENEID_CHANGE, SCENE_ONE, 1.5f, 0.3f);
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(41) * 0.88f)
		{
			m_pParentFSMCom->Set_IdleState();

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
			{
				pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
			}
		}
	}

	}

	return S_OK;
}

HRESULT CBody_Zhongli::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_BODY, &m_iColliderID, m_pColliderCom, this);
	m_pGameInstance->Add_Collider_Object(COLL_EVENTREACT_PLAYER, &m_iColliderID, m_pColliderCom, this);
	m_iColliderID -= 1;
	m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_iColliderID + 1, FALSE);

	return S_OK;
}

HRESULT CBody_Zhongli::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zhongli::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_vDissolveColor = _float3(0.f, 0.5f, 1.f);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	return S_OK;
}

HRESULT CBody_Zhongli::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zhongli::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zhongli::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zhongli::Render()
{
	if (m_pParentFSMCom->Get_CurrentState() == FSM_ATTACK_CHARGED1)
		return S_OK;

	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool	bLightMap = { };
	_bool	bFaceMesh = { };
	_float	fRampIndex = { -1.f };
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

		if (siMesh == m_iFaceMeshIndex || siMesh == m_iFaceEyeMeshIndex)
			bFaceMesh = TRUE;
		else
			bFaceMesh = FALSE;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bFaceMesh", &bFaceMesh, sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRampIndex",
			&(fRampIndex = (siMesh != HAIR_MESH_INDEX ? (m_eCreatureID * 2.f) : ((m_eCreatureID * 2.f) + 1.f))), sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bFreezing == TRUE) // ºù°á
		{
			if (FAILED(m_pFreezingCom->Bind_ShaderResource(m_pShaderCom, "g_FreezingDiffuseTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pFreezingCom->Bind_ShaderResource(m_pShaderCom, "g_FreezingNoiseTexture", 1)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_FREEZING)))
				return E_FAIL;
		}
		else if (m_bPetrification == TRUE) // ¼®È­
		{
			if (FAILED(m_pPetrificationCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_PetrificationTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PETRIFICATION)))
				return E_FAIL;
		}
		else
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

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

void CBody_Zhongli::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_PLAYER_BODY, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Zhongli Body COLL_PLAYER_BODY" << endl;
	else
		cout << "Release Zhongli Body COLL_PLAYER_BODY" << endl;

	hr = m_pGameInstance->Release_Collider_Object(COLL_EVENTREACT_PLAYER, m_iColliderID + 1);

	if (FAILED(hr))
		cout << "Failed Zhongli Body COLL_EVENTREACT_PLAYER" << endl;
	else
		cout << "Release Zhongli Body COLL_EVENTREACT_PLAYER" << endl;
}

CBody_Zhongli* CBody_Zhongli::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Zhongli* pInstance = new CBody_Zhongli(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Zhongli"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Zhongli::Clone(void* pArg)
{
	CBody_Zhongli* pInstance = new CBody_Zhongli(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Zhongli"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Zhongli::Free()
{
	__super::Free();
}
