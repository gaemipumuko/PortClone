#include "stdafx.h"
#include "Body_Diluc.h"

#include "SkillObj.h"
#include "Camera_Main.h"
#include "FX_ShockWave.h"

#include "GameInstance.h"
#include "Part_Status.h"

#define	HAIR_MESH_INDEX 0

CBody_Diluc::CBody_Diluc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Diluc::CBody_Diluc(const CBody_Diluc& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Diluc::Invalidate_Animation(_float fTimeDelta)
{
	if (m_bFreezing == TRUE)
	{
		return S_OK;
	}

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	FSM_STATE ePrevState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState());

	PLAYERINDEX iIndex = static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_DILUC);
	if (static_cast<CPlayer*>(m_pParent)->Get_PlayerInfo(iIndex).bAutoMode == TRUE) /* 자동전투 모드 */
	{
		_bool bRun = static_cast<CPlayer*>(m_pParent)->Get_PlayerInfo(iIndex).bRun;
		if ((eState < FSM_ATTACK_READY || eState > FSM_ATTACK6) && bRun == TRUE)
			m_pParentFSMCom->Change_ForceState(FSM_RUN);

		switch (eState)
		{
#pragma region 움직임
		case FSM_IDLE:
			m_pModelCom->Set_ConvertSpeed(2.f);
			m_pModelCom->Set_Animation(74);
			m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.05f);
			break;

		case FSM_ATTACK_IDLE:
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(106);
			m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
			break;

		case FSM_RUN:
		{
			m_pModelCom->Set_ConvertSpeed(15.f);
			m_pModelCom->Set_Animation(75);
			m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		}
		break;
#pragma endregion

#pragma region 평타
		case FSM_ATTACK_READY:
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
			break;
		case FSM_ATTACK1:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(77);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate == 0.f)
				m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(77) * 0.13f);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(77) * 0.1f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(77) * 0.16f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK2);
			}
		}
		break;

		case FSM_ATTACK2:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(79);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(79) * 0.1f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(79) * 0.26f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK3);
			}
		}
		break;

		case FSM_ATTACK3:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(80);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(80) * 0.05f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(80) * 0.26f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK4);
			}
		}
		break;

		case FSM_ATTACK4:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(81);
			m_vPrevRootTranslation = m_vRootTranslation;
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(81) * 0.1f))
			{
				_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
				Set_ControllerTranslation(vCurrentTranslation);
			}

			if (bRun == FALSE && fRate >= (m_pModelCom->Get_AnimationDuration(81) * 0.42f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
			}
			else if (bRun == TRUE && fRate >= (m_pModelCom->Get_AnimationDuration(81) * 0.42f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_RUN);
			}
		}
		break;
#pragma endregion

#pragma region 낙공
		case FSM_FALL_ATTACK:
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(94);
			m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.2f);
			
			if (m_pParentController->Get_InGround() == TRUE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_END1);
			}
			break;

		case FSM_FALL_ATTACK_END1:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(95);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.5f, m_vRootTranslation);

			if (m_bInitEffect == FALSE && fRate == 0.f)
			{
				CFX_ShockWave::SW_DESC tSWDesc = { };
				tSWDesc.fScale = 0.5f;
				tSWDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
				tSWDesc.vInitPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
				tSWDesc.fDuration = 0.5f;

				m_pGameInstance->Play_AssetFX(L"ShockWave", Get_ParticleTag(L"ShockWave"), &tSWDesc);
				//m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(38) * 0.3f);
				m_bInitEffect = TRUE;
			}

			if (fRate >= (m_pModelCom->Get_AnimationDuration(95) * 0.15f))
			{
				m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_END2);
				m_bInitEffect = FALSE;
			}
		}
		break;

		case FSM_FALL_ATTACK_END2:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(96);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= (m_pModelCom->Get_AnimationDuration(96)))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			}
		}
		break;
#pragma endregion

