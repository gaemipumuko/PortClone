#include "stdafx.h"
#include "Body_Abyss_Mage.h"
#include "Abyss_Mage.h"

#include "GameInstance.h"

#include "Camera_Main.h"

#include "ChainLightning.h"
#include "SkillObj.h"

CBody_Abyss_Mage::CBody_Abyss_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Abyss_Mage::CBody_Abyss_Mage(const CBody_Abyss_Mage& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Abyss_Mage::Invalidate_Animation(_float fTimeDelta)
{
	FSM_ABYSS_MAGE_STATE eState = static_cast<FSM_ABYSS_MAGE_STATE>(m_pParentFSMCom->Get_CurrentState());
	CAbyss_Mage* pAbyss_Mage = static_cast<CAbyss_Mage*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Abyss_Mage"));
	m_eCreatureID = pAbyss_Mage->Get_CreatureID();

	_float fRate = 0.f;

	switch (eState)
	{
	case Client::FSM_ABYSS_MAGE_BLINK_AS:
	{
		if (m_bBlinkSound == TRUE)
		{
			m_pGameInstance->PlaySoundW(L"Abyss_Teleport_End.mp3", 1.f);
			m_bBlinkSound = FALSE;
		}

		m_bRenderEnable = TRUE;

		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_BLINK_AS);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_BLINK_AS) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_THINK);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_BLINK_BS:
	{
		if (m_bBlinkSound == FALSE)
		{
			m_pGameInstance->PlaySoundW(L"Abyss_Teleport_On.mp3", 1.f);
			m_bBlinkSound = TRUE;
		}

		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_BLINK_BS);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		//if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_BLINK_BS) * 0.49f)
		if (fRate >= 0.85f)
		{
			m_bRenderEnable = FALSE;
			//m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_BLINK_AS);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_STANDBY_BRAINWASHING:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_STANDBY_BRAINWASHING);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_STANDBY_BRAINWASHING) * 0.49f)
		{
			//m_pParentFSMCom->Change_ForceState(FSM_DVALIN_IDLE);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_DIE_DROWN:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_FALLING_DIE:
		break;
	case Client::FSM_ABYSS_MAGE_GATHER_AS:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_GATHER_AS);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		
		if (fRate >= 1.81f)
		{
			pAbyss_Mage->Set_ElementShield_Active();
		}

		//if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_GATHER_AS) * 0.99f)
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_GATHER_AS) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_STANDBY_FIGHT);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_GATHER_BS:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_GATHER_BS);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_GATHER_BS) * 0.45f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_GATHER_LOOP);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_GATHER_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_GATHER_LOOP);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_GATHER_LOOP) * 0.45f)
		{
			m_iCycleCount++;
		}

		if (m_iCycleCount >= 5.f)
		{
			m_pGameInstance->PlaySoundW(L"Abyss_Taunt.mp3", 1.f);
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_GATHER_AS);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_GETUP:
	{
		m_bRenderEnable = TRUE;

		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_GETUP);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_GETUP) * 0.45f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_STANDBY_DIZZY);
		}
	}
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
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_ATTACK_NORMAL_ATTACK);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= 1.f && m_bAlreadyNormalAtk == FALSE)
		{
			m_bAlreadyNormalAtk = TRUE;

			// 일반 공격

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

			switch (m_eCreatureID)
			{
			case Client::MONSTER_ABYSS_MAGE_ELEC:
			{
				m_pGameInstance->PlaySoundW(L"Abyss_NormalAttack.mp3", 1.f);

				cout << "번개 심메 평타" << endl;

				CChainLightning::CHAIN_LIGHTNING_DESC tDesc = {};
				tDesc.vPointA = pPlayer->Get_Position();
				tDesc.vPointA.m128_f32[1] += 0.5f;
				tDesc.vPointB = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
				tDesc.vPointB.m128_f32[1] += 1.5f;

				pPlayer->Set_Damage(m_pGameInstance->Get_Float_Random(500.f, 800.f), ELEMENT_ELECTRIC);

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_ChainLightning", L"Layer_Effect", &tDesc)))
					return E_FAIL;
			}
			break;
			case Client::MONSTER_ABYSS_MAGE_ICE:
			{
				cout << "얼음 심메 평타" << endl;

			}
			break;
			case Client::MONSTER_ABYSS_MAGE_WATER:
			{
				cout << "물 심메 평타" << endl;

			}
			break;
			case Client::MONSTER_ABYSS_MAGE_FIRE:
			{
				cout << "불꽃 심메 평타" << endl;
				CAssetFX::ASFX_DESC tAsfxDesc = { };
				tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
				_vector aaa = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
				aaa.m128_f32[1] += 2.f;
				tAsfxDesc.vInitPosition = aaa;
				//tAsfxDesc.vInitPosition =
				//	XMVector4Transform(m_pParentTransformCom->Get_State(CTransform::STATE_POSITION), XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Fx_HeadNode")));
				tAsfxDesc.pTransform = pPlayerTransform;
				m_pGameInstance->Play_AssetFX(L"EffigyFireProjectile", Get_ParticleTag(L"EffigyFireProjectile"), &tAsfxDesc);
			}
			break;
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_ATTACK_NORMAL_ATTACK) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_THINK);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_STANDBY_RANDOMACTION_2:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_RANDOMACTION_3:
		break;
	case Client::FSM_ABYSS_MAGE_STANDBY_RANDOMACTION_1:
		break;
	case Client::FSM_ABYSS_MAGE_ATTACK_SPECIAL_ATTACK:
	{
		if (m_bSPASound == FALSE && m_bAlreadySpecialAtk != TRUE)
		{
			m_bSPASound = TRUE;
			m_pGameInstance->PlaySoundW(L"Abyss_UltimateAttack.mp3", 1.f);
		}

		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_ATTACK_SPECIAL_ATTACK);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= 2.f && m_bAlreadySpecialAtk == FALSE)
		{
			m_bSPASound = FALSE;

			m_bAlreadySpecialAtk = TRUE;

			// 스킬 공격

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

			m_bRangeAttack = TRUE;

			switch (m_eCreatureID)
			{
			case Client::MONSTER_ABYSS_MAGE_ELEC:
			{
				cout << "번개 심메 스킬" << endl;
				CSkillObj::SKILLOBJ_DESC tLightningDesc = {};
				tLightningDesc.vScale = XMVectorSet(7.5f, 2.5f, 7.5f, 0.f);
				tLightningDesc.vSpawnPos = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
				tLightningDesc.vSpawnPos.m128_f32[1] += 1.5f;
				tLightningDesc.fLifeTime = 5.f;

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

				tLightningDesc.vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
					return E_FAIL;

				tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
					return E_FAIL;

				tLightningDesc.vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
					return E_FAIL;

				tLightningDesc.vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
					return E_FAIL;

			}
			break;
			case Client::MONSTER_ABYSS_MAGE_ICE:
			{
				cout << "얼음 심메 스킬" << endl;

			}
			break;
			case Client::MONSTER_ABYSS_MAGE_WATER:
			{
				cout << "물 심메 스킬" << endl;

			}
			break;
			case Client::MONSTER_ABYSS_MAGE_FIRE:
			{
				cout << "불꽃 심메 평타" << endl;
				CAssetFX::ASFX_DESC tAsfxDesc = { };
				tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
				_vector aaa = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
				aaa.m128_f32[1] += 2.f;
				tAsfxDesc.vInitPosition = aaa;
				//tAsfxDesc.vInitPosition =
				//	XMVector4Transform(m_pParentTransformCom->Get_State(CTransform::STATE_POSITION), XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Fx_HeadNode")));
				tAsfxDesc.pTransform = pPlayerTransform;
				m_pGameInstance->Play_AssetFX(L"EffigyFireProjectile", Get_ParticleTag(L"EffigyFireProjectile"), &tAsfxDesc);
				m_pGameInstance->Play_AssetFX(L"EffigyFireProjectile", Get_ParticleTag(L"EffigyFireProjectile"), &tAsfxDesc);
				m_pGameInstance->Play_AssetFX(L"EffigyFireProjectile", Get_ParticleTag(L"EffigyFireProjectile"), &tAsfxDesc);
			}
			break;
			}

		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_ATTACK_SPECIAL_ATTACK) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_THINK);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_STANDBY_DIZZY:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_STANDBY_DIZZY);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_STANDBY_DIZZY) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_GATHER_BS);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_STANDBY_FIGHT:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_STANDBY_FIGHT);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_STANDBY_FIGHT) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_THINK);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_STANDBY_FREE:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_STANDBY_FREE);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_ABYSS_MAGE_WALK:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_WALK);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_WALK) * 0.49f)
		{
			//m_pParentFSMCom->Change_ForceState(FSM_DVALIN_IDLE);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_WARNING:
	{
		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_WARNING);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_WARNING) * 0.49f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_ABYSS_MAGE_STANDBY_FIGHT);
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_DIE:
	{
		if (m_bDeadSound == FALSE)
		{
			m_bDeadSound = TRUE;
			m_pGameInstance->PlaySoundW(L"Abyss_Die.mp3", 1.f);
		}

		m_pModelCom->Set_Animation(FSM_ABYSS_MAGE_DIE);
		fRate = m_pModelCom->Play_Abyss_MageRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_ABYSS_MAGE_DIE) * 0.49f)
		{
			m_bDead = TRUE;
		}
	}
	break;
	case Client::FSM_ABYSS_MAGE_DIE_LOOP:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_AS:
		break;
	case Client::FSM_ABYSS_MAGE_DIE_BS:
		break;
	case Client::FSM_ABYSS_MAGE_THINK:
		m_bAlreadyNormalAtk = FALSE;
		m_bAlreadySpecialAtk = FALSE;
		m_bIsFirstFrame = TRUE;
		break;
	case Client::FSM_ABYSS_MAGE_END:
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

