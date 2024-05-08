#include "stdafx.h"
#include "Andrius.h"

#include "Part_Body.h"
#include "Part_Status.h"
#include "Camera_Main.h"

#include "GameInstance.h"
#include "Hp_Bar.h"
#include "Skill_IceCone.h"

#include "Body_Andrius.h"

#include "Effect.h"
#include "Skill_Claw.h"
#include "Eff_Impact_Glow.h"

#include "BossChest.h"

CAndrius::CAndrius(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CAndrius::CAndrius(const CAndrius& rhs)
	: CCreature(rhs)
{
}

HRESULT CAndrius::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = MONSTER_ANDRIUS;
	tBodyDesc.eType = CTY_MONSTER;
	tBodyDesc.pController = m_pController;

	CREATUREID eCreatureID = MONSTER_ANDRIUS;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	//pPartObject->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot[0] = pPartObject;
	m_Parts.push_back(tSlots);

	tSlots = { };

	/* For. Part_Status */
	CPart_Status::PARTSTATUS_DESC tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	pPartObject = CPart_Status::Create_Status(MONSTER_ANDRIUS, tStatusDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[0] = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

/// <summary>
/// Andrius의 패턴을 제어하는 함수
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CAndrius::Pattern(_float fTimeDelta)
{
	if (m_fHp <= 0 && !m_bIsDying)
	{
		m_bIsDying = TRUE;
		m_bIsRushStarted = FALSE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_DEAD_HOWL_START);

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_THREE, 1.f, 1.f);
	}

	// 현재 내 상태 가져오기
	FSM_ANDRIUS_STATE m_eCurrentState = static_cast<FSM_ANDRIUS_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	// 플레이어 정보 구해놓기
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));

	// 내 정보들 구해놓기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

	// 플레이어를 향하는 방향벡터
	_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	// 치트키
	if (m_pGameInstance->Key_Down('9'))
	{
		m_fHp -= 10000.f;
		m_pHpBar->Set_CurrentHp(m_fHp);
	}
	else if (m_pGameInstance->Key_Down(VK_DELETE))
	{
		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_ICECONE;
		desc.fLifeTime = 2.f;
		desc.vSpawnPos = vPlayerPos;

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_IceCone", L"Layer_Skill", &vPlayerPos)))
			return E_FAIL;
	}


	//if (m_fHp <= m_fMaxHp / 2.f)
	//{
	//	cout << "반피 이하!!!!!!!!!!" << endl;
	//}
	//else
	//{
	//	cout << "현재 체력 :" << m_fHp << ",   " << "최대 체력 : " << m_fMaxHp << endl;
	//}

	// 현재 내 상태에 따라서 패턴 처리
	switch (m_eCurrentState)
	{
	case Client::FSM_ANDRIUS_THINK:
		m_bEffectExecuted = FALSE;		// 이펙트 관리 bool 다시 설정

		// 무한 스위치요미
		DecideNextState();
		break;
	case Client::FSM_ANDRIUS_IDLE:
	{
		m_bIsAlreadyWindBladeShot = FALSE;	// 바람 칼날 변수 꺼주기

		if (m_bIsPhase2 == FALSE)		// 1페이즈라면
		{
			m_fPatternTimer += fTimeDelta;
			if (m_fPatternTimer>= 2.f)
			{
				m_fPatternTimer = 0.f;
				
				if (VerifyIsPlayerNearby() == TRUE)
					Random_Near_Attack_Phase1();
				else
					Random_Far_Attack_Phase1();
			}
		}
		else if (m_bIsPhase2 == TRUE)	// 2페이즈라면
		{
			m_fPatternTimer += fTimeDelta;
			if (m_fPatternTimer >= 2.f)		// 2초가 지났다면
			{
				m_fPatternTimer = 0.f;

				if (VerifyIsPlayerInFront() == FALSE) // 뒤쪽에 있다면
				{
					if (VerifyIsPlayerOnLeft() == TRUE)	// 왼쪽에 있다면
						m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_FAST_LEFT);
					else	// 오른쪽에 있다면
						m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_FAST_RIGHT);
				}
				else	// 앞쪽에 있다면
				{
					if (VerifyIsPlayerNearby() == TRUE)		// 가까이에 있다면
						Random_Near_Attack_Phase2();
					else							// 멀리 있다면
						Random_Far_Attack_Phase2();
				}
			}
		}
	}
		break;
	case Client::FSM_ANDRIUS_WALK_FIRST:	// 첫등장때 앞으로 걷기
	{
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 2.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);

		// 5초가 지나면 다음 패턴
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 8.f)
		{
			m_fPatternTimer = 0.0f;

			if (m_fHp <= static_cast<_int>(m_fMaxHp / 2.f))
				m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_TURN_RUSH_LEFT);
			else if (VerifyIsPlayerOnLeft() == TRUE)
				m_pFiniteStateMachineCom->Change_State(FSM_ANDRIUS_WALK_LEFT);
			else
				m_pFiniteStateMachineCom->Change_State(FSM_ANDRIUS_WALK_RIGHT);

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			pCamera->Set_SceneMode(CAMSCENEID_ANDRIUS, SCENE_ONE, 10.f, 0.2f, TRUE);
		}
	}
	break;
	case Client::FSM_ANDRIUS_WALK_LEFT:		// 플레이어 기준 왼쪽으로 걷기
	{
		m_bEffectExecuted = FALSE;

		_vector qRotationLeft = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(80.0f));
		_vector vWalkDirectionLeft = XMVector3Rotate(vDirectionToPlayer, qRotationLeft);
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vWalkDirectionLeft, 11.f);
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 2.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);

		if (VerifyIsPlayerOnLeft() == FALSE)
			m_pFiniteStateMachineCom->Change_State(FSM_ANDRIUS_WALK_RIGHT);

		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 5.f)
		{
			m_fPatternTimer = 0.0f;

			if (m_fHp <= static_cast<_int>(m_fMaxHp / 2.f))
				m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_TURN_RUSH_LEFT);
			else if (VerifyIsPlayerNearby() == TRUE)
				Random_Near_Attack_Phase1();
			else
				Random_Far_Attack_Phase1();
		}
	}
	break;
	case Client::FSM_ANDRIUS_WALK_RIGHT:	// 플레이어 기준 오른쪽으로 걷기
	{
		m_bEffectExecuted = FALSE;

		_vector qRotationRight = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(80.0f));
		_vector vWalkDirectionRight = XMVector3Rotate(vDirectionToPlayer, qRotationRight);
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vWalkDirectionRight, 11.f);
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 2.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);

		if (VerifyIsPlayerOnLeft() == TRUE)
			m_pFiniteStateMachineCom->Change_State(FSM_ANDRIUS_WALK_LEFT);

		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 5.f)
		{
			m_fPatternTimer = 0.0f;

			if (m_fHp <= static_cast<_int>(m_fMaxHp / 2.f))
				m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_TURN_RUSH_RIGHT);
			else if (VerifyIsPlayerNearby() == TRUE)
				Random_Near_Attack_Phase1();
			else
				Random_Far_Attack_Phase1();
		}
	}
	break;
	case Client::FSM_ANDRIUS_SCRATCH_LEFT:
	{
		//if (m_bEffectExecuted == FALSE)
		//{
		//	CSkillObj::SKILLOBJ_DESC desc = {};
		//	desc.eSkillObjID = SKILLOBJ_SCRATCH_LEFT;
		//	desc.fLifeTime = 5.2f;
		//	desc.vSpawnPos = vSelfPos;
		//	desc.vScale = { 10.1f, 10.1f, 10.1f, 0.f };

		//	// 스킬 범위 바람 생성
		//	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Scratch", L"Layer_Skill", &desc)))
		//		return E_FAIL;

		//	m_bEffectExecuted = TRUE;
		//}
	}
	break;
	case Client::FSM_ANDRIUS_SCRATCH_RIGHT:
	{
		//if (m_bEffectExecuted == FALSE)
		//{
		//	CSkillObj::SKILLOBJ_DESC desc = {};
		//	desc.eSkillObjID = SKILLOBJ_SCRATCH_RIGHT;
		//	desc.fLifeTime = 2.2f;
		//	desc.vSpawnPos = vSelfPos;
		//	desc.vScale = { 10.1f, 10.1f, 10.1f, 0.f };

		//	// 스킬 범위 바람 생성
		//	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Scratch", L"Layer_Skill", &desc)))
		//		return E_FAIL;

		//	m_bEffectExecuted = TRUE;
		//}
	}
	break;
	case Client::FSM_ANDRIUS_SWEEP_LEFT:
	{
		//if (m_bEffectExecuted == FALSE)
		//{
		//	// 바람 스킬 범위 이펙트
		//	CSkillObj::SKILLOBJ_DESC desc = {};
		//	desc.eSkillObjID = SKILLOBJ_WINDRANGE_SWEEP;
		//	desc.fLifeTime = 2.2f;
		//	desc.vSpawnPos = vSelfPos;
		//	desc.vScale = { 0.1f, 0.1f, 0.1f, 0.f };

		//	// 스킬 범위 바람 생성
		//	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
		//		return E_FAIL;

		//	m_bEffectExecuted = TRUE;
		//}
	}
		break;
	case Client::FSM_ANDRIUS_SWEEP_RIGHT:
	{
		//if (m_bEffectExecuted == FALSE)
		//{
		//	// 바람 스킬 범위 이펙트
		//	CSkillObj::SKILLOBJ_DESC desc = {};
		//	desc.eSkillObjID = SKILLOBJ_WINDRANGE_SWEEP;
		//	desc.fLifeTime = 2.2f;
		//	desc.vSpawnPos = vSelfPos;
		//	desc.vScale = { 0.1f, 0.1f, 0.1f, 0.f };

		//	// 스킬 범위 바람 생성
		//	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
		//		return E_FAIL;

		//	m_bEffectExecuted = TRUE;
		//}
	}
		break;
	case Client::FSM_ANDRIUS_JUMP:
	{
		m_vLastPlayerPos.m128_f32[1] = 0.f;

		_vector vDirectionToPlayer = XMVector3Normalize(m_vLastPlayerPos - vSelfPos);
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer);

		_vector vDistance = m_vLastPlayerPos - vSelfPos;	
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		_float fSpeedPerSec = fDistance / 0.3f;

		_vector vDirection = XMVector3Normalize(vDistance);

		_float fMoveDistance = fSpeedPerSec * fTimeDelta;

		vSelfPos += vDirection * fMoveDistance;

		_float4 fSelfPos = {};
		XMStoreFloat4(&fSelfPos, vSelfPos);
		m_pController->Set_Position(fSelfPos);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSelfPos);

		// 목표지점에 도달했는지 확인
		if (XMVectorGetX(XMVector3Length(vSelfPos - m_vLastPlayerPos)) < 0.001f)
			vSelfPos = m_vLastPlayerPos;
	}
	break;
	case Client::FSM_ANDRIUS_BACKJUMP:
	{
	}
		break;
	case Client::FSM_ANDRIUS_RUSH:
	{
		// 동그라미 이펙트
		m_fCircleTimer += fTimeDelta;

		if (m_fCircleTimer >= 0.3f)			// n 초 마다 돌진 동그라미 이펙트
		{
			m_fCircleTimer = 0.f;

			CEffect::EFFECT_DESC desc = {};
			desc.eEffectID = EFFECT_HITTING_STAR;
			desc.fLifeTime = 1.f;
			desc.vDir = vSelfLook;
			vSelfPos.m128_f32[1] += 1.f;
			vSelfPos += vSelfLook * 2.f;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 5.f, 5.f, 0.f, 0.f };
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Rush_Circle", L"Layer_Effect", &desc)))
				return E_FAIL;
		}

		// 돌진이 시작되지 않았다면 시작 위치를 설정
		if (m_bIsRushStarted == FALSE)
		{
			m_vRushStartPos = vSelfPos;
			m_bIsRushStarted = TRUE; // 돌진 시작 플래그를 설정

			// 돌진 바람가르는 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_RUSH;
			vSelfPos += vSelfLook * 3.f;
			vSelfPos += vSelfUp * 2.f;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 0.01f, 0.01f, 0.01f, 0.f };
			desc.vDir = vSelfLook;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Rush", L"Layer_Skill", &desc)))
				return E_FAIL;

			desc.vScale = { 0.245f, 0.245f, 0.245f, 0.f };
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Rush", L"Layer_Skill", &desc)))
				return E_FAIL;
		}
		
		m_fPatternTimer += fTimeDelta;
		if(m_fPatternTimer <= 0.5f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 20.f);
		}

		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 25.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 5.f);

		_vector vDistance = vSelfPos - m_vRushStartPos;
		_float fRushDistance = XMVectorGetX(XMVector3Length(vDistance));

		_vector vMaxDistance = vSelfPos - m_vFieldCenterPos;
		_float fMaxRushDistance = XMVectorGetX(XMVector3Length(vMaxDistance));

		// 벽을 향해 처박으러 가고 있다면
		if (VerifyIsImminentWallCollisionForAndrius() == TRUE)
		{
			if (VerifyIsPlayerOnLeft())
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRAM_LEFT);
			else
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRAM_RIGHT);

			m_vScramStartPos = vSelfPos;
			m_bIsRushStarted = FALSE;
			m_fPatternTimer = 0.f;
		}
		else
		{
			// 돌진거리가 40이상이면 멈춰!
			if (fRushDistance > 40.f)
			{
				if (VerifyIsPlayerOnLeft())
					m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRAM_LEFT);
				else
					m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRAM_RIGHT);

				m_vScramStartPos = vSelfPos;
				m_bIsRushStarted = FALSE;
				m_fPatternTimer = 0.f;
			}
		}
	}
	break;
	case Client::FSM_ANDRIUS_RUSH_READY:
		break;
	case Client::FSM_ANDRIUS_TURN_RUSH_LEFT:
	{
		if (!m_bIsTurnRushStarted)
		{
			CAssetFX::ASFX_DESC tAsfxDesc = { };
			m_pGameInstance->Play_AssetFX(L"AndriusMeteorite", L"AndriusMeteorite", &tAsfxDesc);
			m_pGameInstance->Play_AssetFX(L"AndriusSavageTornado", L"AndriusSavageTornado", &tAsfxDesc);

			m_bIsTurnRushStarted = TRUE;

			// 시작할 때 최소 반지름 길이
			m_fMinTurnRushCircleRadius = XMVectorGetX(XMVector3Length(vSelfPos - m_vFieldCenterPos));

			// 반지름 배율을 최소 반지름 길이로 설정
			m_fTurnRushCircleRadiusScale = m_fMinTurnRushCircleRadius;

			// 회전값 초기화
			m_fRotVal = 3.f;  // fTurnRushCircleRadiusScale이 fMinTurnRushCircleRadius에 가까울 때의 초기 회전값

			// 중점에서 보스를 향하는 방향 벡터
			m_vDirToBossFromCenter_Nor = XMVector3Normalize(vSelfPos - m_vFieldCenterPos);
		}

		// 반지름 스케일을 시간에 따라 증가
		m_fTurnRushCircleRadiusScale += fTimeDelta * 5.f;

		// fTurnRushCircleRadiusScale이 증가함에 따라 m_fRotVal을 감소
		_float fMaxTurnRushCircleRadiusScale = 40.f;
		m_fRotVal = ((fMaxTurnRushCircleRadiusScale - m_fTurnRushCircleRadiusScale) / (fMaxTurnRushCircleRadiusScale - m_fMinTurnRushCircleRadius)) * (2.f - 0.7f) + 0.4f;

		// 반지름 스케일의 최대값을 제한
		if (m_fTurnRushCircleRadiusScale > fMaxTurnRushCircleRadiusScale)
		{
			m_fTurnRushCircleRadiusScale = fMaxTurnRushCircleRadiusScale;
			m_fRotVal = 0.7f;  // 최대 반지름에서의 회전값
		}

		// 회전용 쿼터니언 계산
		_vector qRotation = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(m_fRotVal));

		// 중점에서 보스를 향하는 방향벡터를 회전
		m_vDirToBossFromCenter_Nor = XMVector3Rotate(m_vDirToBossFromCenter_Nor, qRotation);
		m_vDirCenterToTarget = m_vDirToBossFromCenter_Nor * m_fTurnRushCircleRadiusScale;

		// 중심점에서의 목표지점까지의 위치를 계산
		_vector vCenterToTargetPos = m_vFieldCenterPos + m_vDirCenterToTarget;

		// vSelfPos에서 vCenterToTargetPos까지의 방향 벡터
		_vector vDirBossToTarget = XMVector3Normalize(vCenterToTargetPos - vSelfPos);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirBossToTarget, 15.f);
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 20.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 5.f);


		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 8.f)
		{
			m_fPatternTimer = 0.0f;
			if (m_iTurnRushPatternIndex == 0 || m_iTurnRushPatternIndex == 1)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_START);
				m_iTurnRushPatternIndex++;
				m_vScramStartPos = vSelfPos;
				m_bIsTurnRushStarted = FALSE;
			}
			else if (m_iTurnRushPatternIndex == 2)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_JUMP);
				m_iTurnRushPatternIndex = 0;
				m_vScramStartPos = vSelfPos;
				m_bIsTurnRushStarted = FALSE;
			}
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_RIGHT:
	{
		if (!m_bIsTurnRushStarted)
		{
			CAssetFX::ASFX_DESC tAsfxDesc = { };
			m_pGameInstance->Play_AssetFX(L"AndriusMeteorite", L"AndriusMeteorite", &tAsfxDesc);
			m_pGameInstance->Play_AssetFX(L"AndriusSavageTornado", L"AndriusSavageTornado", &tAsfxDesc);

			m_bIsTurnRushStarted = TRUE;

			// 시작할 때 최소 반지름 길이
			m_fMinTurnRushCircleRadius = XMVectorGetX(XMVector3Length(vSelfPos - m_vFieldCenterPos));

			// 반지름 배율을 최소 반지름 길이로 설정
			m_fTurnRushCircleRadiusScale = m_fMinTurnRushCircleRadius;

			// 회전값 초기화
			m_fRotVal = 3.f;  // fTurnRushCircleRadiusScale이 fMinTurnRushCircleRadius에 가까울 때의 초기 회전값

			// 중점에서 보스를 향하는 방향 벡터
			m_vDirToBossFromCenter_Nor = XMVector3Normalize(vSelfPos - m_vFieldCenterPos);
		}

		// 반지름 스케일을 시간에 따라 증가
		m_fTurnRushCircleRadiusScale += fTimeDelta * 5.f;

		// fTurnRushCircleRadiusScale이 증가함에 따라 m_fRotVal을 감소
		_float fMaxTurnRushCircleRadiusScale = 40.f;
		m_fRotVal = ((fMaxTurnRushCircleRadiusScale - m_fTurnRushCircleRadiusScale) / (fMaxTurnRushCircleRadiusScale - m_fMinTurnRushCircleRadius)) * (2.f - 0.7f) + 0.4f;

		// 반지름 스케일의 최대값을 제한
		if (m_fTurnRushCircleRadiusScale > fMaxTurnRushCircleRadiusScale)
		{
			m_fTurnRushCircleRadiusScale = fMaxTurnRushCircleRadiusScale;
			m_fRotVal = 0.7f;  // 최대 반지름에서의 회전값
		}

		// 회전용 쿼터니언 계산
		//_vector qRotation = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.7f));
		_vector qRotation = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotVal));

		// 중점에서 보스를 향하는 방향벡터를 회전
		m_vDirToBossFromCenter_Nor = XMVector3Rotate(m_vDirToBossFromCenter_Nor, qRotation);
		m_vDirCenterToTarget = m_vDirToBossFromCenter_Nor * m_fTurnRushCircleRadiusScale;

		// 중심점에서의 목표지점까지의 위치를 계산 (중심점에서 방향 벡터에 반지름 길이를 곱한 값).
		_vector vCenterToTargetPos = m_vFieldCenterPos + m_vDirCenterToTarget;

		// vSelfPos에서 vCenterToTargetPos 까지의 방향 벡터
		_vector vDirBossToTarget = XMVector3Normalize(vCenterToTargetPos - vSelfPos);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirBossToTarget, 15.f);
		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 20.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 5.f);

		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 8.f)
		{
			m_fPatternTimer = 0.0f;
			if (m_iTurnRushPatternIndex == 0 || m_iTurnRushPatternIndex == 1)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_START);
				m_iTurnRushPatternIndex++;
				m_vScramStartPos = vSelfPos;
				m_bIsTurnRushStarted = FALSE;
			}
			else if (m_iTurnRushPatternIndex == 2)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_JUMP);
				m_iTurnRushPatternIndex = 0;
				m_vScramStartPos = vSelfPos;
				m_bIsTurnRushStarted = FALSE;
			}
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH:
	{
		m_fCircleTimer += fTimeDelta;

		if (m_fCircleTimer >= 0.3f)			// n 초 마다 돌진 동그라미 이펙트
		{
			m_fCircleTimer = 0.f;

			CEffect::EFFECT_DESC desc = {};
			desc.eEffectID = EFFECT_HITTING_STAR;
			desc.fLifeTime = 1.f;
			desc.vDir = vSelfLook;
			vSelfPos.m128_f32[1] += 1.f;
			vSelfPos += vSelfLook * 2.f;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 5.f, 5.f, 0.f, 0.f };
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Rush_Circle", L"Layer_Effect", &desc)))
				return E_FAIL;
		}

		// 돌진이 시작되지 않았다면 시작 위치를 설정
		if (!m_bIsRushStarted)
		{
			m_vRushStartPos = vSelfPos;
			m_bIsRushStarted = TRUE; // 돌진 시작 플래그를 설정

			// 돌진 바람가르는 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_RUSH;
			vSelfPos += vSelfLook * 3.f;
			vSelfPos += vSelfUp * 2.f;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 0.01f, 0.01f, 0.01f, 0.f };
			desc.vDir = vSelfLook;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Rush", L"Layer_Skill", &desc)))
				return E_FAIL;

			desc.vScale = { 0.245f, 0.245f, 0.245f, 0.f };
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Rush", L"Layer_Skill", &desc)))
				return E_FAIL;
		}

		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer <= 0.5f)
		{
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 20.f);
		}

		_float4 fSelfLook = {};
		XMStoreFloat4(&fSelfLook, vSelfLook);
		m_pController->Movement(fTimeDelta, fSelfLook, 25.f);
		//m_pTransformCom->Go_Straight(fTimeDelta * 5.f);

		_vector vDistance = vSelfPos - m_vRushStartPos;
		_float fRushDistance = XMVectorGetX(XMVector3Length(vDistance));

		// 돌진거리가 30이상이면 멈춰!
		if (55.f < fRushDistance)
		{
			if (VerifyIsPlayerOnLeft())
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_ARRIVE);
			else
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_ARRIVE);

			m_vScramStartPos = vSelfPos;
			m_bIsRushStarted = FALSE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP:
	{
		m_pGameInstance->Stop_AssetFX(L"AndriusMeteorite");
		m_pGameInstance->Stop_AssetFX(L"AndriusSavageTornado");

		_vector vDirectionToTarget = XMVector3Normalize(m_vFieldCenterPos - vSelfPos);
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToTarget);

		_vector vDistance = m_vFieldCenterPos - vSelfPos;
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		_float fSpeedPerSec = fDistance / 0.3f;

		_vector vDirection = XMVector3Normalize(vDistance);

		_float fMoveDistance = fSpeedPerSec * fTimeDelta;

		vSelfPos += vDirection * fMoveDistance;

		_float4 fSelfPos = {};
		XMStoreFloat4(&fSelfPos, vSelfPos);
		m_pController->Set_Position(fSelfPos);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSelfPos);

		// 목표지점에 도달했는지 확인
		if (XMVectorGetX(XMVector3Length(vSelfPos - m_vFieldCenterPos)) < 0.001f)
		{
			vSelfPos = m_vFieldCenterPos;
		}
	}
	break;
	case Client::FSM_ANDRIUS_ICICLESHOOT_RIGHT:
		break;
	case Client::FSM_ANDRIUS_ICICLESHOOT_LEFT:
		break;
	case Client::FSM_ANDRIUS_JUMP_READY_LEFT:
	{
		if (m_bEffectExecuted == FALSE)
		{
			m_vLastPlayerPos = vPlayerPos;
			m_vLastPlayerPos.m128_f32[1] = 0.f;

			// 바람 스킬 범위 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_WINDRANGE_JUMP;
			desc.fLifeTime = 2.2f;
			desc.vSpawnPos = m_vLastPlayerPos;
			desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };

			// 스킬 범위 바람 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
				return E_FAIL;

			m_pGameInstance->PlaySoundW(L"Andrius_EffJump.mp3", 0.5f);

			m_bEffectExecuted = TRUE;
		}
	}
		break;
	case Client::FSM_ANDRIUS_JUMP_READY_RIGHT:
	{
		if (m_bEffectExecuted == FALSE)
		{
			m_vLastPlayerPos = vPlayerPos;
			m_vLastPlayerPos.m128_f32[1] = 0.f;

			// 바람 스킬 범위 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_WINDRANGE_JUMP;
			desc.fLifeTime = 2.2f;
			desc.vSpawnPos = m_vLastPlayerPos;
			desc.vScale = { 0.1f, 0.1f, 0.1f, 0.f };

			// 스킬 범위 바람 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
				return E_FAIL;

			m_bEffectExecuted = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_JUMP_READY_FRONT:
		break;
	case Client::FSM_ANDRIUS_SCRAM_LEFT:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 7.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 4.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 1.f);
		}
	}
	break;
	case Client::FSM_ANDRIUS_SCRAM_RIGHT:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 7.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 4.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 1.f);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_START:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 15.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 16.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 4.f);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_ARRIVE:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 15.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 16.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 4.f);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_START:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 15.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 16.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 4.f);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_ARRIVE:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 15.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 16.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 4.f);
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_LEFT_JUMP:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 15.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 16.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 4.f);
		}
		else
		{
			if (m_bEffectExecuted == FALSE)
			{
				// 바람 스킬 범위 이펙트
				CSkillObj::SKILLOBJ_DESC desc = {};
				desc.eSkillObjID = SKILLOBJ_WINDRANGE_JUMP_CENTER;
				desc.fLifeTime = 3.5f;
				desc.vSpawnPos = m_vFieldCenterPos;
				desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };

				// 스킬 범위 바람 생성
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
					return E_FAIL;

				m_pGameInstance->PlaySoundW(L"Andrius_EffJump.mp3", 0.5f);

				m_bEffectExecuted = TRUE;
			}
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_RUSH_SCRAM_RIGHT_JUMP:
	{
		_vector vDistance = vSelfPos - m_vScramStartPos;
		_float fScramDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (fScramDistance < 15.f)
		{
			_float4 fSelfLook = {};
			XMStoreFloat4(&fSelfLook, vSelfLook);
			m_pController->Movement(fTimeDelta, fSelfLook, 16.f);
			//m_pTransformCom->Go_Straight(fTimeDelta * 4.f);
		}
		else
		{
			if (m_bEffectExecuted == FALSE)
			{
				// 바람 스킬 범위 이펙트
				CSkillObj::SKILLOBJ_DESC desc = {};
				desc.eSkillObjID = SKILLOBJ_WINDRANGE_JUMP_CENTER;
				desc.fLifeTime = 3.5f;
				desc.vSpawnPos = m_vFieldCenterPos;
				desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };

				// 스킬 범위 바람 생성
				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
					return E_FAIL;

				m_pGameInstance->PlaySoundW(L"Andrius_EffJump.mp3", 0.5f);

				m_bEffectExecuted = TRUE;
			}
		}
	}
	break;
	case Client::FSM_ANDRIUS_TURN_FAST_LEFT:
		break;
	case Client::FSM_ANDRIUS_TURN_FAST_RIGHT:
		break;
	case Client::FSM_ANDRIUS_TURN180_LEFT:
		break;
	case Client::FSM_ANDRIUS_TURN180_RIGHT:
		break;
		// Phase 2 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	case Client::FSM_ANDRIUS_PHAZE2_ENTER_HOWL_START:
	{
		if (m_bEffectExecuted == FALSE)
		{
			// 바람 스킬 범위 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_WINDRANGE_HOWL;
			desc.fLifeTime = 4.5f;
			
			vSelfPos += vSelfLook;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };

			// 스킬 범위 바람 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
				return E_FAIL;

			m_bEffectExecuted = TRUE;
		}
	}
		break;
	case Client::FSM_ANDRIUS_PHAZE2_ENTER_HOWLING:
	{
		if(m_fPatternTimer == 0.f)
			m_pGameInstance->PlayBGM(L"BGM_Andrius_Phase2.mp3", 0.5f);

		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 1.f)
		{
			m_fPatternTimer = 0.f;
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_PHAZE2_ENTER_HOWL_END);
		}
	}
	break;
	case Client::FSM_ANDRIUS_PHAZE2_ENTER_HOWL_END:
	{
		CAssetFX::ASFX_DESC tAsfxDesc = { };
		m_pGameInstance->Play_AssetFX(L"AndriusMeteorite", L"AndriusMeteorite", &tAsfxDesc);
	}
		m_bEffectExecuted = FALSE;
		m_bIsPhase2 = TRUE;
		break;
	case Client::FSM_ANDRIUS_ICE_SHOCK_WAVE_READY:
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_ANDRIUS_ICE_SHOCK_WAVE:
	{
		m_bIsIceShockWaving = TRUE;
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SHOCK_WAVE_READY:
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vDirectionToPlayer, 15.f);
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SHOCK_WAVE:
	{
		Pattern_WindBlade(fTimeDelta);
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SCRATCH_LEFT:
	{
	}
	break;
	case Client::FSM_ANDRIUS_WIND_SCRATCH_RIGHT:
	{
	}
	break;
	case Client::FSM_ANDRIUS_ROAR_START:
		break;
	case Client::FSM_ANDRIUS_ROARING:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 10.f)
		{
			m_fPatternTimer = 0.f;
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_ROAR_END);
		}

		// 주변에 광역 바람 데미지
	}
	break;
	case Client::FSM_ANDRIUS_ROAR_END:
		break;
	case Client::FSM_ANDRIUS_IDLE_VARIATION_1:
		break;
	case Client::FSM_ANDRIUS_IDLE_VARIATION_2:
		break;
	case Client::FSM_ANDRIUS_IDLE_VARIATION_3:
		break;
	case Client::FSM_ANDRIUS_HAIL_SHAKE_START:
	{
		if (m_bEffectExecuted == FALSE)
		{
			// 바람 스킬 범위 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_WINDRANGE_HAIL;
			desc.fLifeTime = 7.5f;
			vSelfPos += vSelfLook;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 0.03f, 0.1f, 0.03f, 0.f };

			// 스킬 범위 바람 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
				return E_FAIL;

			desc.vScale = { 0.08f, 0.08f, 0.08f, 0.f };
			// 스킬 범위 바람 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
				return E_FAIL;

			m_bEffectExecuted = TRUE;
		}
	}
	break;
	case Client::FSM_ANDRIUS_HAIL_SHAKING:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 2.f)
		{
			m_fPatternTimer = 0.f;
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_HAIL_SHAKE_END);
		}
	}
	break;
	case Client::FSM_ANDRIUS_HAIL_SHAKE_END:
		m_bEffectExecuted = FALSE;
		m_bIsHailing = TRUE;
		break;
	case Client::FSM_ANDRIUS_DEAD_HOWL_START:
		m_pGameInstance->Stop_AssetFX(L"AndriusMeteorite");

		break;
	case Client::FSM_ANDRIUS_DEAD_HOWLING:
	{
		m_fPatternTimer += fTimeDelta;
		if (m_fPatternTimer >= 3.f)
		{
			if (m_bDropChest == FALSE)
			{
				CBossChest::BOSS_CHEST	tBossChest = { };
				tBossChest.eLevel = static_cast<LEVELID>(m_pGameInstance->Get_CurrentLevel());
				XMStoreFloat4(&tBossChest.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				tBossChest.vReservePosition = _float4{ 861.662f, 25.f, 186.045f, 1.f };


				tBossChest.DropList.push_back({ 124, 10000 });
				tBossChest.DropList.push_back({ 125, 100 });
				tBossChest.DropList.push_back({ 125, 100 });

				tBossChest.DropList.push_back({ 127, 1 });

				tBossChest.DropList.push_back({ 134, 2 });
				tBossChest.DropList.push_back({ 138, 2 });
				tBossChest.DropList.push_back({ 142, 2 });
				tBossChest.DropList.push_back({ 146, 2 });

				if (FAILED(m_pGameInstance->Add_Object_Layer(tBossChest.eLevel, TEXT("Prototype_GameObject_Boss_Chest"), L"Layer_BossChest", &tBossChest)))
					return E_FAIL;

				m_bDropChest = TRUE;
			}

			m_Parts[PART_BODY].pPartySlot[0]->Release_ColliderID();
			m_bIsDead = TRUE;
		}
	}
		break;
	case Client::FSM_ANDRIUS_END:
		break;
	default:
		break;
	}

	return S_OK;
}

