#include "stdafx.h"
#include "Dvalin.h"

#include "Part_Body.h"
#include "Part_Status.h"
#include "Camera_Main.h"

#include "GameInstance.h"
#include "Hp_Bar.h"
#include "Skill_FireCore.h"
#include "Body_Dvalin.h"

#include "Effect.h"
#include "Slash_Trail.h"
#include "Eff_Impact_Glow.h"
#include "FX_Meteorite.h"
#include "Updraft.h"

#include "BossChest.h"

#include "Dungeon.h"

CDvalin::CDvalin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CDvalin::CDvalin(const CDvalin& rhs)
	: CCreature(rhs)
{
}

_float4 CDvalin::Get_PlatformPosition()
{
	CBody_Dvalin* pBody = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot);

	_float4 vPlatformPosition;
	XMStoreFloat4(&vPlatformPosition, pBody->Get_BoneWorldPosition("Bip001 Spine01"));

	return vPlatformPosition;
}

HRESULT CDvalin::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = MONSTER_DVALIN;
	tBodyDesc.eType = CTY_MONSTER;

	CREATUREID eCreatureID = MONSTER_DVALIN;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	tSlots = { };

	/* For. Part_Status */
	CPart_Status::PARTSTATUS_DESC tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	pPartObject = CPart_Status::Create_Status(MONSTER_DVALIN, tStatusDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

void CDvalin::BreatheEffect()
{
	CSlash_Trail::SLASH_DESC tDesc = {};
	tDesc.eTrail_Type = CSlash_Trail::TRAIL_RANDOM;
	tDesc.pParentsTransform = m_pBreatheTransformCom;
	tDesc.eElementColor = ELEMENT_FROST;
	tDesc.iExceptAxis = CSlash_Trail::AXIS_X + CSlash_Trail::AXIS_Z;
	tDesc.fTrailSpeed = m_pGameInstance->Get_Float_Random(1.f, 1.7f);
	tDesc.fLifeTime = 1.f;
	//tDesc.fHeight = m_pGameInstance->Get_Float_Random(0.f, 1.f);
	tDesc.fHeight = m_pGameInstance->Get_Float_Random(0.3f, 1.4f);
	_float fRandomScale = m_pGameInstance->Get_Float_Random(1.1f, 2.f);
	tDesc.vScale = XMVectorSet(fRandomScale * 2.f, fRandomScale * 2.f, 0.8f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
		return;

	_matrix BreatheMatrix = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Get_BreatheMatrix();
	_vector vDir = BreatheMatrix.r[2] * -1.f;

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPos = BreatheMatrix.r[3];
	vPos.m128_f32[1] += 0.5f;
	vLook.m128_f32[1] = 0.f; XMVector3Normalize(vLook);

	CEffect::EFFECT_DESC desc = {};

	
	for (_uint i = 0; i < 10; ++i)
	{
		desc.eEffectID = EFFECT_FOG;
		desc.fLifeTime = m_pGameInstance->Get_Float_Random(4.f, 6.f);
		vPos.m128_f32[0] += m_pGameInstance->Get_Float_Random(-0.7f, 0.7f);
		vPos.m128_f32[1] += m_pGameInstance->Get_Float_Random(-0.7f, 0.7f);
		vPos.m128_f32[2] += m_pGameInstance->Get_Float_Random(-0.7f, 0.7f);
		desc.vSpawnPos = vPos;
		_float fSize = m_pGameInstance->Get_Float_Random(1.f, 2.f);
		desc.vScale = { fSize, fSize, 0.f, 0.f };
		desc.fSpeed = m_pGameInstance->Get_Float_Random(18.f, 23.f);
		desc.vDir = vDir;
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Fog", L"Layer_Effect", &desc)))
			return;
	}
}

/// <summary>
/// Dvalin 의 패턴을 제어하는 함수
/// </summary>
/// <param name="fTimeDelta"></param>
/// <returns></returns>
HRESULT CDvalin::Pattern(_float fTimeDelta)
{
	// 현재 내 상태 가져오기
	FSM_DVALIN_STATE m_eCurrentState = static_cast<FSM_DVALIN_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	// 플레이어 정보 구해놓기
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_vPlayerPos = vPlayerPos;
	vPlayerPos.m128_f32[1] = 0.f;
	vPlayerPos.m128_f32[3] = 1.f;
	// 내 정보들 구해놓기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vSelfRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	// 플레이어를 향하는 방향벡터
	_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	// 드발린 위치 설정
	SetDvalinPosForPlatform(vSelfLook);
	if(m_bCanRot)
		SetDvalinRotForPlatform(m_ePlayerCurrentPlatform);

	// 체력이 다 까였다면
	if (m_fHp <= 0.f)
	{
		m_fHp = 0.f;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_DIE);
		cout << "드발린 사망" << endl;

		Set_FloorCollapse(FALSE, 0);		// 0번 
		Set_FloorCollapse(FALSE, 1);		// 1번 
		Set_FloorCollapse(FALSE, 2);		// 2번 
		Set_FloorCollapse(FALSE, 3);		// 3번 
		Set_FloorCollapse(FALSE, 4);		// 4번 
		Set_FloorCollapse(FALSE, 5);		// 5번 
		Set_FloorCollapse(FALSE, 5);		// 6번 
		Set_FloorCollapse(FALSE, 6);		// 7번 
		Set_FloorCollapse(FALSE, 7);		// 8번
	}

	// 쉴드가 다 까인다면
	if (m_fWindShield_Hp <= 0.f && !m_bAlreadyGroggy 
		&& m_eCurrentState != FSM_DVALIN_GROGGY_BS && m_eCurrentState != FSM_DVALIN_GROGGY_LOOP
		&& m_eCurrentState != FSM_DVALIN_GROGGY_AS)
	{
		if (m_bAlreadyGroggy == FALSE)
		{
			static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Set_ActivatedColliderBody(FALSE);
			CUpdraft::UPDRAFT_DESC tDesc = {};
			tDesc.vColliderSize = _float3(150.f, 10.f, 150.f);
			tDesc.vPosition = _float4(0.f, -10.f, 0.f, 1.f);
			tDesc.fLifeTime = 20.f;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
		}

		m_bAlreadyGroggy = TRUE;
		m_bIsWindShieldActive = FALSE;
		m_fWindShield_Hp = 0.f;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_GROGGY_BS);
		m_iWindShield_BreakCount++;
		cout << "드발린 그로기" << endl;
		m_pHpBar->Set_CurrentShield(m_fWindShield_Hp);
	}
	

	// 플레이어 활동 반경 제한
	_vector vWorldCenterPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vDirCenterToPlayer = vPlayerPos - vWorldCenterPos;
	_vector vNorDirCenterToPlayer = XMVector3Normalize(vDirCenterToPlayer);
	_float fDistanceCenterToPlayer = XMVectorGetX(XMVector3Length(vDirCenterToPlayer));

	std::cout << "중앙부터 거리 : " << fDistanceCenterToPlayer << endl;

	if (fDistanceCenterToPlayer < 87.f)
	{
		_vector vFinalPos = vWorldCenterPos + (vNorDirCenterToPlayer * 87.001f);
		_float4 f4FinalPos = {};
		XMStoreFloat4(&f4FinalPos, vFinalPos);

		pPlayer->Get_PlayerInfo(iIndex).pController->Set_Position(f4FinalPos);
		std::cout << "플레이어 활동반경 제한" << endl;
	}




	switch (m_eCurrentState)
	{
	case Client::FSM_DVALIN_THINK:			// 패턴 판단
	{
		if (m_bIsAttacking == FALSE)
		{
			if (m_PrevPatternWasAttack == TRUE)
			{
				if (m_bIsFront == TRUE)
				{
					m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_IDLE2);
					cout << "드발린 패턴 : 앞 IDLE" << endl;
				}
				else
				{
					m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_IDLE);
					cout << "드발린 패턴 : 뒤 IDLE" << endl;
				}
			}
			else
			{	
				Identify_Dvalin_AttackType();
			}
		}
		else if (m_bIsAttacking == TRUE)
		{
			Dvalin_Attack_Pattern();
		}
	}
	break;
	case Client::FSM_DVALIN_FIRST:		// 등장
		m_bIsFront = FALSE;
		break;
	case Client::FSM_DVALIN_IDLE2:		// IDLE (앞)
		m_PrevPatternWasAttack = FALSE;
		m_bIsFront = TRUE;
		break;
	case Client::FSM_DVALIN_IDLE:		// IDLE (뒤)
		m_PrevPatternWasAttack = FALSE;
		m_bIsFront = FALSE;
		break;
	case Client::FSM_DVALIN_ATTACK_GNAW_LEFT:	// 물기 ( 드발린 기준 왼쪽 )
		m_bIsFront = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_GNAW_RIGHT:	// 물기 ( 드발린 기준 오른쪽 )
		m_bIsFront = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_BREATHELEFT:	// 브레스 ( 드발린 기준 오른쪽 )
		m_bIsFront = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_BREATHERIGHT:	// 브레스 ( 드발린 기준 왼쪽 )
		m_bIsFront = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT:	// 돌진 ( 뒤 -> 앞 )
		m_bIsFront = TRUE;
		m_PrevPatternWasRush = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK:	// 돌진 ( 앞 -> 뒤 )
		m_bIsFront = FALSE;
		m_PrevPatternWasRush = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_1:	// 돌진 ( 앞 -> 앞 ) // 자작
		m_bIsFront = TRUE;
		m_PrevPatternWasRush = TRUE;
		break;
	case Client::FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS:	// 플랫폼 붕괴 BS
		m_bIsFront = FALSE;
		break;
	case Client::FSM_DVALIN_ATTACK_PULSE_BOMBS:			// 파동탄
		m_bIsFront = FALSE;
		break;
	case Client::FSM_DVALIN_ATTACK_FLOORCOLLAPSE:		// 플랫폼 붕괴
	{
		m_bIsFront = FALSE;

		if (m_bIsFloorCollapse_1 == TRUE)
		{
			m_bIsFloorCollapse_1 = FALSE;

			cout << "첫번째 플랫폼 붕괴" << endl;

			//Set_FloorCollapse(TRUE, m_ePlayerCurrentPlatform);			// 0번
			//Set_FloorCollapse(TRUE, m_ePlayerCurrentPlatform + 1);		// 1번 
			//Set_FloorCollapse(TRUE, m_ePlayerCurrentPlatform + 2);		// 2번
			Set_FloorCollapse(TRUE, 0);
			Set_FloorCollapse(TRUE, 1);
			Set_FloorCollapse(TRUE, 2);

			m_pGameInstance->Stop_Particle(L"Dvl_StarRain", 3.f);
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[0])))
				return E_FAIL;
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[1])))
				return E_FAIL;
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[2])))
				return E_FAIL;
			//if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[m_ePlayerCurrentPlatform])))
			//	return E_FAIL;
			//if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[m_ePlayerCurrentPlatform + 1])))
			//	return E_FAIL;
			//if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[m_ePlayerCurrentPlatform + 2])))
			//	return E_FAIL;
		}
		else if (m_bIsFloorCollapse_2 == TRUE)
		{
			m_bIsFloorCollapse_2 = FALSE;

			cout << "두번째 플랫폼 붕괴" << endl;

			//Set_FloorCollapse(TRUE, m_ePlayerCurrentPlatform);			// 0번
			//Set_FloorCollapse(TRUE, m_ePlayerCurrentPlatform + 1);		// 1번 
			//Set_FloorCollapse(TRUE, m_ePlayerCurrentPlatform + 2);		// 2번

			Set_FloorCollapse(TRUE, 3);
			Set_FloorCollapse(TRUE, 5);

			Set_FloorCollapse(TRUE, 7);
			Set_FloorCollapse(TRUE, 8);

			m_pGameInstance->Stop_Particle(L"Dvl_StarRain", 3.f);
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[3])))
				return E_FAIL;
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[5])))
				return E_FAIL;
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[7])))
				return E_FAIL;
			if (FAILED(Set_CollapseFX(m_vecDvalinPlatfromRot[8])))
				return E_FAIL;
		}
	}
	break;
	case Client::FSM_DVALIN_ATTACK_OMEGA_BOMB:			// 오메가 파동탄
		m_bIsFront = FALSE;
		break;
	case Client::FSM_DVALIN_ATTACK_STARFALL:			// 별똥별
		m_bIsFront = FALSE;
		break;
	case Client::FSM_DVALIN_GROGGY_LOOP:				// 그로기 LOOP
		m_bIsFront = TRUE;
		break;
	case Client::FSM_DVALIN_GROGGY_AS:					// 그로기 AS
		m_bIsFront = TRUE;
		m_fWindShield_Hp = m_fWindShield_MaxHp;
		m_bAlreadyGroggy = FALSE;
		m_pHpBar->Set_CurrentShield(m_fWindShield_Hp);
		static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Set_ActivatedColliderBody(TRUE);
		cout << "드발린 쉴드 회복" << endl;
		break;
	case Client::FSM_DVALIN_GROGGY_BS:					// 그로기 BS
		m_bIsFront = TRUE;
		break;	
	case Client::FSM_DVALIN_DIE:						// 사망
		m_bIsFront = TRUE;
		break;
	}

	return S_OK;
}