#pragma region 피격
		case FSM_HIT_L:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(101);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(101))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
		}
		break;

		case FSM_HIT_H:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(100);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(100) * 0.f && fRate < m_pModelCom->Get_AnimationDuration(100) * 0.1f)
			{
				m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 0.8f);
			}

			if (fRate >= m_pModelCom->Get_AnimationDuration(100))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
		}
		break;

		case FSM_HIT_THROW:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(45);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(45) || m_pParentController->Get_InAir() == FALSE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_HIT_THROW_FLY);
			}
		}
		break;

		case FSM_HIT_THROW_FLY:
		{
			m_pModelCom->Set_ConvertSpeed(50.f);
			m_pModelCom->Set_Animation(54);
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
			m_pModelCom->Set_Animation(44);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate == 0.f)
				m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(44) * 0.07f);

			if (fRate >= m_pModelCom->Get_AnimationDuration(44))
			{
				m_pModelCom->Set_ConvertSpeed(5.f);
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_READY);
			}
		}

		break;

#pragma endregion

#pragma region 죽음
		case FSM_DEAD1:
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(23);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(23))
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
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pModelCom->Set_Animation(24);
			_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

			if (fRate >= m_pModelCom->Get_AnimationDuration(24))
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

	/* 이전상태 체크 */
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
		eState = ePrevState;
		m_pParentFSMCom->Change_ForceState(ePrevState);
	}

	switch (eState)
	{
#pragma region 움직임
	case FSM_IDLE:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(74);
		m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.05f);
		break;

	case FSM_WALK:
	{
		m_pModelCom->Set_ConvertSpeed(10.f);
		m_pModelCom->Set_Animation(76);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE &&
			fRate >= m_pModelCom->Get_AnimationDuration(76) * 0.11f && fRate <= m_pModelCom->Get_AnimationDuration(76) * 0.13f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(76) * 0.86f))
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(76) * 0.f);
			m_pModelCom->Reset_EventCaller();
			m_pParentFSMCom->Change_ForceState(FSM_WALK);
		}
	}
	break;

	case FSM_RUN:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(75);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_SPLINT &&
			fRate >= m_pModelCom->Get_AnimationDuration(75) * 0.2f && fRate <= m_pModelCom->Get_AnimationDuration(75) * 0.22f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
		if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_RUN_WALK &&
			fRate >= m_pModelCom->Get_AnimationDuration(75) * 0.82f && fRate <= m_pModelCom->Get_AnimationDuration(75) * 0.84f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
	}
		break;

	case FSM_SPLINT:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(68);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE && 
			fRate >= m_pModelCom->Get_AnimationDuration(68) * 0.3f && fRate <= m_pModelCom->Get_AnimationDuration(68) * 0.31f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(68) * 0.9f))
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(68) * 0.f);
			m_pModelCom->Reset_EventCaller();
			m_pParentFSMCom->Change_ForceState(FSM_SPLINT);
		}
	}
	break;

	case FSM_DASH:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(67);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_SPLINT_RUN &&
			fRate >= m_pModelCom->Get_AnimationDuration(67) * 0.26f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}
		else if (m_bChangeAnim == TRUE && static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()) == FSM_RUN_WALK &&
			fRate >= m_pModelCom->Get_AnimationDuration(67) * 0.3f)
		{
			m_bChangeAnim = FALSE;
			m_pParentFSMCom->Change_ForceState(static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState()));
		}

		if (fRate == 0.f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ActionMode(2.f, FALSE);
			}
		}

		m_bChangeAnim = FALSE;
		if ((m_bChangeAnim == FALSE) && fRate >= m_pModelCom->Get_AnimationDuration(67) * 0.3f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_RUN_STOP);
		}
	}
	break;

	case FSM_JUMP:
		m_pModelCom->Set_ConvertSpeed(10.f);
		m_pModelCom->Set_Animation(47);
		m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.f);
	break;

	case FSM_WALK_RUN:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(64);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(64) * 0.78f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_RUN);
		}
	}
	break;

	case FSM_WALK_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(73);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(73)))
		{
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;

	case FSM_RUN_WALK:
	{
		m_pModelCom->Set_ConvertSpeed(10.f);
		m_pModelCom->Set_Animation(66);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(66) * 0.9f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_WALK);
		}
	}
	break;

	case FSM_RUN_STOP:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(65);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate <= (m_pModelCom->Get_AnimationDuration(65) * 0.1f))
		{
			_vector vPlayerLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);
			vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);

			_float4 PlayerLook;
			XMStoreFloat4(&PlayerLook, vPlayerLook);

			m_pParentController->Movement(fTimeDelta, PlayerLook, 2.0f);
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(65)))
		{
			m_pParentFSMCom->Set_IdleState();
			m_fPosition = {};
		}
	}
	break;

	case FSM_SPLINT_RUN:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(69);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(69) * 0.68f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_RUN);
		}
	}
	break;