/// <summary>
/// 우박 패턴
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CAndrius::Pattern_Hail(_float fTimeDelta)
{
	m_fHailTimer += fTimeDelta;
	m_fPlayerHailTimer += fTimeDelta;
	m_fRandomHailTimer += fTimeDelta;

	if (m_fHailTimer >= 20.f)
		m_bIsHailing = FALSE;

	if (m_fPlayerHailTimer >= 3.f)
	{
		m_fPlayerHailTimer = 0.f;

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
		switch (m_eCurrentPlayerState)		// 플레이어의 현재 상태에서 따라서 미리 떨어지는 부분의 위치를 조정
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

		_vector vPlayerHailPos = vPlayerPos + vPlayerLook * fDistanceValue;
		vPlayerHailPos.m128_f32[1] = 0.f;

		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_ICECONE;
		desc.fLifeTime = 2.f;
		desc.vSpawnPos = vPlayerHailPos;
		desc.vScale = { 0.4f, 0.4f, 0.4f, 0.f };

		// vPlayerHailPos 에 고드름 생성
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_IceCone", L"Layer_Skill", &desc)))
			return E_FAIL;
	}

	if (m_fRandomHailTimer >= 1.f)
	{
		m_fRandomHailTimer = 0.f;

		_float fRandomAngle = m_pGameInstance->Get_Float_Random(0.f, XM_2PI);
		_float fRandomRadius = m_pGameInstance->Get_Float_Random(0.f, 20.f);

		// 랜덤한 위치를 계산
		_vector vRandomHailPos = XMVectorSet
		(
			fRandomRadius * cos(fRandomAngle),	// X 축
			0.f,								// Y 축
			fRandomRadius * sin(fRandomAngle),  // Z 축
			0.f							        // W
		);

		// m_vFieldCenterPos에 이 랜덤한 위치를 더하여 최종 월드 좌표
		vRandomHailPos = XMVectorAdd(m_vFieldCenterPos, vRandomHailPos);

		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_ICECONE;
		desc.fLifeTime = 2.f;
		desc.vSpawnPos = vRandomHailPos;
		desc.vScale = { 0.4f, 0.4f, 0.4f, 0.f };

		// vRandomHailPos 에 고드름 생성
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_IceCone", L"Layer_Skill", &desc)))
			return E_FAIL;
	}

	return S_OK;
}