/// <summary>
/// 지형 플랫폼 패스 변경
/// </summary>
/// <param name="_SetCollapse"></param>
/// <param name="__platformNum"></param>
/// <returns></returns>
HRESULT CDvalin::Set_FloorCollapse(_bool _SetCollapse, _int __platformNum)
{
	if (_SetCollapse == TRUE)
	{
		CUpdraft::UPDRAFT_DESC tDesc = {};
		tDesc.vColliderSize = _float3(2.f, 6.f, 2.f);
		switch (__platformNum)
		{
		case 0:
			
			tDesc.vPosition = _float4(-95.2543f, -0.137542f, 15.5571f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(-95.102f, -0.0368215f, -19.0601f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 1:
			tDesc.vPosition = _float4(-83.1382f, -0.0058463f, 51.6006f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(-61.2108f, -0.138047f, 76.9488f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 2:
			tDesc.vPosition = _float4(-29.81f, 0.0f, 90.669f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(2.24479f, -0.13656f, 96.904f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 3:
			tDesc.vPosition = _float4(37.7938f, 0.0f, 87.943f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(66.8985f, -0.125198f, 72.5525f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 4:
			tDesc.vPosition = _float4(84.183f, -0.0159733f, 42.7604f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(97.0111f, -0.129614f, 11.4595f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 5:
			tDesc.vPosition = _float4(92.2347f, 0.142585f, -22.4609f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(81.8007f, 0.00338629f, -53.8715f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 6:
			tDesc.vPosition = _float4(62.7565f, -0.0187659f, -75.6543f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(33.4232f, -0.139369f, -92.5279f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 7:
			tDesc.vPosition = _float4(4.67843f, -0.0195173f, -96.527f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(-28.3375f, -0.135986f, -91.6359f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		case 8:
			tDesc.vPosition = _float4(-82.619f, -0.13702f, -51.1535f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;

			tDesc.vPosition = _float4(-60.5254f, -0.024512f, -76.3833f, 1.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
				return E_FAIL;
			break;
		default:
			break;
		}
	}

	CDungeon* pDungeon = static_cast<CDungeon*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_StormTerror", 1));

	__platformNum = (__platformNum + 2) % 9;

	if(_SetCollapse)
		pDungeon->Set_ElementPassID(__platformNum, VTXMESH_DVALIN_PLATFORM_DESTROY);
	else
		pDungeon->Set_ElementPassID(__platformNum, VTXMESH_TERRAIN);

	return S_OK;
}

/// <summary>
/// 어떤 타입의 공격을 할것인지 판별
/// </summary>
/// <returns></returns>
HRESULT CDvalin::Identify_Dvalin_AttackType()
{
	_int iRandomAttack = 0;

	if (m_ePlayerCurrentPlatform != m_eDvalinCurrentPlatform)
	{
		if(m_bIsFront)
			iRandomAttack = DVALIN_ATTACK_RUSH_FRONT_TO_FRONT;
		else
			iRandomAttack = DVALIN_ATTACK_RUSH_BACK_TO_FRONT;
	}
	else if (m_bIsFront)
	{
		rotate(vecFrontPatternOrder.begin(), vecFrontPatternOrder.begin() + 1, vecFrontPatternOrder.end());
		iRandomAttack = vecFrontPatternOrder[0];
	}
	// 위쪽이고, 이전에 돌진(뒤->앞)을 했었고, 쉴드가 이미 한번 까졌었다면
	else if (!m_bIsFront && m_iWindShield_BreakCount == 1 && m_bAlreadyRoar == FALSE)
	{
		iRandomAttack = DVALIN_ATTACK_FLOORCOLLAPSE;
		m_bAlreadyRoar = TRUE;
	}
	else if (!m_bIsFront && m_iWindShield_BreakCount == 2 && m_bAlreadyRoar_2 == FALSE)
	{
		iRandomAttack = DVALIN_ATTACK_FLOORCOLLAPSE;
		m_bAlreadyRoar_2 = TRUE;
	}
	//else if (!m_bIsFront && m_iWindShield_BreakCount >= 3 && m_bIsStarFall == FALSE)
	//{
	//	iRandomAttack = DVALIN_ATTACK_STARFALL;
	//	m_bIsStarFall = TRUE;
	//}
	
	// 뒤쪽이고, 이전 패턴이 돌진이 아니었다면
	else if (!m_bIsFront && !m_PrevPatternWasRush)
	{
		iRandomAttack = DVALIN_ATTACK_RUSH_BACK_TO_FRONT;
		m_PrevPatternWasRush = TRUE;
	}
	// 위쪽이고, 이전에 돌진(뒤->앞)을 했었다면
	else
	{
		rotate(vecBackPatternOrder.begin(), vecBackPatternOrder.begin() + 1, vecBackPatternOrder.end());
		iRandomAttack = vecBackPatternOrder[0];
		m_PrevPatternWasRush = FALSE;
	}

	switch (iRandomAttack)
	{
	case 0:		// 물기
		m_eAttack_Type = DVALIN_ATTACK_GNAW;
		if (VerifyIsPlayerOnLeft() == TRUE)
			m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_GNAW_LEFT);
		else
			m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_GNAW_RIGHT);

		m_bIsAttacking = TRUE;
		cout << "드발린 패턴 : 물기" << endl;
		break;
	case 1:		// 브레스
		m_eAttack_Type = DVALIN_ATTACK_BREATH;
		if (VerifyIsPlayerOnLeft() == TRUE)
			m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_BREATHERIGHT);
		else
			m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_BREATHELEFT);
		
		m_bIsAttacking = TRUE;
		m_PrevPatternWasAttack = TRUE;
		cout << "드발린 패턴 : 브레스" << endl;
		break;
	case 2:	
		// 돌진 ( 뒤 -> 앞 )
		m_eAttack_Type = DVALIN_ATTACK_RUSH_BACK_TO_FRONT;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT);
		m_bIsAttacking = TRUE;
		m_PrevPatternWasAttack = TRUE;
		cout << "드발린 패턴 : 돌진 (뒤 -> 앞)" << endl;
		break;
	case 3:		// 돌진 ( 앞 -> 뒤 )
		m_eAttack_Type = DVALIN_ATTACK_RUSH_FRONT_TO_BACK;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK);
		m_bIsAttacking = TRUE;
		m_PrevPatternWasAttack = TRUE;
		cout << "드발린 패턴 : 돌진 (앞 -> 뒤)" << endl;
		break;
	case 4:		// 돌진 ( 앞 -> 앞 )
		m_eAttack_Type = DVALIN_ATTACK_RUSH_FRONT_TO_FRONT;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_1);
		m_bIsAttacking = TRUE;
		m_PrevPatternWasAttack = TRUE;
		cout << "드발린 패턴 : 돌진 (앞 -> 앞)" << endl;
		break;
	case 5:		// 파동탄
		m_eAttack_Type = DVALIN_ATTACK_PULSE_BOMBS;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_PULSE_BOMBS);
		m_bIsAttacking = TRUE;
		cout << "드발린 패턴 : 파동탄" << endl;
		break;
	case 6:		// 왕파동탄
		m_eAttack_Type = DVALIN_ATTACK_OMEGA_BOMB;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_OMEGA_BOMB);
		m_bIsAttacking = TRUE;
		cout << "드발린 패턴 : 왕파동탄" << endl;
		break;
	case 7:		// 플랫폼 붕괴
		m_eAttack_Type = DVALIN_ATTACK_FLOORCOLLAPSE;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS);
		m_bIsAttacking = TRUE;
		m_iAttackIndex++;
		cout << "드발린 패턴 : 플랫폼 붕괴" << endl;
		break;
	case 8:		// 별똥별
		m_eAttack_Type = DVALIN_ATTACK_STARFALL;
		m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_STARFALL);
		m_bIsAttacking = TRUE;
		m_iAttackIndex++;
		cout << "드발린 패턴 : 별똥별" << endl;
		break;
	default:
		break;
	}

	return S_OK;
}

/// <summary>
/// 공격 제어 함수
/// </summary>
/// <returns></returns>
HRESULT CDvalin::Dvalin_Attack_Pattern()
{
	switch (m_eAttack_Type)
	{
	case Client::CDvalin::DVALIN_ATTACK_GNAW:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_BREATH:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_RUSH_BACK_TO_FRONT:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_RUSH_FRONT_TO_BACK:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_RUSH_FRONT_TO_FRONT:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_PULSE_BOMBS:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_OMEGA_BOMB:
		m_bIsAttacking = FALSE;
		break;
	case Client::CDvalin::DVALIN_ATTACK_FLOORCOLLAPSE:
	{
		m_pGameInstance->Play_Particle(L"Dvl_StarRain", L"Dvl_StarRain", PARTICLE_DVL_STARRAIN, TEX_PARTICLE_DVLSTARRAIN, nullptr, nullptr);

		if (m_iAttackIndex == 1)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_FLOORCOLLAPSE);
			m_iAttackIndex = 0;
			m_bIsAttacking = FALSE;
		}
	}
	break;
	case Client::CDvalin::DVALIN_ATTACK_STARFALL:
		m_bIsAttacking = FALSE;
		break;
	}

	return S_OK;
}

/// <summary> 
/// /// 플레이어가 왼쪽에 있는지, 오른쪽에 있는지 판별하는 함수, // True면 왼쪽, False 면 오른쪽
/// </summary>
/// <returns></returns>
_bool CDvalin::VerifyIsPlayerOnLeft()
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
_bool CDvalin::VerifyIsPlayerNearby(_float _fDistance)
{
	// 플레이어 트랜스폼 가져와서
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_float fDistanceToPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - vSelfPos));

	return fDistanceToPlayer < _fDistance ? TRUE : FALSE;
}

/// <summary>
/// 플레이어가 앞에 있는지 뒤에 있는지 판별하는 함수, // True면 앞에 있음, False면 뒤에 있음
/// </summary>
/// <returns></returns>
_bool CDvalin::VerifyIsPlayerInFront()
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
/// 플레이어의 현재 플랫폼 판별
/// </summary>
void CDvalin::VerifyPlayerCurrentPlatform()
{
	// 플레이어 트랜스폼 컴포넌트를 가져와서
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	// 플레이어의 위치 구하기
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[1] = 0.f;

	// 원점에서 플레이어를 향하는 방향 벡터를 구하기
	_vector vDirectionCenterToPlayer = XMVector3Normalize(vPlayerPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_float fDot = XMVectorGetX(XMVector3Dot(m_vCenterStandardDir, vDirectionCenterToPlayer));
	fDot = max(-1.0f, min(1.0f, fDot));
	_float fAngle = acosf(fDot) * (180.0f / XM_PI);

	// 플레이어를 향하는 벡터와 수직인 벡터
	_vector vPerpendicular = XMVector3Normalize(XMVectorSet(vDirectionCenterToPlayer.m128_f32[2], 0.0f, -vDirectionCenterToPlayer.m128_f32[0], 0.0f));

	// 자신의 Look과 플레이어를 향하는 벡터와 수직인 벡터를 내적
	_float fDotResult = XMVectorGetX(XMVector3Dot(m_vCenterStandardDir, XMVector3Normalize(vPerpendicular)));

	if (fDotResult > 0)		// 왼쪽에 있을때
	{
		if (fAngle >= 0.f && fAngle < 30.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_3;
		else if (fAngle >= 30.f && fAngle < 70.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_2;
		else if (fAngle >= 70.f && fAngle < 110.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_1;
		else if (fAngle >= 110.f && fAngle < 150.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_9;
		else if (fAngle >= 150.f && fAngle < 180.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_8;
	}
	else if (fDotResult < 0)	// 오른쪽 있을때
	{
		if (fAngle >= 0.f && fAngle < 10.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_3;
		else if (fAngle >= 10.f && fAngle < 50.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_4;
		else if (fAngle >= 50.f && fAngle < 90.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_5;
		else if (fAngle >= 90.f && fAngle < 130.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_6;
		else if (fAngle >= 130.f && fAngle < 170.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_7;
		else if (fAngle >= 170.f && fAngle < 180.f)
			m_ePlayerCurrentPlatform = PLATFORM_INDEX_8;
	}
	else if (fDotResult == -1)	// 정반대
	{
		m_ePlayerCurrentPlatform = PLATFORM_INDEX_8;
	}

	//cout << "현재 플레이어 플랫폼 : " << m_ePlayerCurrentPlatform + 1 << "번" << endl;
}

void CDvalin::SetDvalinPosForPlatform(_vector _selfLook)
{
	_vector vFinalDvalinPos = m_vInitPos + _selfLook * 20.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFinalDvalinPos);
}

void CDvalin::SetDvalinRotForPlatform(DVALIN_PLATFORM_NUMBER _platformNum)
{
	if (_platformNum == PLATFORM_INDEX_END)
		MSG_BOX(TEXT("드발린 보스전 플레이어 현재 플랫폼 이상함!!"));
	else
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vecDvalinPlatfromRot[_platformNum]));

	m_eDvalinCurrentPlatform = _platformNum;
}

HRESULT CDvalin::Set_CollapseFX(_float fDegree)
{
	CFX_Meteorite::METEO_DESC	tMeteoDesc = { };
	tMeteoDesc.vTargetPosition	= _float4(0.f, 0.f, 97.f, 1.f);
	XMStoreFloat4(&tMeteoDesc.vTargetPosition, XMVector4Transform(XMLoadFloat4(&tMeteoDesc.vTargetPosition), XMMatrixRotationY(XMConvertToRadians(fDegree))));

	tMeteoDesc.vInitPosition	= XMLoadFloat4(&tMeteoDesc.vTargetPosition) + _vector{ 0.f, 50.f, 0.f, 0.f };

	tMeteoDesc.fScale			= 15.f;
	tMeteoDesc.ePosType			= CFX_Meteorite::METEO_DESC::POS_FIX;
	tMeteoDesc.fDistance		= XMVectorGetX(XMVector3Length(XMLoadFloat4(&tMeteoDesc.vTargetPosition) - tMeteoDesc.vInitPosition));
	tMeteoDesc.vMeteoriteColor	= _float4{ 0.4392132f, 0.6797233f, 0.8867924f, 1.f };

	_vector vDir = XMVector3Normalize(XMLoadFloat4(&tMeteoDesc.vTargetPosition) - tMeteoDesc.vInitPosition);
	XMStoreFloat4(&tMeteoDesc.vDirection, vDir * -1.f);

	m_pGameInstance->Play_AssetFX(L"Meteorite", Get_ParticleTag(L"Meteorite"), &tMeteoDesc);

	return S_OK;
}

void CDvalin::Collision_State(_float fTimeDelta)
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

			if (eState == FSM_DVALIN_ATTACK_BREATHELEFT || eState == FSM_DVALIN_ATTACK_BREATHERIGHT) /* 브레스 */
			{
				m_bBreathe = !m_bBreathe;
			}

			if (eState == FSM_DVALIN_ATTACK_PULSE_BOMBS) // 파동탄
			{
				CFX_Meteorite::METEO_DESC tMeteoDesc = { };
				tMeteoDesc.ePosType = CFX_Meteorite::METEO_DESC::POS_FIX;
				tMeteoDesc.fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&tMeteoDesc.vTargetPosition) - tMeteoDesc.vInitPosition));
				tMeteoDesc.vMeteoriteColor = _float4{ 0.4392132f, 0.6797233f, 0.8867924f, 1.f };
				tMeteoDesc.fScale = 2.f;
				for (_int iMeteorite = 0; iMeteorite < 2; ++iMeteorite)
				{
					_float fRandX = m_pGameInstance->Get_Float_Random(-2.f, 2.f);
					_float fRandZ = m_pGameInstance->Get_Float_Random(-2.f, 2.f);

					XMStoreFloat4(&tMeteoDesc.vTargetPosition, pPlayer->Get_Position());
					tMeteoDesc.vTargetPosition.x += fRandX;
					tMeteoDesc.vTargetPosition.y = 0.f;
					tMeteoDesc.vTargetPosition.z += fRandZ;

					if(iMeteorite == 0)
						tMeteoDesc.vInitPosition = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Get_BoneWorldPosition("Bone_Wings_L1_18");
					else
						tMeteoDesc.vInitPosition = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Get_BoneWorldPosition("Bone_Wings_R1_04");
					_vector vDir = XMVector3Normalize(XMLoadFloat4(&tMeteoDesc.vTargetPosition) - tMeteoDesc.vInitPosition);
					XMStoreFloat4(&tMeteoDesc.vDirection, vDir * -1.f);

					m_pGameInstance->Play_AssetFX(L"Meteorite", Get_ParticleTag(L"Meteorite"), &tMeteoDesc);
				}
			}
			else if (eState == FSM_DVALIN_ATTACK_OMEGA_BOMB)
			{
				CFX_Meteorite::METEO_DESC tMeteoDesc = { };
				tMeteoDesc.ePosType = CFX_Meteorite::METEO_DESC::POS_FIX;
				tMeteoDesc.fScale = 5.f;
				tMeteoDesc.fSpeed = 3.f;
				tMeteoDesc.fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&tMeteoDesc.vTargetPosition) - tMeteoDesc.vInitPosition));
				tMeteoDesc.vMeteoriteColor = _float4{ 0.4392132f, 0.6797233f, 0.8867924f, 1.f };

				XMStoreFloat4(&tMeteoDesc.vTargetPosition, pPlayer->Get_Position());
				tMeteoDesc.vTargetPosition.y = 0.f;

				tMeteoDesc.vInitPosition = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Get_BoneWorldPosition("Breathe");
				_vector vDir = XMVector3Normalize(XMLoadFloat4(&tMeteoDesc.vTargetPosition) - tMeteoDesc.vInitPosition);
				XMStoreFloat4(&tMeteoDesc.vDirection, vDir * -1.f);

				m_pGameInstance->Play_AssetFX(L"Meteorite", Get_ParticleTag(L"Meteorite"), &tMeteoDesc);

			}
		}
		else if (eState == FSM_DVALIN_ATTACK_PULSE_BOMBS || eState == FSM_DVALIN_ATTACK_OMEGA_BOMB || // 투사체 패턴
			eState == FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS || eState == FSM_DVALIN_ATTACK_STARFALL)
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

			if (eState == FSM_DVALIN_ATTACK_BREATHELEFT || eState == FSM_DVALIN_ATTACK_BREATHERIGHT) /* 브레스 */
			{
				m_bBreathe = !m_bBreathe;
			}

		}
		else if (eState == FSM_DVALIN_ATTACK_PULSE_BOMBS || eState == FSM_DVALIN_ATTACK_OMEGA_BOMB || // 투사체 패턴
			eState == FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS || eState == FSM_DVALIN_ATTACK_STARFALL)
		{
			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}
		}
	}

	if (m_bBreathe == TRUE && eState != FSM_DVALIN_ATTACK_BREATHELEFT && eState != FSM_DVALIN_ATTACK_BREATHERIGHT)
		m_bBreathe = FALSE;

	if (m_bBreathe == TRUE)
	{
		_matrix BreatheMatrix = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot)->Get_BreatheMatrix();

		PxQueryFilterData tQueryFilterData = PxQueryFilterData();
		tQueryFilterData.flags = PxQueryFlag::eSTATIC;

		_float3 vDir = {};
		_float3 vBreatheColliderPos = {};
		XMStoreFloat3(&vDir, BreatheMatrix.r[2] * -1.f);
		XMStoreFloat3(&vBreatheColliderPos, BreatheMatrix.r[3]);
		m_pGameInstance->RayCast_Position(vBreatheColliderPos, vDir, 20.f, &vBreatheColliderPos, tQueryFilterData);

		BreatheMatrix.r[3] = XMVectorSet(vBreatheColliderPos.x, vBreatheColliderPos.y, vBreatheColliderPos.z, 1.f);

		CImpact_Glow::IMPACT_DESC descc = {};
		descc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
		descc.eELEMENT = ELEMENT_FROST;
		descc.pParent = (CGameObject*)this;
		descc.vSpawnPos = BreatheMatrix.r[3];

		_float fRandomScale = m_pGameInstance->Get_Float_Random(1.f, 3.f);
		descc.vScale = { 1.5f * fRandomScale, 2.1f * fRandomScale, 1.f, 1.f };
		descc.fRadius = m_pGameInstance->Get_Float_Random(3.5f, 5.2f);
		descc.iFanSize = m_pGameInstance->Get_Int_Random(60, 75);
		
		m_pBreatheTransformCom->Set_WorldMatrix(BreatheMatrix);
		m_pBreatheColliderCom->Update(BreatheMatrix);

		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &descc)))
			return;

		CDvalin::BreatheEffect();

		vector<CGameObject*> HitPlayer = m_pGameInstance->Find_Collision_Objects(m_pBreatheColliderCom, COLL_PLAYER_BODY);

		if (pPlayer->Get_CombatMode() == FALSE)
		{
			if (HitPlayer.size() != 0)
			{
				_float4 vDir = {};
				XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
				vDir.w = 1.f;

				pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_END, FSM_HIT_THROW, 6.f, 3.f, vDir, 0.8f);
			}
		}
		else
		{
			for (auto PlayerBody : HitPlayer)
			{
				PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID(eIndex);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);

				_float4 vDir = {};
				XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
				vDir.w = 1.f;

				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_END, FSM_HIT_THROW, 6.f, 3.f, vDir, 0.8f);
			}
		}
	}
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBreatheColliderCom);
#endif


	/* 상태에 따라 충돌 처리할 콜라이더를 정하고 플레이어와의 충돌 검사 */
	if (eState == FSM_DVALIN_ATTACK_GNAW_LEFT || eState == FSM_DVALIN_ATTACK_GNAW_RIGHT) // 물기
	{
		iWeaponID = static_cast<CBody_Dvalin*>(m_Parts[0].pPartySlot)->Get_WeaponColliderID(CBody_Dvalin::DVALIN_COLL_HEAD);
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_BODY, iWeaponID, COLL_PLAYER_BODY);
	}
	else if (
		eState == FSM_DVALIN_ATTACK_RUSH_BACK_TO_FRONT ||
		eState == FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK ||
		eState == FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_1) // 돌진
	{
		iWeaponID = static_cast<CBody_Dvalin*>(m_Parts[0].pPartySlot)->Get_WeaponColliderID(CBody_Dvalin::DVALIN_COLL_RUSH);
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
	}

	/* 붕괴된 땅에 서있을 때 데미지 */
	CDungeon* pDungeon = static_cast<CDungeon*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_StormTerror", 1));
	if (m_iPlatformindex != (_int)m_ePlayerCurrentPlatform)
	{
		m_iPlatformindex = (_int)m_ePlayerCurrentPlatform;
		m_fCollapseDamage = 0.f;
		m_fCollapseDelay = 0.f;
	}

	_int iIndex = (m_ePlayerCurrentPlatform + 2) % 9;
	if (pDungeon->isCollaps(iIndex) == TRUE && pPlayer->Get_InAir() == FALSE)
	{
		m_fCollapseDelay += fTimeDelta;
		if (m_fCollapseDelay >= 3.f)
		{
			m_fCollapseDelay = 0.f;
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
			m_fCollapseDamage += m_pGameInstance->Get_Float_Random(200.f, 400.f);
			pPlayer->Set_Damage(m_fCollapseDamage, ELEMENT_END, FSM_END);
		}
	}

}