#pragma endregion

#pragma region 공중
	case FSM_FALL_LOW:
	{
		m_pModelCom->Set_ConvertSpeed(20.f);
		m_pModelCom->Set_Animation(28);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.f);
		m_vRootTranslation.m128_f32[1] = 0.f;
		if (fRate >= (m_pModelCom->Get_AnimationDuration(28) * 0.5f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;

	case FSM_FLY_START:
	{
		m_pModelCom->Set_ConvertSpeed(60.f);
		m_pModelCom->Set_Animation(40);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(40) * 0.2f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_FLY_NORMAL);
		}
	}
	break;

	case FSM_FLY_NORMAL:
		m_pModelCom->Set_ConvertSpeed(2.f);
		m_pModelCom->Set_Animation(36);
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
		m_pModelCom->Set_Animation(42);
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
		m_pModelCom->Set_Animation(43);
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
		m_pModelCom->Set_Animation(26);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	break;

	case FSM_FALL_ATTACK_START1:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(93);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(93) * 0.9f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_START2);
		}
	}
	break;
	case FSM_FALL_ATTACK_START2:
	{
		m_pModelCom->Set_ConvertSpeed(15.f);
		m_pModelCom->Set_Animation(97);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(97) * 0.1f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK);
		}
	}
	break;

	case FSM_FALL_ATTACK:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(94);
		m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.2f);
	break;

	case FSM_FALL_ATTACK_END1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(95);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.5f, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(95) * 0.15f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_FALL_ATTACK_END2);
		}
	}
	break;

	case FSM_FALL_ATTACK_END2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(96);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(96) * 0.35f))
		{
			m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(96)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;
#pragma endregion

#pragma region 평타, 강공격
	case FSM_ATTACK_IDLE:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(106);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_ATTACK_READY: /* 강공, 평타 공통대기모션 */
	{
		//m_pModelCom->Set_ConvertSpeed(5.f);
		//m_pModelCom->Set_Animation(78);

		//_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta /** 0.5f*/, m_vRootTranslation);
		//if(fRate == 0.f)
		//	m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(78) * 0.1f);

		//if (fRate <= (m_pModelCom->Get_AnimationDuration(78)) && m_pGameInstance->Key_Pressing(VK_LBUTTON))
		//	m_bChargedReady = true;
		//else if(fRate <= (m_pModelCom->Get_AnimationDuration(78)) && !m_pGameInstance->Key_Pressing(VK_LBUTTON))
		//	m_bChargedReady = false;

		//if (fRate >= (m_pModelCom->Get_AnimationDuration(78)) * 0.9f)
		//{
		//	if (!m_bChargedReady)
		//	{
		//		m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
		//	}
		//	else
		//		m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED_READY);

		//	m_bUpdateRootTranslation = TRUE;
		//}

		m_pModelCom->Set_ConvertSpeed(2.f);
		if (ePrevState == FSM_IDLE)
		{
			m_pModelCom->Set_Animation(106);
			m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 1.05f);
		}
		else
		{
			m_pModelCom->Set_Animation(106);
			m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		}

		if (m_fExtraAttackTime == 0.f && m_fExtraAttackTime <= 0.3f && m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedReady = true;
		else if (m_fExtraAttackTime > 0.f && m_bChargedReady && m_fExtraAttackTime <= 0.3f && m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedReady = true;
		else if (m_fExtraAttackTime <= 0.5f && !m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedReady = false;

		m_fExtraAttackTime += fTimeDelta;

		if (!m_bChargedReady)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
			m_fExtraAttackTime = 0.f;
		}
		else if (m_bChargedReady && m_fExtraAttackTime >= 0.3f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED_READY);
			m_fExtraAttackTime = 0.f;
		}
	}
	break;

	case FSM_ATTACK_CHARGED_READY:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(85);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate <= (m_pModelCom->Get_AnimationDuration(85) * 0.5f) && m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedAttackStart = true;
		else if (fRate <= (m_pModelCom->Get_AnimationDuration(85) * 0.5f) && !m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedAttackStart = false;

		if (fRate >= (m_pModelCom->Get_AnimationDuration(85) * 0.5f))
		{
			if (!m_bChargedAttackStart)
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED2);
			else
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED1);
		}
	}
	break;

	case FSM_ATTACK1:
	{
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(77);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if(fRate == 0.f)
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(77) * 0.13f);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(77) * 0.1f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(77) * 0.16f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK2);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(77) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(79);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(79) * 0.1f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(79) * 0.26f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK3);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(79) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK3:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(80);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(80) * 0.05f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(80) * 0.26f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK4);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(80) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
			m_vPrevRootTranslation = {};
		}
	}
	break;

	case FSM_ATTACK4:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(81);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(81) * 0.1f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		/* Camera Shaking */
		if (m_bCameraControl && fRate >= m_pModelCom->Get_AnimationDuration(81) * 0.26f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				m_bCameraControl = FALSE;
			}
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			m_bAttack = TRUE;
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(81) * 0.42f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
			m_bCameraControl = TRUE;
			m_vPrevRootTranslation = {};
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(81) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bCameraControl = TRUE;
			m_vPrevRootTranslation = {};
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ATTACK_CHARGED1:
	{
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(82);
		_float fRate = {};
		if(!m_bChargedAttackEnd)
			fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.9f, m_vRootTranslation);
		else
			fRate = m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta * 0.9f, m_vRootTranslation);

		if (m_pGameInstance->Key_Pressing(VK_LBUTTON))
			m_bChargedAttackEnd = true;
		else
			m_bChargedAttackEnd = false;

		if (fRate == 0.f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
			{
				pCamera->Set_ActionMode(2.f);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(82) * 0.7f))
		{
			if (m_bChargedAttackEnd)
			{
				m_pModelCom->Reset_EventCaller();
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED_READY);
			}
		}
		if (fRate >= (m_pModelCom->Get_AnimationDuration(82) * 0.8f))
		{
			if (!m_bChargedAttackEnd)
			{
				m_pModelCom->Reset_EventCaller();
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED2);
			}
		}
	}
	break;

	case FSM_ATTACK_CHARGED2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(83);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate == 0.f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ActionMode(2.f);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(83) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_CHARGED_END);
		}
	}
	break;

	case FSM_ATTACK_END:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(102);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(102) * 0.8f))
		{
			m_pParentFSMCom->Set_IdleState();
		}
	}
	break;

	case FSM_ATTACK_CHARGED_END:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(84);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(84) * 0.8f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;