/// <summary>
/// 얼음 기둥 패턴
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CAndrius::Pattern_IcePillar(_float fTimeDelta)
{
	return S_OK;
}

/// <summary>
/// 얼음 충격파 패턴
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CAndrius::Pattern_IceShockWave(_float fTimeDelta)
{
	m_fIceShockWaveTotalTimer += fTimeDelta;

	if (m_iIceShockWaveIndex >= 15)
	{
		m_bIsIceShockWaving = FALSE;
		m_iIceShockWaveIndex = 3;
		m_iIceShockWaveIce_Floor_Index = 3;
	}
	
	if (m_fIceShockWaveTotalTimer >= 0.1f)
	{
		_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		// 얼음 바닥 생성
		CEffect::EFFECT_DESC Effectdesc = {};
		Effectdesc.eEffectID = EFFECT_ICE_FLOOR_ICECONE;
		Effectdesc.fLifeTime = 1.f;
		Effectdesc.vScale = { 0.1f, 0.1f, 0.1f, 0.f };
		_vector vIceSpawnPos = vSelfPos + vSelfLook * (m_iIceShockWaveIce_Floor_Index * 2.5f);

		//if(m_iIceShockWaveIce_Floor_Index % 2 == 0)
		//	vIceSpawnPos.m128_f32[1] = 0.01f;
		//else
		//	vIceSpawnPos.m128_f32[1] = 0.02f;

		vIceSpawnPos.m128_f32[1] = m_pGameInstance->Get_Float_Random(0.113f, 0.114f);
		Effectdesc.vSpawnPos = vIceSpawnPos;
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Effect_Ice_Floor"), TEXT("Layer_Ice_Floor"), &Effectdesc)))
			return E_FAIL;

		m_iIceShockWaveIce_Floor_Index++;
	}

	if (m_fIceShockWaveTotalTimer >= 0.2f) // 실행되고 0.2초가 지나고, 0.2초마다
	{
		_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		// 얼음 충격파 생성
		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_ICESHOCKWAVE;
		desc.fLifeTime = 1.0f;
		desc.vDir = vSelfLook;
		desc.vSpawnPos = vSelfPos + vSelfLook * (m_iIceShockWaveIndex * 2.f);
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_IceShockWave", L"Layer_Skill", &desc)))
			return E_FAIL;
		
		m_iIceShockWaveIndex++;

		if (m_iIceShockWaveIndex >= 15) // 총 10개 생성되면 멈추기
		{
			m_bIsIceShockWaving = FALSE;
			m_iIceShockWaveIndex = 3;
			m_iIceShockWaveIce_Floor_Index = 3;
			
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iID, TRUE);
		}

		m_fIceShockWaveTotalTimer = 0.0f; // 타이머를 초기화
	}

	return S_OK;
} 