void CDvalin::ElementalReaction(_float* pDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		*pDamage *= 2.f;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		*pDamage *= 1.5f;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_MELT:
		*pDamage *= 2.f;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_MELT_REVERSE:
		*pDamage *= 1.5f;
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

void CDvalin::Set_ElementUI(ELEMENT eElement)
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

void CDvalin::Set_Damage(_float fDamage, ELEMENT eElement)
{
	ELEMENT eImune = ELEMENT_END;
	if (eElement == ELEMENT_WIND && m_fWindShield_Hp > 0.f)
	{
		fDamage = 0.f;
		eImune = ELEMENT_WIND;
	}
	else if (m_fWindShield_Hp > 0.f)
	{
		m_fWindShield_Hp -= fDamage;
		if (m_fWindShield_Hp < 0)
		{
			m_pHpBar->Set_CurrentHp(m_fHp += m_fWindShield_Hp); /* 체력바 */
			m_fWindShield_Hp = 0.f;
		}
		m_pHpBar->Set_CurrentShield(m_fWindShield_Hp);
	}
	else
		m_pHpBar->Set_CurrentHp(m_fHp -= fDamage); /* 체력바 */

	_vector vPos = {};
	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	if (eState == FSM_DVALIN_GROGGY_LOOP)
	{
		CBody_Dvalin* pBody = static_cast<CBody_Dvalin*>(m_Parts[PART_BODY].pPartySlot);
		vPos = pBody->Get_BoneWorldPosition("hitbox_Seal01");
		vPos.m128_f32[1] += 0.5f;
	}
	__super::Create_DamageFont(fDamage, eElement, vPos, eImune); /* 데미지 폰트 */
}

HRESULT CDvalin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDvalin::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);
	m_vInitPos = ObjDesc.vInitPos;
	m_eCreatureID = static_cast<CREATUREID>(ObjDesc.iObjectType);

	//m_vInitPos = XMVectorSet(-10.f, 0.f, 0.f, 1.f);
	//m_vInitPos = XMVectorSet(-20.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CCreature::Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(CDvalin::Add_Parts(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitPos);
	m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>
	{
	});

	m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_FIRST);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_BACK);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH_FRONT_TO_FRONT_1);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_IDLE);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_IDLE2);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_RUSH);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_BREATHELEFT);
	//m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_GNAW_LEFT);

	// m_pFiniteStateMachineCom->Change_ForceState(FSM_DVALIN_ATTACK_FLOORCOLLAPSE_BS);
	// m_bIsAttacking = TRUE;
	// m_iAttackIndex++;
	// m_eAttack_Type = DVALIN_ATTACK_FLOORCOLLAPSE;


	CBounding_SPHERE::BOUNDING_SPHERE_DESC		tSphereDesc = {};
	tSphereDesc.fRadius = 4.f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE_Breathe"), (CComponent**)&m_pBreatheColliderCom, &tSphereDesc)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Breathe"), (CComponent**)&m_pBreatheTransformCom)))
		return E_FAIL;


	COrtho::ORTHO_DESC tOrthoDesc;
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
	tOrthoDesc.fPosition = { (_float)(771.f - (_float)g_iWinSizeX / 2.f), (_float)(-(90.f - (_float)g_iWinSizeY / 2.f)) };
	tOrthoDesc.fSize = { 500.f,8.f };
	tOrthoDesc.ePage = PAGE_MAIN;

	m_pHpBar = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));
	m_pHpBar->Set_MaxShield(m_fWindShield_MaxHp);
	m_pHpBar->Set_CurrentShield(m_fWindShield_Hp);
	m_pHpBar->Set_MaxHp(m_fMaxHp);
	m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_BOSS);
	m_pHpBar->Set_CurrentHp(m_fHp);
	m_pHpBar->Set_OwnerInfo(TEXT("풍마룡 드발린"), TEXT(" 지난날의 천공의 왕 "), 90);

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_strStatusTag = Get_ParticleTag(L"Dvalin");


	m_pGameInstance->PlayBGM(L"Dvalin_BGM.mp3", 0.5f);
	return S_OK;
}