HRESULT CBody_Abyss_Mage::Add_Component(void* pArg)
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
	
	/* For.Com_Collider_SPHERE */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC		SphereDesc = {};
	SphereDesc.fRadius = 4.f;
	SphereDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_Sphere_Attack"), (CComponent**)&m_pSphereAttackColliderCom, &SphereDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CBody_Abyss_Mage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Abyss_Mage::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Abyss_Mage::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Abyss_Mage::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;


	_matrix ColliderMatrix = m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
	ColliderMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	ColliderMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	ColliderMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	m_pSphereAttackColliderCom->Update(ColliderMatrix);

	if (m_bRangeAttack == TRUE)
	{
		m_fAttackDelay += fTimeDelta;
		if (m_fAttackDelay >= 0.5f)
		{
			++m_iAttackCount;
			m_fAttackDelay = 0.f;
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));

			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, pPlayer->Get_BodyCollisionID(PLAYER_ONE), TRUE);
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, pPlayer->Get_BodyCollisionID(PLAYER_TWO), TRUE);
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, pPlayer->Get_BodyCollisionID(PLAYER_THREE), TRUE);
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, pPlayer->Get_BodyCollisionID(PLAYER_FOUR), TRUE);

			vector<CGameObject*> HitPlayer = m_pGameInstance->Find_Collision_Objects(m_pSphereAttackColliderCom, COLL_PLAYER_BODY);
			for (auto pPlayerBody : HitPlayer)
			{
				PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(143, 233), ELEMENT_ELECTRIC);
			}
		}

		if (m_iAttackCount >= 10)
		{
			m_bRangeAttack = FALSE;
			m_fAttackDelay = 0.f;
			m_iAttackCount = 0;
		}
	}

	return S_OK;
}

HRESULT CBody_Abyss_Mage::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pSphereAttackColliderCom);
#endif

	return S_OK;
}

HRESULT CBody_Abyss_Mage::Render()
{
	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		//if (m_eTypeID == CTY_MONSTER && m_iFaceMeshIndex - 1 == siMesh)s
		//	continue;

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

void CBody_Abyss_Mage::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);
}

CBody_Abyss_Mage* CBody_Abyss_Mage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Abyss_Mage* pInstance = new CBody_Abyss_Mage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Abyss_Mage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Abyss_Mage::Clone(void* pArg)
{
	CBody_Abyss_Mage* pInstance = new CBody_Abyss_Mage(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Abyss_Mage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Abyss_Mage::Free()
{
	__super::Free();

	Safe_Release(m_pSphereAttackColliderCom);
}