/// <summary>
/// 바람 칼날 패턴
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CAndrius::Pattern_WindBlade(_float fTimeDelta)
{
	if (m_bIsAlreadyWindBladeShot == FALSE)
	{
		m_fPatternTimer += fTimeDelta;

		if (m_fPatternTimer >= 0.5f)
		{
			m_fPatternTimer = 0.f;

			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			_vector qRotationLeft = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-10.0f));
			_vector qRotationRight = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(10.0f));
			_vector vDirLeft = XMVector3Rotate(vSelfLook, qRotationLeft);
			_vector vDirRight = XMVector3Rotate(vSelfLook, qRotationRight);

			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_WINDBLADE;
			desc.fLifeTime = 15.f;
			desc.vDir = vSelfLook;
			desc.vSpawnPos = vSelfPos +vSelfLook;
			desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindBlade", L"Layer_Skill", &desc)))
				return E_FAIL;

			desc.vSpawnPos = vSelfPos;
			desc.vDir = vDirLeft;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindBlade", L"Layer_Skill", &desc)))
				return E_FAIL;

			desc.vDir = vDirRight;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindBlade", L"Layer_Skill", &desc)))
				return E_FAIL;

			m_bIsAlreadyWindBladeShot = TRUE;
		}
	}

	return S_OK;
}