HRESULT CDvalin::PreviousTick(_float fTimeDelta)
{
	if (m_bIsDying)
	{
		if (m_bDropChest == FALSE)
		{
			CBossChest::BOSS_CHEST	tBossChest = { };
			tBossChest.eLevel = static_cast<LEVELID>(m_pGameInstance->Get_CurrentLevel());
			XMStoreFloat4(&tBossChest.vPosition, m_vPlayerPos);
			tBossChest.vReservePosition = _float4{ 373.978f, 61.4704f, 674.976f, 1.f };

			tBossChest.DropList.push_back({ 124, 10000 });
			tBossChest.DropList.push_back({ 124, 10000 });
			tBossChest.DropList.push_back({ 124, 10000 });
			tBossChest.DropList.push_back({ 124, 10000 });
			tBossChest.DropList.push_back({ 125, 100 });
			tBossChest.DropList.push_back({ 125, 100 });

			tBossChest.DropList.push_back({ 126, 1 });

			if (FAILED(m_pGameInstance->Add_Object_Layer(tBossChest.eLevel, TEXT("Prototype_GameObject_Boss_Chest"), L"Layer_BossChest", &tBossChest)))
				return E_FAIL;

			m_bDropChest = TRUE;
		}

		m_pGameInstance->Add_InteractionCount(L"Dvalin");
		Set_Destroy(TRUE);
	}

	/*if (m_pGameInstance->Key_Down('T'))
	{
		cout << m_ePlayerCurrentPlatform << endl;
	}*/

	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		//_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[0])->Get_CombinedMatrix();
		//if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&CombinedWorldMatrix).r[3], 5.f) && m_fHp > 0.f)
		//	Add_Target(this);

		if (m_fSelfDamage != 0.f)
		{
			Set_Damage(m_fSelfDamage, ELEMENT_ELECTRIC);
			m_fSelfDamage = 0.f;
		}

		for (auto& pPart : m_Parts)
		{
			if (pPart.pPartySlot != nullptr)
			{
				pPart.pPartySlot->PreviousTick(fTimeDelta);

				/* 데미지 받았을 때 */
				_float fDamage = pPart.pPartySlot->Get_Damage();
				if (m_bAlreadyGroggy == TRUE)
					fDamage *= 3.5f;
				if (fDamage != 0)
				{
					ELEMENT eElement = pPart.pPartySlot->Get_ReceiveElement();
					Set_ElementUI(eElement);
					ELEMENTAL_REACTION eReaction = pPart.pPartySlot->Get_ElementalReaction(); /* 원소반응 */
					ElementalReaction(&fDamage, eReaction);

					CDvalin::Set_Damage(fDamage, eElement);

					CModel* pModel = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot->Find_Component(TEXT("Com_Model")));
					m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, pModel->Get_CombinedMatrix("HitBox"), m_pTransformCom, 0.5f);
				}
			}
		}

		m_pHpBar->PreviousTick(fTimeDelta);
		m_pElement_OHD->PreviousTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CDvalin::Tick(_float fTimeDelta)
{
#pragma region 치트
	// 정인

	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down('9'))
	{
		m_fWindShield_Hp = 0.f;
		//cout << "드발린 쉴드 감소 -1000" << endl;
		cout << "드발린 쉴드 잔량 : " << m_fWindShield_Hp << endl;
		m_pHpBar->Set_CurrentShield(m_fWindShield_Hp);
	}

	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down('8'))
	{
		m_fHp -= 220000.f;
		cout << "드발린 체력 감소 -220000" << endl;
		cout << "드발린 체력 잔량 : " << m_fHp << endl;
		m_pHpBar->Set_CurrentHp(m_fHp);
	}

	_vector AAA = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//cout << "드발린 위치 : " << AAA.m128_f32[0] << ", "
		//<< AAA.m128_f32[1] << ", "
		//<< AAA.m128_f32[2] << endl;ㅋ

	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_UP))
	{
		AAA.m128_f32[0] += 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AAA);
	}
	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_DOWN))
	{
		AAA.m128_f32[0] -= 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AAA);
	}	
	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_LEFT))
	{
		AAA.m128_f32[2] += 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AAA);
	}
	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_RIGHT))
	{
		AAA.m128_f32[2] -= 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AAA);
	}
	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_PRIOR))
	{
		AAA.m128_f32[1] += 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AAA);
	}
	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_NEXT))
	{
		AAA.m128_f32[1] -= 5.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AAA);
	}