#pragma endregion

#pragma region 원소전투
	case FSM_ELEMENTAL_SKILL1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(86);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.1f, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(86) * 0.1f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down('E'))
		{
			m_bAttack = TRUE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(86) * 0.68f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL1_IDLE);
			m_bCameraControl = TRUE;
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL1_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(87);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.2f, m_vRootTranslation);

		if (m_bCameraControl)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				m_bCameraControl = FALSE;
			}
		}

		if (m_pGameInstance->Key_Down('E'))
		{
			m_bAttack = TRUE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(87) * 0.2f))
		{
			if (m_pGameInstance->Key_Down(VK_LBUTTON))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
				_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
				static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
			}
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(87) * 0.2f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL2);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(87) * 0.4f))
		{
			if (m_pGameInstance->Key_Pressing('W') == TRUE 
				|| m_pGameInstance->Key_Pressing('A') == TRUE
				|| m_pGameInstance->Key_Pressing('S') == TRUE
				|| m_pGameInstance->Key_Pressing('D') == TRUE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_WALK);
				_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
				static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
				m_bAttack = FALSE;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(87)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
			static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(88);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(88) * 0.1f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate > 0.f && m_pGameInstance->Key_Down('E'))
		{
			m_bAttack = TRUE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(88) * 0.78f))
		{
			m_pModelCom->Set_ConvertSpeed(20.f);
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL2_IDLE);
			m_bCameraControl = TRUE;
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL2_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(89);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.2f, m_vRootTranslation);

		if (m_bCameraControl)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				m_bCameraControl = FALSE;
			}
		}

		if (m_pGameInstance->Key_Down('E'))
		{
			m_bAttack = TRUE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(89) * 0.2f))
		{
			if (m_pGameInstance->Key_Down(VK_LBUTTON))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);

				_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
				static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
			}
		}

		if (m_bAttack && fRate >= (m_pModelCom->Get_AnimationDuration(89) * 0.2f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL3);
			m_bAttack = FALSE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(89) * 0.44f))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			if (m_pGameInstance->Key_Pressing('W') == TRUE
				|| m_pGameInstance->Key_Pressing('A') == TRUE
				|| m_pGameInstance->Key_Pressing('S') == TRUE
				|| m_pGameInstance->Key_Pressing('D') == TRUE)
			{
				_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
				static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
				m_pParentFSMCom->Change_ForceState(FSM_WALK);
				m_bAttack = FALSE;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(89)))
		{
			_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
			static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
			m_bAttack = FALSE;
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL3:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(90);
		m_vPrevRootTranslation = m_vRootTranslation;
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.2f, m_vRootTranslation);

		if (m_bReset_SkillTime == FALSE)
		{
			_float fMaxTime = static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Get_ElementalSkillTime();
			static_cast<CPart_Status*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_STATUS, iIndex))->Set_CurSkillTime(fMaxTime);
			m_bReset_SkillTime = TRUE;
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(90) * 0.1f))
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(90) * 0.78f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_SKILL3_IDLE);
			m_bCameraControl = TRUE;
			m_bReset_SkillTime = FALSE;
		}
	}
	break;

	case FSM_ELEMENTAL_SKILL3_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(91);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.2f, m_vRootTranslation);

		if (m_bCameraControl && fRate > 0.f)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				m_bCameraControl = FALSE;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(91) * 0.2f))
		{
			if (m_pGameInstance->Key_Down(VK_LBUTTON))
			{
				m_pParentFSMCom->Change_ForceState(FSM_ATTACK1);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(91) * 0.4f))
		{
			if (m_pGameInstance->Key_Pressing('W') == TRUE
				|| m_pGameInstance->Key_Pressing('A') == TRUE
				|| m_pGameInstance->Key_Pressing('S') == TRUE
				|| m_pGameInstance->Key_Pressing('D') == TRUE)
			{
				m_pParentFSMCom->Change_ForceState(FSM_WALK);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(91)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;

	case FSM_ELEMENTAL_BURST:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(92);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_iCameraScene == 0 && fRate >= (m_pModelCom->Get_AnimationDuration(92) * 0.04f) && fRate < (m_pModelCom->Get_AnimationDuration(92) * 0.3f))
		{
			// 궁 컷씬
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DILUC, SCENE_ONE, 1.f, 0.2f);
				m_iCameraScene += 1;
			}
		}
		else if (m_iCameraScene == 1 && fRate >= (m_pModelCom->Get_AnimationDuration(92) * 0.3f) && fRate < (m_pModelCom->Get_AnimationDuration(92) * 0.64f))
		{
			// 궁 컷씬
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DILUC, SCENE_TWO, -1.f, 2.f);
				m_iCameraScene += 1;
			}
		}
		else if (m_iCameraScene == 2 && fRate >= (m_pModelCom->Get_AnimationDuration(92) * 0.64f))
		{
			CSkillObj::SKILLOBJ_DESC tDesc = {};
			tDesc.fDamage = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_BurstDamage();
			tDesc.vDir = XMVector3Normalize(XMVectorSet(m_CombinedWorldMatrix._31, 0.f, m_CombinedWorldMatrix._33, 0.f));
			tDesc.vSpawnPos = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f,1.5f,0.f,0.f)
				+ XMVector3Normalize(m_pParentTransformCom->Get_State(CTransform::STATE_LOOK)) * 1.5f;
			//XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, m_CombinedWorldMatrix._44);
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Phoenix", L"Layer_Skill", &tDesc)))
				return E_FAIL;

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_DILUC, SCENE_THREE, 1.f, 1.f);
				m_iCameraScene += 1;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(92) * 0.95f))
		{
			/*CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CREATURE_END, SCENE_END, 0.f, 0.f, false);
			}*/

			m_pModelCom->Set_ConvertSpeed(1.5f);
			m_pParentFSMCom->Change_ForceState(FSM_ELEMENTAL_BURST_IDLE);
			
			m_iCameraScene = 0;
		}
	}
	break;

	case FSM_ELEMENTAL_BURST_IDLE:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(106);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(106) * 0.12f))
		{
			if (m_pGameInstance->Key_Pressing('W') == TRUE
				|| m_pGameInstance->Key_Pressing('A') == TRUE
				|| m_pGameInstance->Key_Pressing('S') == TRUE
				|| m_pGameInstance->Key_Pressing('D') == TRUE)
			{
				m_pModelCom->Set_ConvertSpeed(5.f);
				m_pParentFSMCom->Change_ForceState(FSM_WALK);
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(106) * 0.12f))
		{
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

#pragma region 피격
	case FSM_HIT_L:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(101);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(101) * 0.9f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;

	case FSM_HIT_H:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(100);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(100) * 0.f && fRate < m_pModelCom->Get_AnimationDuration(100) * 0.1f)
		{
			m_pModelCom->Play_PlayerRootAnimation_NoHeight(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 0.8f);
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(100) * 0.9f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}
	break;

	case FSM_HIT_THROW:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(45);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(45) || m_pParentController->Get_InAir() == FALSE)
		{
			m_pParentFSMCom->Change_ForceState(FSM_HIT_THROW_FLY);
		}
	}
	break;

	case FSM_HIT_THROW_FLY:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_ConvertSpeed(50.f);
		m_pModelCom->Set_Animation(46);
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
		m_pModelCom->Set_Animation(44);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate == 0.f)
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(44) * 0.07f);

		if (fRate >= m_pModelCom->Get_AnimationDuration(44))
		{
			m_pModelCom->Set_ConvertSpeed(5.f);
			m_pParentFSMCom->Change_ForceState(FSM_ATTACK_IDLE);
		}
	}

	break;