/// <summary> 
/// /// 플레이어가 왼쪽에 있는지, 오른쪽에 있는지 판별하는 함수, // True면 왼쪽, False 면 오른쪽
/// </summary>
/// <returns></returns>
_bool CAndrius::VerifyIsPlayerOnLeft()
{
	// 플레이어 트랜스폼 가져와서
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	// 플레이어를 향하는 방향벡터 구하기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	// 플레이어를 향하는 벡터와 수직인 벡터
	_vector vPerpendicular = XMVector3Normalize(XMVectorSet(vDirectionToPlayer.m128_f32[2], 0.0f, -vDirectionToPlayer.m128_f32[0], 0.0f));

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

	return FALSE;
}

/// <summary>
/// 플레이어가 가까이에 있는지 판별하는 함수
/// </summary>
/// <returns></returns>
_bool CAndrius::VerifyIsPlayerNearby()
{
	// 플레이어 트랜스폼 가져와서
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_float fDistanceToPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - vSelfPos));

	return fDistanceToPlayer < 9.f ? TRUE : FALSE;
}

/// <summary>
/// 플레이어가 앞에 있는지 뒤에 있는지 판별하는 함수, // True면 앞에 있음, False면 뒤에 있음
/// </summary>
/// <returns></returns>
_bool CAndrius::VerifyIsPlayerInFront()
{
	// 플레이어 트랜스폼 컴포넌트를 가져와서
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	// 플레이어의 위치 벡터 구하기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	// 플레이어를 향하는 방향 벡터를 구하기
	_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	// 자신의 전방 벡터를 구하기
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	// 두 벡터의 내적을 계산
	_float fDotResult = XMVectorGetX(XMVector3Dot(vSelfLook, vDirectionToPlayer));

	// 내적의 결과가 양수라면 플레이어는 몬스터의 앞쪽
	return fDotResult > 0 ? TRUE : FALSE;
}