#pragma endregion

	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		VerifyPlayerCurrentPlatform();

		if (FAILED(CDvalin::Pattern(fTimeDelta)))
			return E_FAIL;

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			if (pPart.pPartySlot != nullptr)
				pPart.pPartySlot->Tick(fTimeDelta);
		}

		CDvalin::Collision_State(fTimeDelta);

		m_pHpBar->Tick(fTimeDelta);
		//m_pElement_OHD->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f, 20.f);
		m_pElement_OHD->Update_Matrix((_float)(551.f - (_float)g_iWinSizeX / 2.f), (_float)(-(124.f - (_float)g_iWinSizeY / 2.f)), 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CDvalin::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		Clear_Target();

		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			if (pPart.pPartySlot != nullptr)
				pPart.pPartySlot->LateTick(fTimeDelta);
		}

		m_pHpBar->LateTick(fTimeDelta);
		m_pElement_OHD->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CDvalin::Render()
{
	return S_OK;
}

CDvalin* CDvalin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDvalin* pInstance = new CDvalin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDvalin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDvalin::Clone(void* pArg)
{
	CDvalin* pInstance = new CDvalin(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDvalin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDvalin::Free()
{
	__super::Free();

	Safe_Release(m_pBreatheColliderCom);
	Safe_Release(m_pBreatheTransformCom);
	
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