#pragma endregion

#pragma region 죽음
	case FSM_DEAD1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(23);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(23))
		{
			if (m_fDissolve == 0.f)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Diluc_Fallen_0" + to_wstring(iRandomVoice) + L".ogg";
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
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(24);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(24))
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
		m_pModelCom->Set_Animation(98);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.4f, m_vRootTranslation);

		if (fRate == 0.f)
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(98) * 0.4f);

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
			{
				pCamera->Set_SceneMode(CAMSCENEID_CHANGE, SCENE_ONE, 1.5f, 0.3f);
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(98) * 0.8f)
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

HRESULT CBody_Diluc::Add_Component(void* pArg)
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

HRESULT CBody_Diluc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Diluc::Initialize_Clone(void* pArg)
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

HRESULT CBody_Diluc::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Diluc::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Diluc::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Diluc::Render()
{
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

		if (m_bFreezing == TRUE) // 빙결
		{
			if (FAILED(m_pFreezingCom->Bind_ShaderResource(m_pShaderCom, "g_FreezingDiffuseTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pFreezingCom->Bind_ShaderResource(m_pShaderCom, "g_FreezingNoiseTexture", 1)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_FREEZING)))
				return E_FAIL;
		}
		else if (m_bPetrification == TRUE) // 석화
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

void CBody_Diluc::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_PLAYER_BODY, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Diluc Body COLL_PLAYER_BODY" << endl;
	else
		cout << "Release Diluc Body COLL_PLAYER_BODY" << endl;

	hr = m_pGameInstance->Release_Collider_Object(COLL_EVENTREACT_PLAYER, m_iColliderID + 1);

	if (FAILED(hr))
		cout << "Failed Diluc Body COLL_EVENTREACT_PLAYER" << endl;
	else
		cout << "Release Diluc Body COLL_EVENTREACT_PLAYER" << endl;
}

CBody_Diluc* CBody_Diluc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Diluc* pInstance = new CBody_Diluc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Diluc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Diluc::Clone(void* pArg)
{
	CBody_Diluc* pInstance = new CBody_Diluc(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Diluc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Diluc::Free()
{
	__super::Free();
}