/// <summary>
/// 안드리우스가 벽쪽으로 돌진 중인가 판별
/// </summary>
/// <returns></returns>
_bool CAndrius::VerifyIsImminentWallCollisionForAndrius()
{
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	// 중앙으로부터 거리가 40이상이라면?
	if (XMVectorGetX(XMVector3Length(vSelfPos - m_vFieldCenterPos)) >= 40.f)
	{
		// 중앙에서 안드리우스를 향하는 방향 벡터 
		_vector vDirCenterToAndrius = XMVector3Normalize(vSelfPos - m_vFieldCenterPos);

		_float fDot = XMVectorGetX(XMVector3Dot(vDirCenterToAndrius, vSelfLook));
		fDot = max(-1.0f, min(1.0f, fDot));
		_float fAngle = acosf(fDot) * (180.0f / XM_PI);

		// 벽쪽으로 향해 있다면
		if (fAngle < 90.f)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

/// <summary>
/// 다음 패턴 결정해주는 코드
/// </summary>Ee
void CAndrius::DecideNextState()
{
	if (m_bIsPhase2 == FALSE)		// 1페이즈라면
	{
		_int iRandomAttack = m_pGameInstance->Get_Int_Random(0, 1);

		if (iRandomAttack == 0)			// Idle
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_IDLE);
		}
		else if (iRandomAttack == 1)	// 걷기
		{
			if (VerifyIsPlayerOnLeft() == TRUE)
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
			}
			else
			{
				m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_WALK_RIGHT);
			}
		}
	}
	else if (m_bIsPhase2 == TRUE)		// 2페이즈라면
	{
		switch (m_FSM_NextState)
		{
		case Client::FSM_ANDRIUS_TURN_FAST_LEFT:
			m_FSM_NextState = FSM_ANDRIUS_END;
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_FAST_LEFT);
			break;
		case Client::FSM_ANDRIUS_TURN_FAST_RIGHT:
			m_FSM_NextState = FSM_ANDRIUS_END;
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_FAST_RIGHT);
			break;
		case Client::FSM_ANDRIUS_END:
			m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_IDLE);
			break;
		default:
			break;
		}
	}
}

/// <summary>
/// 가까이 있을 때  랜덤 공격 (페이즈1)
/// </summary>
void CAndrius::Random_Near_Attack_Phase1()
{
	_int iRandomAttack = m_pGameInstance->Get_Int_Random(0, 1);
	//iRandomAttack = 1;
	switch (iRandomAttack)
	{
	case 0:
		if (VerifyIsPlayerOnLeft())
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_SCRATCH_RIGHT); // 오른발 할퀴기
			m_bClawClock = FALSE;
			m_bSweep = FALSE;
		}
		else
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_SCRATCH_LEFT);  // 왼발 할퀴기
			m_bClawClock = TRUE;
			m_bSweep = FALSE;
		}
		break;
	case 1:
		if (VerifyIsPlayerOnLeft())
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_SWEEP_LEFT);		// 반시계 휩쓸기
			m_bClawClock = FALSE;
			m_bSweep = TRUE;
		}
		else
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_SWEEP_RIGHT);	// 시계 휩쓸기
			m_bClawClock = TRUE;
			m_bSweep = TRUE;
		}
		break;
	case 2:
		break;
	}

	m_bClaw = TRUE;
}

/// <summary>
/// 멀리 있을 때 랜덤 공격 (페이즈1)
/// </summary>
void CAndrius::Random_Far_Attack_Phase1()
{
	_int iRandomAttack = m_pGameInstance->Get_Int_Random(1, 2);
	//iRandomAttack = 2;
	switch (iRandomAttack)
	{
	case 0:
		if (VerifyIsPlayerOnLeft())
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_ICICLESHOOT_LEFT);
		else
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_ICICLESHOOT_RIGHT);
		break;
	case 1:
		if (VerifyIsPlayerOnLeft())
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_JUMP_READY_LEFT);
		else
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_JUMP_READY_RIGHT);
		break;
	case 2:
		m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_RUSH_READY);
		break;
	}
}

/// <summary>
/// 가까이 있을 때 랜덤 공격 (페이즈2)
/// </summary>
void CAndrius::Random_Near_Attack_Phase2()
{
	_int iRandomAttack = m_pGameInstance->Get_Int_Random(0, 1);

	switch (iRandomAttack)
	{
	case 0:			// 얼음 할퀴기
		if(VerifyIsPlayerOnLeft())
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_WIND_SCRATCH_RIGHT);
			m_bClawClock = FALSE;
			m_bSweep = FALSE;
		}
		else
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_WIND_SCRATCH_LEFT);
			m_bClawClock = TRUE;
			m_bSweep = FALSE;
		}
		break;
	case 1:			// 꼬리치기
		//if(VerifyIsPlayerOnLeft())
		//	m_pFiniteStateMachineCom->Change_RegulateState(ANIM_ANDRIUS_TAILSWEEP_RIGHT);
		//else 
		//	m_pFiniteStateMachineCom->Change_RegulateState(ANIM_ANDRIUS_TAILSWEEP_LEFT);

		if (VerifyIsPlayerOnLeft())
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_SWEEP_LEFT);		// 반시계 휩쓸기
			m_bClawClock = FALSE;
			m_bSweep = TRUE;
		}
		else
		{
			m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_SWEEP_RIGHT);	// 시계 휩쓸기
			m_bClawClock = TRUE;
			m_bSweep = TRUE;
		}
		break;
	}
	m_bClaw = TRUE;
}

/// <summary>
/// 멀리 있을 때 랜덤 공격 (페이즈2)
/// </summary>
void CAndrius::Random_Far_Attack_Phase2()
{
	_int iRandomAttack = m_pGameInstance->Get_Int_Random(0, 1);

	switch (iRandomAttack)
	{
	case 0:
		m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_ICE_SHOCK_WAVE_READY);
		break;
	case 1:
		m_pFiniteStateMachineCom->Change_RegulateState(FSM_ANDRIUS_WIND_SHOCK_WAVE_READY);
		break;
	}
}

void CAndrius::Collision_State()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	_int iWeaponID = -1;
	vector<CGameObject*> vecPlayer;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	{
		if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[0])->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;
			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);

			if (m_bClaw == TRUE)
			{
				if ((eState >= FSM_ANDRIUS_SCRATCH_LEFT && eState <= FSM_ANDRIUS_SWEEP_RIGHT) || 
					eState == FSM_ANDRIUS_WIND_SCRATCH_LEFT || eState == FSM_ANDRIUS_WIND_SCRATCH_RIGHT)
				{
					m_bClaw = FALSE;
					CSkill_Claw::CLAW_DESC tDesc = {};
					tDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					tDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					tDesc.vPosition.m128_f32[1] += 2.f;
					tDesc.vScale = XMVectorSet(0.1f, 0.1f, 0.15f, 0.f);
					tDesc.bReverse = !m_bClawClock;

					if (m_bSweep == FALSE)
					{
						if (m_bClawClock == TRUE)
							tDesc.fAngle = 30.f;
						else
							tDesc.fAngle = 150.f;
					}

					if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Claw", L"Layer_Effect", &tDesc)))
						return;
				}
			}

			if (eState == FSM_ANDRIUS_JUMP || eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP)
			{
				CImpact_Glow::IMPACT_DESC tDesc = {};
				tDesc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
				tDesc.eELEMENT = ELEMENT_FROST;
				tDesc.pParent = (CGameObject*)this;
				tDesc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				tDesc.vScale = { 3.f, 3.5f, 1.f, 1.f };
				tDesc.fRadius = { 5.f };
				tDesc.iFanSize = 75;

				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tDesc)))
					return;
			}

		}
		else if (eState == FSM_ANDRIUS_WIND_SHOCK_WAVE)
		{
			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
		}
	}
	else
	{
		if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[0])->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;

			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}

			if (m_bClaw == TRUE)
			{
				if ((eState >= FSM_ANDRIUS_SCRATCH_LEFT && eState <= FSM_ANDRIUS_SWEEP_RIGHT) ||
					eState == FSM_ANDRIUS_WIND_SCRATCH_LEFT || eState == FSM_ANDRIUS_WIND_SCRATCH_RIGHT)
				{
					m_bClaw = FALSE;
					CSkill_Claw::CLAW_DESC tDesc = {};
					tDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					tDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					tDesc.vPosition.m128_f32[1] += 2.f;
					tDesc.vScale = XMVectorSet(0.1f, 0.1f, 0.15f, 0.f);
					tDesc.bReverse = !m_bClawClock;

					if (m_bSweep == FALSE)
					{
						if (m_bClawClock == TRUE)
							tDesc.fAngle = 40.f;
						else
							tDesc.fAngle = 140.f;
					}

					if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_Claw", L"Layer_Effect", &tDesc)))
						return;
				}
			}

		}
		else if (eState == FSM_ANDRIUS_WIND_SHOCK_WAVE)
		{
			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}
		}
	}


	/* 안드리우스 상태에 따라 충돌 처리할 콜라이더를 정하고 플레이어와의 충돌 검사 */
	if (eState == FSM_ANDRIUS_SCRATCH_LEFT || eState == FSM_ANDRIUS_WIND_SCRATCH_LEFT)
	{
		iWeaponID = static_cast<CBody_Andrius*>(m_Parts[PART_BODY].pPartySlot[0])->Get_WeaponCollisionID(CBody_Andrius::WEAPON_LHAND);
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);

	}
	else if (eState == FSM_ANDRIUS_SCRATCH_RIGHT || eState == FSM_ANDRIUS_WIND_SCRATCH_RIGHT)
	{
		iWeaponID = static_cast<CBody_Andrius*>(m_Parts[PART_BODY].pPartySlot[0])->Get_WeaponCollisionID(CBody_Andrius::WEAPON_RHAND);
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);
	}
	else if (eState == FSM_ANDRIUS_SWEEP_LEFT || eState == FSM_ANDRIUS_SWEEP_RIGHT)
	{
		iWeaponID = static_cast<CBody_Andrius*>(m_Parts[PART_BODY].pPartySlot[0])->Get_WeaponCollisionID(CBody_Andrius::WEAPON_SWEEP);
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_WEAPON, iWeaponID, COLL_PLAYER_BODY);
	}
	else if (eState == FSM_ANDRIUS_RUSH || eState == FSM_ANDRIUS_JUMP || eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP || eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH)
	{
		iWeaponID = static_cast<CBody_Andrius*>(m_Parts[PART_BODY].pPartySlot[0])->Get_CollisionID();
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

			_float4 vDir = {};
			XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			vDir.w = 1.f;

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_END, FSM_HIT_THROW, 6.f, 3.f, vDir);
		}
		else if (eState == FSM_ANDRIUS_SWEEP_LEFT || eState == FSM_ANDRIUS_SWEEP_RIGHT || eState == FSM_ANDRIUS_JUMP || eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP)
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

				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_END, FSM_HIT_THROW, 6.f, 3.f, vDir);
			}
		}
		else if (eState == FSM_ANDRIUS_SWEEP_LEFT || eState == FSM_ANDRIUS_SWEEP_RIGHT || eState == FSM_ANDRIUS_JUMP || eState == FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP)
		{
			m_bHitPlayer = TRUE;
		}
	}
	
}

void CAndrius::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CAndrius::Set_Damage(fDamage * 2.f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CAndrius::Set_Damage(fDamage * 1.5f, ELEMENT_FIRE);
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

void CAndrius::Set_ElementUI(ELEMENT eElement)
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

void CAndrius::Set_Damage(_float fDamage, ELEMENT eElement)
{
	m_pHpBar->Set_CurrentHp(m_fHp -= fDamage); /* 체력바 */
	__super::Create_DamageFont(fDamage, eElement); /* 데미지 폰트 */
}

HRESULT CAndrius::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndrius::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);
	m_vFieldCenterPos = ObjDesc.vInitPos;
	m_eCreatureID = static_cast<CREATUREID>(ObjDesc.iObjectType);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_Controller(2.0f, 2.5f, 0.0f, m_pTransformCom)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	// 초기 설정
	_float4 float4Position;
	XMStoreFloat4(&float4Position, m_vFieldCenterPos);
	float4Position.x += 40.f;
	m_pTransformCom->Set_Scale({ 1.f, 1.f, 1.f, 0.f });
	m_pController->Set_Position(float4Position);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(-90.f));

	m_fHp = m_fMaxHp;

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>
	{
		FSM_ANDRIUS_SCRATCH_LEFT,
		FSM_ANDRIUS_SCRATCH_RIGHT,
		FSM_ANDRIUS_SWEEP_LEFT,
		FSM_ANDRIUS_SWEEP_RIGHT,
		FSM_ANDRIUS_RUSH_READY,
		FSM_ANDRIUS_TURN_RUSH_LEFT,
		FSM_ANDRIUS_TURN_RUSH_RIGHT,
		FSM_ANDRIUS_TURN_RUSH_PATTERN_RUSH,
		FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP,
		FSM_ANDRIUS_ICICLESHOOT_RIGHT,
		FSM_ANDRIUS_ICICLESHOOT_LEFT,
		FSM_ANDRIUS_JUMP_READY_LEFT,
		FSM_ANDRIUS_JUMP_READY_RIGHT,
		FSM_ANDRIUS_JUMP_READY_FRONT,
		// Phase 2
		FSM_ANDRIUS_ICE_SHOCK_WAVE,
		FSM_ANDRIUS_WIND_SHOCK_WAVE,
		FSM_ANDRIUS_WIND_SCRATCH_LEFT,
		FSM_ANDRIUS_WIND_SCRATCH_RIGHT,
		FSM_ANDRIUS_ICE_SHOCK_WAVE_READY,
		FSM_ANDRIUS_WIND_SHOCK_WAVE_READY, 
		FSM_ANDRIUS_DEAD_HOWL_START
	});

	//m_bIsPhase2 = true;
	m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_WALK_FIRST);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_WALK_LEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_IDLE);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRATCH_LEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SWEEP_LEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_JUMP_READY_LEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_JUMP_READY_RIGHT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_PHAZE2_ENTER_HOWL_START);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_RUSH_READY);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SWEEP_RIGHT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_WIND_SCRATCH_LEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_WIND_SCRATCH);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRAM_LEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_SCRAM_RIGHT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_TURN_RUSH_PATTERN_JUMP);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_ANDRIUS_PHAZE2_ENTER_HOWL_END);

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
	m_pHpBar->Set_OwnerInfo(TEXT("안드리우스"), TEXT("울프의 영주"), 95);

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_pGameInstance->PlayBGM(L"BGM_Andrius_Phase1.mp3", 0.5f);

	m_strStatusTag = Get_ParticleTag(L"Andrius");

	return S_OK;
}

HRESULT CAndrius::PreviousTick(_float fTimeDelta)
{
	if (m_bIsDead == TRUE)
	{
		m_pGameInstance->Add_InteractionCount(L"Andrius");
		m_pGameInstance->PlayBGM(L"Mond_Field.mp3", 0.6f);
		//m_pGameInstance->PlayBGM(L"MondstadtBGM.mp3", 0.6f);

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);

		Set_Destroy(TRUE);
	}

	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[0])->Get_CombinedMatrix();
		if (m_fHp > 0.f)
			Add_Target(this);

		if (m_fSelfDamage != 0.f)
		{
			Set_Damage(m_fSelfDamage, ELEMENT_ELECTRIC);
			m_fSelfDamage = 0.f;
		}

		for (auto& pPart : m_Parts)
		{
			for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
			{
				if (pPart.pPartySlot[iSlot] != nullptr)
				{
					pPart.pPartySlot[iSlot]->PreviousTick(fTimeDelta);

					/* 데미지 받았을 때 */
					_float fDamage = pPart.pPartySlot[iSlot]->Get_Damage();
					if (fDamage != 0)
					{
						ELEMENT eElement = pPart.pPartySlot[iSlot]->Get_ReceiveElement();
						Set_ElementUI(eElement);
						ELEMENTAL_REACTION eReaction = pPart.pPartySlot[iSlot]->Get_ElementalReaction(); /* 원소반응 */
						ElementalReaction(fDamage, eReaction);

						CAndrius::Set_Damage(fDamage, eElement);

						CModel* pModel = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[0]->Find_Component(TEXT("Com_Model")));
						m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, pModel->Get_CombinedMatrix("HitBox"), m_pTransformCom, 0.5f);
					}
				}
			}
		}

		m_pHpBar->PreviousTick(fTimeDelta);
		m_pElement_OHD->PreviousTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CAndrius::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CAndrius::Pattern(fTimeDelta)))
			return E_FAIL;

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		if (m_bIsHailing)		// 우박 패턴 중이라면
		{
			if (FAILED(CAndrius::Pattern_Hail(fTimeDelta)))
				return E_FAIL;
		}

		if (m_bIsIceShockWaving)	// 얼음 충격파 패턴중이라면
		{
			if (FAILED(CAndrius::Pattern_IceShockWave(fTimeDelta)))
				return E_FAIL;
		}

		for (auto& pPart : m_Parts)
		{
			for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
			{
				if (pPart.pPartySlot[iSlot] != nullptr)
					pPart.pPartySlot[iSlot]->Tick(fTimeDelta);
			}
		}

		CAndrius::Collision_State();

		m_pHpBar->Tick(fTimeDelta);
		//m_pElement_OHD->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f, 20.f);
		m_pElement_OHD->Update_Matrix((_float)(551.f - (_float)g_iWinSizeX / 2.f), (_float)(-(120.f - (_float)g_iWinSizeY / 2.f)), 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);

		m_pController->Apply_Gravity(fTimeDelta);
	}

	//Render Fonts
	m_pGameInstance->Render_Font(L"Font_Default", L"안드리우스", _float2(820.f, 40.f), 0.5f, Colors::White);
	m_pGameInstance->Render_Font(L"Font_Default", L"울프의 영주", _float2(820.f, 50.f), 0.3f, Colors::Gray);

	return S_OK;
}

HRESULT CAndrius::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		Clear_Target();

		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
			{
				if (pPart.pPartySlot[iSlot] != nullptr)
					pPart.pPartySlot[iSlot]->LateTick(fTimeDelta);
			}
		}

		m_pHpBar->LateTick(fTimeDelta);
		m_pElement_OHD->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CAndrius::Render()
{
	return S_OK;
}

CAndrius* CAndrius::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndrius* pInstance = new CAndrius(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CAndrius"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAndrius::Clone(void* pArg)
{
	CAndrius* pInstance = new CAndrius(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CAndrius"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAndrius::Free()
{
	__super::Free();
	
	if (m_bCloned == true)
	{
		Safe_Release(m_pHpBar);
		Safe_Release(m_pElement_OHD);
	}

	for (auto& pPart : m_Parts)
	{
		for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
		{
			Safe_Release(pPart.pPartySlot[iSlot]);
		}
	}
	m_Parts.clear();
}