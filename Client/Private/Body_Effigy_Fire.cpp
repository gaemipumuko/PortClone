#include "stdafx.h"
#include "Effigy_Fire.h"
#include "Body_Effigy_Fire.h"
#include "FX_EffigyFireClap.h"
#include "FX_EffigyFireTrsCow.h"
#include "FX_EffigyFireTrsFish.h"
#include "FX_EffigyFireTrsBomb.h"

#include "SkillObj.h"
#include "Camera_Main.h"

#include "Bone.h"
#include "GameInstance.h"

CBody_Effigy_Fire::CBody_Effigy_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Effigy_Fire::CBody_Effigy_Fire(const CBody_Effigy_Fire& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Effigy_Fire::Invalidate_Animation(_float fTimeDelta)
{
	FSM_EFFIGY_STATE eState = static_cast<FSM_EFFIGY_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	CEffigy_Fire* pEffigy_Fire = static_cast<CEffigy_Fire*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Monster"));

	switch (eState)
	{
	case Client::FSM_EFFIGY_STANDBY_FREE:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_STANDBY_FREE);
		m_pModelCom->Play_Animation(TRUE, m_pParentTransformCom, fTimeDelta);
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
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_CLAP);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bClapEff == FALSE && fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_CLAP) * 0.22f)
		{
			CFX_EffigyFireClap::CLAP_DESC tClapDesc = { };
			tClapDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tClapDesc.pTransform = m_pAuraTransform;
			tClapDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrix("Fx_HeadNode");

			m_pGameInstance->Play_AssetFX(L"EffigyFireClap", Get_ParticleTag(L"EffigyFireClap"), &tClapDesc);

			m_bClapEff = TRUE;
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_CLAP) * 0.3f)
			pEffigy_Fire->Set_IsCoreExposure(TRUE);		// 본체 코어 노출

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_CLAP) * 0.95f)
		{
			m_bClapEff = FALSE;
			pEffigy_Fire->Set_IsCoreExposure(FALSE);

			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
		}
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE_LOOP:
		break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE_AS:
		break;
	case Client::FSM_EFFIGY_ATTACK_RANGED_BS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_RANGED_BS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_RANGED_BS) * 0.95f)
		{
			CAssetFX::ASFX_DESC tAsfxDesc = { };
			tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_RELATIVE;
			tAsfxDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tAsfxDesc.pTransform = m_pMagicCircleTransform;
			m_pGameInstance->Play_AssetFX(L"EffigyFireMagicCircle", L"EffigyFireMagicCircle", &tAsfxDesc);
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_ATTACK_RANGED_LOOP);
			m_pGameInstance->PlaySoundW(L"EffigyFire_Magic.mp3");
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_RANGED_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_RANGED_LOOP);
		m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		m_fFire_Cube_Timer += fTimeDelta;

		if (m_fFire_Cube_Timer >= 0.5f && m_fFire_Cube_Timer < 1.0f)
		{
			if(m_iAttack_Cube_Index == 0)
				Spawn_Fire_Cube();
		}
		else if (m_fFire_Cube_Timer >= 1.0f && m_fFire_Cube_Timer < 1.5f)
		{
			if (m_iAttack_Cube_Index == 1)
				Spawn_Fire_Cube();

			if (m_iAttack_Cube_Index == 2)
				Spawn_Fire_Cube();
		}
		else if (m_fFire_Cube_Timer >= 1.5f && m_fFire_Cube_Timer < 2.0f)
		{
			if (m_iAttack_Cube_Index == 3)
				Spawn_Fire_Cube();

			if (m_iAttack_Cube_Index == 4)
				Spawn_Fire_Cube();
		}
		else if (m_fFire_Cube_Timer >= 2.0f && m_fFire_Cube_Timer < 2.5f)
		{
			if (m_iAttack_Cube_Index == 5)
				Spawn_Fire_Cube();

			if (m_iAttack_Cube_Index == 6)
				Spawn_Fire_Cube();
		}
		else if (m_fFire_Cube_Timer >= 2.5f && m_fFire_Cube_Timer < 3.0f)
		{
			if (m_iAttack_Cube_Index == 7)
			{
				Spawn_Fire_Cube();
				m_pGameInstance->Stop_AssetFX(L"EffigyFireMagicCircle");
			}
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_RANGED_AS:
	{
		m_fFire_Cube_Timer = 0.f;
		m_iAttack_Cube_Index = 0;

		pEffigy_Fire->Set_IsCoreExposure(TRUE);

		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_RANGED_AS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_RANGED_AS) * 0.7f)
			pEffigy_Fire->Set_IsCoreExposure(FALSE);

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_RANGED_AS) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
			Fire_Cube_Mesh_Index.clear();
		}
	}
	break;
	case Client::FSM_EFFIGY_MOVE_RUSHMOVE_LOOP:
	{
		if (m_bIsFirstFrame == TRUE)
		{
			m_bIsFirstFrame = FALSE;
			m_bRenderEnable = TRUE;
		}

		m_pModelCom->Set_Animation(FSM_EFFIGY_MOVE_RUSHMOVE_LOOP);
		m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_EFFIGY_REBRON_CUBES_REAPPEAR:
	{
		m_bIsFirstFrame = TRUE;
		m_bRenderEnable = TRUE;

		m_pModelCom->Set_Animation(FSM_EFFIGY_REBRON_CUBES_REAPPEAR);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_REBRON_CUBES_REAPPEAR) * 0.95f)
		{
			if(m_bHasMoved == TRUE)
				m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_THINK);
			else
				m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_MOVE_RUSHMOVE_LOOP);
		}
	}
	break;
	case Client::FSM_EFFIGY_STANDBY_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_STANDBY_LOOP);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_STANDBY_LOOP) * 0.95f)
		{
			if(!m_bIsIgnite)
				m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_MOVE_RUSHMOVE_LOOP);
			else
				m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_IGNITE_BS);
		}
	}
	break;
	case Client::FSM_EFFIGY_BLINK:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_BLINK);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_BLINK) * 0.75f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_THINK);
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_BS:
	{
		Fire_Cube_Mesh_Index = {};

		m_pModelCom->Set_Animation(FSM_EFFIGY_IGNITE_BS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_IGNITE_BS) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_IGNITE_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_AS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_IGNITE_AS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_IGNITE_AS) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_SUCCESS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_IGNITE_SUCCESS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_IGNITE_SUCCESS) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_IGNITE_LOOP);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_EFFIGY_IGNITE_BS02:
	{
		Fire_Cube_Mesh_Index = {};

		m_pModelCom->Set_Animation(FSM_EFFIGY_IGNITE_BS02);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_IGNITE_BS02) * 0.95f)
		{
			if(m_bIsIgniteSuccess == TRUE)		// 점화에 성공했다면?
				m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_IGNITE_SUCCESS);
			else								// 점화에 실패했다면?
				m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_IGNITE_AS);
		}
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE2_AS:
	{
		m_iAnimCount = 0;

		m_pModelCom->Set_Animation(FSM_EFFIGY_CORE_EXPOSURE2_AS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_CORE_EXPOSURE2_AS) * 0.75f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_PILLAR:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_PILLAR);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_PILLAR) * 0.75f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_CORE_EXPOSURE2_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSURE2_LOOP:
	{
		pEffigy_Fire->Set_IsCoreExposure(TRUE);
		
		m_pModelCom->Set_Animation(FSM_EFFIGY_CORE_EXPOSURE2_LOOP);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_CORE_EXPOSURE2_LOOP) * 0.95f)
			m_iAnimCount++;

		if (m_iAnimCount >= 30)
		{
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_CORE_EXPOSURE2_AS);
			pEffigy_Fire->Set_IsCoreExposure(FALSE);
		}
	}
	break;
	case Client::FSM_EFFIGY_SHAKE_SHAKE:
	{
		pEffigy_Fire->Set_IsCoreExposure(FALSE);
		m_bRenderEnable = TRUE;

		m_pModelCom->Set_Animation(FSM_EFFIGY_SHAKE_SHAKE);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_SHAKE_SHAKE) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_BS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_ROLLING_BS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_ROLLING_BS) * 0.95f)
		{
			CAssetFX::ASFX_DESC tAsfxDesc = { };
			tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_RELATIVE;
			tAsfxDesc.pTransform = m_pAuraTransform;

			m_pGameInstance->Play_AssetFX(L"EffigyFireAura", L"EffigyFireAura", &tAsfxDesc);	/* Fire Aura */
			m_pGameInstance->Play_AssetFX(L"EffigyFireTrail", L"EffigyFireTrail", &tAsfxDesc);	/* Fire Trail */

			/* Particle */
			wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_LLarge");
			wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_2_LLarge");
			wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_3_LLarge");
			wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_4_LLarge");

			m_ParticleTags.push_back(strParticle_Large1);
			m_ParticleTags.push_back(strParticle_Large2);
			m_ParticleTags.push_back(strParticle_Large3);
			m_ParticleTags.push_back(strParticle_Large4);

			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_LLarge", strParticle_Large1,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2_LLarge", strParticle_Large2,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3_LLarge", strParticle_Large3,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4_LLarge", strParticle_Large4,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);

			/* Change State */
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_ATTACK_ROLLING_AIMING);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_AIMING:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_ROLLING_AIMING);
		m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_ROLLING_LOOP);
		m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_ROLLING_AS:
	{
		for (auto& iter : m_ParticleTags)
			m_pGameInstance->Stop_Particle(iter, 3.f);
		m_ParticleTags.clear();

		m_pGameInstance->Stop_AssetFX(L"EffigyFireAura");	/* Fire Auta */
		m_pGameInstance->Stop_AssetFX(L"EffigyFireTrail", 3.f);	/* Fire Trail */

		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_ROLLING_AS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_ROLLING_AS) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_TRANSFORM_1:
	{
		if (m_bIsFirstFrame == TRUE)
		{
			m_bIsFirstFrame = FALSE;
			m_bRenderEnable = TRUE;

			CFX_EffigyFireTrsCow::TRSCOW_DESC tTrsCowDesc = { };
			tTrsCowDesc.ePosType	= CAssetFX::ASFX_DESC::POS_FIX;
			tTrsCowDesc.pTransform	= m_pAuraTransform;
			tTrsCowDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrix("Bone001");

			m_pGameInstance->Play_AssetFX(L"EffigyFireTrsCow", Get_ParticleTag(L"EffigyFireTrsCow"), &tTrsCowDesc);
		}

		m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_TRANSFORM_1);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_1) * 0.65f)
			m_bRenderEnable = FALSE;
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_1) * 0.95f)
		{
			m_bIsFirstFrame = TRUE;
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_THINK);
		}

		_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		vCurrentTranslation.m128_f32[1] = 0.f;
		Set_EtcTranslation(vCurrentTranslation);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_TRANSFORM_2:
	{
		m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_TRANSFORM_2);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bTrsFishEff[0] == FALSE && fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_2) * 0.05f)
		{
			CFX_EffigyFireTrsFish::TRSFISH_DESC tTrsFishDesc = { };
			tTrsFishDesc.ePosType		= CAssetFX::ASFX_DESC::POS_FIX;
			tTrsFishDesc.pTransform		= m_pAuraTransform;
			tTrsFishDesc.pBoneMatrix	= m_pModelCom->Get_CombinedMatrix("Fx_HeadNode");

			m_pGameInstance->Play_AssetFX(L"EffigyFireTrsFish", Get_ParticleTag(L"EffigyFireTrsFish"), &tTrsFishDesc);

			m_bTrsFishEff[0] = TRUE;

			/* Particle */
			wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_LLarge");
			wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_2_LLarge");
			wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_3_LLarge");
			wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_4_LLarge");

			m_ParticleTags.push_back(strParticle_Large1);
			m_ParticleTags.push_back(strParticle_Large2);
			m_ParticleTags.push_back(strParticle_Large3);
			m_ParticleTags.push_back(strParticle_Large4);

			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_LLarge", strParticle_Large1,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2_LLarge", strParticle_Large2,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3_LLarge", strParticle_Large3,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4_LLarge", strParticle_Large4,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		}

		if (m_bTrsFishEff[1] == FALSE && fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_2) * 0.42f)
		{
			CFX_EffigyFireTrsFish::TRSFISH_DESC tTrsFishDesc = { };
			tTrsFishDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tTrsFishDesc.pTransform = m_pAuraTransform;
			tTrsFishDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrix("Fx_HeadNode");

			m_pGameInstance->Play_AssetFX(L"EffigyFireTrsFish", Get_ParticleTag(L"EffigyFireTrsFish"), &tTrsFishDesc);

			m_bTrsFishEff[1] = TRUE;

			for (auto& iter : m_ParticleTags)
				m_pGameInstance->Stop_Particle(iter, 4.f);
			m_ParticleTags.clear();
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_2) * 0.05f)
		{
			m_bIsFirstFrame = FALSE;
			m_bRenderEnable = TRUE;
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			vCurrentTranslation.m128_f32[1] = 0.f;
			Set_EtcTranslation(vCurrentTranslation);
		}
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_2) * 0.46f)
		{
			m_bRenderEnable = FALSE;
		}
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_2) * 0.95f)
		{
			m_bTrsFishEff[0] = FALSE;
			m_bTrsFishEff[1] = FALSE;

			m_bIsFirstFrame = TRUE;
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_THINK);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_TRANSFORM_3:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_TRANSFORM_3);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bTrsBombEff == FALSE && fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_3) * 0.25f)
		{
			CFX_EffigyFireTrsBomb::TRSBOMB_DESC tTrsBombDesc = { };
			tTrsBombDesc.ePosType		= CAssetFX::ASFX_DESC::POS_FIX;
			tTrsBombDesc.pTransform		= m_pAuraTransform;
			tTrsBombDesc.pBoneMatrix	= m_pModelCom->Get_CombinedMatrix("Fx_HeadNode");

			m_pGameInstance->Play_AssetFX(L"EffigyFireTrsBomb", Get_ParticleTag(L"EffigyFireTrsBomb"), &tTrsBombDesc);

			m_bTrsBombEff = TRUE;
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_3) * 0.001f)
		{
			m_bIsFirstFrame = FALSE;
			m_bRenderEnable = TRUE;
		}
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_3) * 0.805f)
			m_bRenderEnable = FALSE;
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_TRANSFORM_3) * 0.99f)
		{
			m_bTrsBombEff = FALSE;
			m_bIsFirstFrame = TRUE;
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_THINK);
		}
	}
	break;
	case Client::FSM_EFFIGY_CORE_EXPOSE:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_CORE_EXPOSE);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_CORE_EXPOSE) * 0.95f)
			m_iAnimCount++;
		
		if(m_iAnimCount >= 2)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_PUNCH:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_PUNCH);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (m_bPunchTrail == FALSE && fRate > 90.f)
		{
			/* Particle */
			wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_LLarge_Eruption");
			wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_2_LLarge_Eruption");
			wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_3_LLarge_Eruption");
			wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_4_LLarge_Eruption");

			m_ParticleTags.push_back(strParticle_Large1);
			m_ParticleTags.push_back(strParticle_Large2);
			m_ParticleTags.push_back(strParticle_Large3);
			m_ParticleTags.push_back(strParticle_Large4);

			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_LLarge_Eruption", strParticle_Large1,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2_LLarge_Eruption", strParticle_Large2,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3_LLarge_Eruption", strParticle_Large3,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4_LLarge_Eruption", strParticle_Large4,
				PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);

			m_bPunchTrail = TRUE;
		}


		if (fRate > 160.f)
		{
			for (auto& iter : m_ParticleTags)
				m_pGameInstance->Stop_Particle(iter, 4.f);
			m_ParticleTags.clear();
		}

		if (m_bPunchEff == FALSE && fRate > 136.f)
		{
			CAssetFX::ASFX_DESC tAsfxDesc = { };
			tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tAsfxDesc.vInitPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 0.1f, 0.f, 0.f };

			m_pGameInstance->Play_AssetFX(L"EffigyFireImpact", Get_ParticleTag(L"EffigyFireImpact"), &tAsfxDesc);

			m_bPunchEff = TRUE;
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_PUNCH) * 0.95f)
		{
			m_bPunchEff = FALSE;
			m_bPunchTrail = FALSE;

			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SNAKE_BS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_SNAKE_BS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_SNAKE_BS) * 0.95f)
		{
			m_iFireBallStep = 0;
			m_iFireBallIndex = 0;
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_ATTACK_SNAKE_LOOP);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SNAKE_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_SNAKE_LOOP);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (24.f + m_iFireBallStep * 34.f + m_iFireBallIndex * 2.f < fRate)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

			CAssetFX::ASFX_DESC tAsfxDesc = { };
			tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tAsfxDesc.vInitPosition =
				XMVector4Transform(m_pParentTransformCom->Get_State(CTransform::STATE_POSITION), XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Fx_HeadNode")));
			tAsfxDesc.pTransform = pPlayerTransform;

			m_pGameInstance->Play_AssetFX(L"EffigyFireProjectile", Get_ParticleTag(L"EffigyFireProjectile"), &tAsfxDesc);

			++m_iFireBallIndex;

			if (m_iFireBallIndex > 2)
			{
				++m_iFireBallStep;
				m_iFireBallIndex = 0;
			}
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_SNAKE_LOOP) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_ATTACK_SNAKE_AS);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SNAKE_AS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_SNAKE_AS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_SNAKE_AS) * 0.95f)
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SELF_EXPLOSION_BS:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_BS);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_BS) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_LOOP);
			CBounding_SPHERE::BOUNDING_SPHERE_DESC		WeaponColliderDesc = {};
			WeaponColliderDesc.fRadius = 10.f;
			m_pWeaponColliderCom[EFFIGY_WEAPON_BODY]->Update_Desc(&WeaponColliderDesc);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SELF_EXPLOSION_LOOP:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_LOOP);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_LOOP) * 0.95f)
		{
			m_iAnimCount++;
			cout << "Explosion Count : " << m_iAnimCount << endl;
		}

		if (m_iAnimCount > 11)
		{
			m_iAnimCount = 0;
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE);
		}
	}
	break;
	case Client::FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE:
	{
		m_pModelCom->Set_Animation(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE) * 0.95f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_STANDBY_LOOP);
			CBounding_SPHERE::BOUNDING_SPHERE_DESC		WeaponColliderDesc = {};
			WeaponColliderDesc.fRadius = 1.8f;
			WeaponColliderDesc.vCenter = _float3(0.f, WeaponColliderDesc.fRadius, 0.f);
			m_pWeaponColliderCom[EFFIGY_WEAPON_BODY]->Update_Desc(&WeaponColliderDesc);
		}
	}
	break;
	case Client::FSM_EFFIGY_STANDBY_ALERT:
	{
		static_cast<CEffigy_Fire*>(m_pParent)->Set_Alert(TRUE);

		if (m_bAlertParticle == FALSE)
		{
			/* Particle */
			// wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_Orbit");
			// wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_1_Orbit");
			// wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_1_Orbit");
			// wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_1_Orbit");
			// 
			// m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Orbit", strParticle_Large1,
			// 	PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			// m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Orbit", strParticle_Large2,
			// 	PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			// m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Orbit", strParticle_Large3,
			// 	PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			// m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Orbit", strParticle_Large4,
			// 	PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
			// 
			// m_pGameInstance->Stop_Particle(strParticle_Large1, 2.f);
			// m_pGameInstance->Stop_Particle(strParticle_Large2, 2.f);
			// m_pGameInstance->Stop_Particle(strParticle_Large3, 2.f);
			// m_pGameInstance->Stop_Particle(strParticle_Large4, 2.f);

			m_bAlertParticle = TRUE;
		}

		m_pModelCom->Set_Animation(FSM_EFFIGY_STANDBY_ALERT);
		fRate = m_pModelCom->Play_EffigyRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(FSM_EFFIGY_STANDBY_ALERT) * 0.95f)
		{
			m_bAlertParticle = FALSE;
			m_pParentFSMCom->Change_ForceState(FSM_EFFIGY_THINK);
		}
	}
	break;
	case Client::FSM_EFFIGY_HIT:
		break;
	case Client::FSM_EFFIGY_TRASH:
		break;
	case Client::FSM_EFFIGY_TRASH2:
		break;
	case Client::FSM_EFFIGY_THINK:
		break;
	case Client::FSM_EFFIGY_END:
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

HRESULT CBody_Effigy_Fire::Bind_ShaderResources_Trail()
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

HRESULT CBody_Effigy_Fire::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Transform_Aura */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Aura"), (CComponent**)&m_pAuraTransform)))
		return E_FAIL;

	/* For.Com_Transform_Aura */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_MagicCircle"), (CComponent**)&m_pMagicCircleTransform)))
		return E_FAIL;

	/* For.Com_Transform_Particle */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Particle"), (CComponent**)&m_pParticleTransform)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.9f, 0.9f, 0.9f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y + 0.4f, 0.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f , 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);


	/* For.Com_Collider_SPHERE */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC		WeaponColliderDesc = {};
	WeaponColliderDesc.fRadius = 1.8f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_WEAPON_BODY"), (CComponent**)&m_pWeaponColliderCom[EFFIGY_WEAPON_BODY], &WeaponColliderDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iWeaponColliderID[EFFIGY_WEAPON_BODY], m_pWeaponColliderCom[EFFIGY_WEAPON_BODY], this);


	/* For.Com_Collider_OBB */
	BoundingDesc.vExtents = _float3(3.5f, 1.5f, 2.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_WEAPON_CLAP"), (CComponent**)&m_pWeaponColliderCom[EFFIGY_WEAPON_CLAP], &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_WEAPON, &m_iWeaponColliderID[EFFIGY_WEAPON_CLAP], m_pWeaponColliderCom[EFFIGY_WEAPON_CLAP], this);

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Spawn_Fire_Cube()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	// 내 정보들 구해놓기
	_vector vSelfPos = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pParentTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pParentTransformCom->Get_State(CTransform::STATE_UP));
	_vector vSelfRight = XMVector3Normalize(m_pParentTransformCom->Get_State(CTransform::STATE_RIGHT));

	vSelfPos.m128_f32[1] += 3.5f;

	// 플레이어를 향하는 방향벡터
	_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	CSkillObj::SKILLOBJ_DESC desc = {};
	desc.eSkillObjID = SKILLOBJ_FIRE_CUBE;
	desc.fLifeTime = 10.f;
	desc.vScale = { 0.01f, 0.01f, 0.01f, 0.f };
	_vector vOriginalLook = vSelfLook;

	vSelfLook *= 3.8f;
	vSelfUp *= 3.8f;
	vSelfRight *= 3.6f;

	Fire_Cube_Attack_Pos.push_back(vSelfPos + vSelfUp);
	Fire_Cube_Attack_Pos.push_back(vSelfPos + (vSelfUp / 2 + vSelfRight / 2));
	Fire_Cube_Attack_Pos.push_back(vSelfPos + (vSelfUp / 2 + -vSelfRight / 2));
	Fire_Cube_Attack_Pos.push_back(vSelfPos + vSelfRight);
	Fire_Cube_Attack_Pos.push_back(vSelfPos - vSelfRight);
	Fire_Cube_Attack_Pos.push_back(vSelfPos + (-vSelfUp / 2 + vSelfRight / 2));
	Fire_Cube_Attack_Pos.push_back(vSelfPos + (-vSelfUp / 2 + -vSelfRight / 2));
	Fire_Cube_Attack_Pos.push_back(vSelfPos - vSelfUp);

	desc.vSpawnPos = Fire_Cube_Attack_Pos[m_iAttack_Cube_Index];

	if(m_iAttack_Cube_Index == 0 || m_iAttack_Cube_Index == 7)
		desc.vDir = vSelfLook / 3.8f;
	else
		desc.vDir = XMVector3Normalize(desc.vSpawnPos - vSelfPos);

	if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_FireCube", L"Layer_Skill", &desc)))
		return E_FAIL;

	switch (m_iAttack_Cube_Index)
	{
	case 0:
		Fire_Cube_Mesh_Index.push_back(2);
		break;
	case 1 :
		Fire_Cube_Mesh_Index.push_back(3);
		break;
	case 2:
		Fire_Cube_Mesh_Index.push_back(7);
		break;
	case 3:
		Fire_Cube_Mesh_Index.push_back(5);
		break;
	case 4:
		Fire_Cube_Mesh_Index.push_back(8);
		break;
	case 5:
		Fire_Cube_Mesh_Index.push_back(4);
		break;
	case 6:
		Fire_Cube_Mesh_Index.push_back(6);
		break;
	case 7:
		Fire_Cube_Mesh_Index.push_back(1);
		break;
	}

	m_iAttack_Cube_Index++;

	Fire_Cube_Attack_Pos.clear();

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Invalidate_Particle()
{
	FSM_EFFIGY_STATE eState = static_cast<FSM_EFFIGY_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (m_bRenderEnable == FALSE ||
		eState == FSM_EFFIGY_ATTACK_TRANSFORM_1 || 
		eState == FSM_EFFIGY_ATTACK_TRANSFORM_2 || 
		eState == FSM_EFFIGY_ATTACK_TRANSFORM_3)
	{
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_1", TRUE);
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_2", TRUE);
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_3", TRUE);
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_4", TRUE);
	}
	else
	{
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_1", FALSE);
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_2", FALSE);
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_3", FALSE);
		m_pGameInstance->Pause_Particle(L"Effigy_PaperBurn_4", FALSE);
	}

	if ((m_bParticleMove == FALSE) && (m_bRenderEnable == TRUE) && (eState == FSM_EFFIGY_MOVE_RUSHMOVE_LOOP))
	{
		/* Particle */
		wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_Small");
		wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_2_Small");
		wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_3_Small");
		wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_4_Small");

		m_ParticleMoveTags.push_back(strParticle_Large1);
		m_ParticleMoveTags.push_back(strParticle_Large2);
		m_ParticleMoveTags.push_back(strParticle_Large3);
		m_ParticleMoveTags.push_back(strParticle_Large4);

		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Small", strParticle_Large1,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2_Small", strParticle_Large2,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3_Small", strParticle_Large3,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4_Small", strParticle_Large4,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);

		m_bParticleMove = TRUE;
	}
	else if ((m_bParticleMove == TRUE) && (eState != FSM_EFFIGY_MOVE_RUSHMOVE_LOOP))
	{
		for (auto& iter : m_ParticleMoveTags)
			m_pGameInstance->Stop_Particle(iter, 3.f);
		m_ParticleMoveTags.clear();

		m_bParticleMove = FALSE;
	}

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Effigy_Fire::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	CEffigy_Fire* pEffigy_Fire = static_cast<CEffigy_Fire*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Monster"));

	if (pEffigy_Fire->Get_IsFireShieldActive() == TRUE)
	{
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1", L"Effigy_PaperBurn_1", PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2", L"Effigy_PaperBurn_2", PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3", L"Effigy_PaperBurn_3", PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4", L"Effigy_PaperBurn_4", PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pModelCom->Get_CombinedMatrix("Bone001"), m_pParticleTransform);
	}

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	_matrix BodyWeaponColliderMatrix =
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("WetElectricHitBox")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMLoadFloat4x4(&m_CombinedWorldMatrix);

	m_pAuraTransform->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
	m_pMagicCircleTransform->Set_WorldMatrix(BodyWeaponColliderMatrix);
	m_pParticleTransform->Set_WorldFloat4x4(m_CombinedWorldMatrix);

	if (FAILED(CBody_Effigy_Fire::Invalidate_Particle()))
		return E_FAIL;

	FSM_EFFIGY_STATE eState = static_cast<FSM_EFFIGY_STATE>(m_pParentFSMCom->Get_CurrentState());
	if ((eState >= FSM_EFFIGY_ATTACK_TRANSFORM_1 && eState <= FSM_EFFIGY_ATTACK_TRANSFORM_3) || eState == FSM_EFFIGY_ATTACK_SELF_EXPLOSION_EXPLODE)
	{
		_float fScale = 1.f;
		if (eState == FSM_EFFIGY_ATTACK_TRANSFORM_3)
			fScale = 1.5f;

		BodyWeaponColliderMatrix.r[0] = XMVectorSet(fScale, 0.f, 0.f, 0.f);
		BodyWeaponColliderMatrix.r[0] = XMVectorSet(0.f, fScale, 0.f, 0.f);
		BodyWeaponColliderMatrix.r[0] = XMVectorSet(0.f, 0.f, fScale, 0.f);
	}

	m_pWeaponColliderCom[EFFIGY_WEAPON_BODY]->Update(BodyWeaponColliderMatrix);

	_matrix ColliderMatrix = m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
	ColliderMatrix.r[3] += XMVector3Normalize(m_pParentTransformCom->Get_State(CTransform::STATE_LOOK)) * 1.5f;
	m_pWeaponColliderCom[EFFIGY_WEAPON_CLAP]->Update(ColliderMatrix);

	return S_OK;
}

HRESULT CBody_Effigy_Fire::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	m_bPetrification = FALSE;

	if (isHostElement(ELEMENT_FIRE) == FALSE)
	{
		_bool bResetElement = FALSE;
		if (m_eReceiveElement == ELEMENT_END)
			bResetElement = TRUE;
		CPart_Body::Set_HostElement(ELEMENT_FIRE, TRUE);
		if (bResetElement == TRUE)
			m_eReceiveElement = ELEMENT_END;
	}

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Render()
{
	CEffigy_Fire* pEffigy_Fire = static_cast<CEffigy_Fire*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Monster"));

	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
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
				if (pEffigy_Fire->Get_IsFireShieldActive() == FALSE)
				{
					if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
						return E_FAIL;
				}
				else
				{
					if (FAILED(m_pShaderCom->Begin((siMesh == 0) ? VTXANIMMESH_DEFAULT : VTXANIMMESH_EFFIGY_FRIE)))
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


		// siMesh가 Fire_Cube_Mesh_Index 안에 있는지 확인
		_bool bIsMeshInIndex = std::find(Fire_Cube_Mesh_Index.begin(), Fire_Cube_Mesh_Index.end(), siMesh) != Fire_Cube_Mesh_Index.end();

		// siMesh가 Fire_Cube_Mesh_Index에 없을 때에만 Render
		if (!bIsMeshInIndex)
		{
			if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
				return E_FAIL;
		}
	}

	m_pWeaponColliderCom[EFFIGY_WEAPON_BODY]->Render();

	return S_OK;
}

HRESULT CBody_Effigy_Fire::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
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

		// siMesh가 Fire_Cube_Mesh_Index 안에 있는지 확인
		_bool bIsMeshInIndex = std::find(Fire_Cube_Mesh_Index.begin(), Fire_Cube_Mesh_Index.end(), siMesh) != Fire_Cube_Mesh_Index.end();

		if (!bIsMeshInIndex)
		{
			if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CBody_Effigy_Fire::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Effigy_Fire COLL_MONSTER_BODY" << endl;
	else
		cout << "Release Effigy_Fire COLL_MONSTER_BODY" << endl;

	hr = m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iWeaponColliderID[0]);

	if (FAILED(hr))
		cout << "Failed Effigy_Fire COLL_MONSTER_WEAPON" << endl;
	else
		cout << "Release Effigy_Fire COLL_MONSTER_WEAPON" << endl;

	hr = m_pGameInstance->Release_Collider_Object(COLL_MONSTER_WEAPON, m_iWeaponColliderID[1]);

	if (FAILED(hr))
		cout << "Failed Effigy_Fire COLL_MONSTER_WEAPON" << endl;
	else
		cout << "Release Effigy_Fire COLL_MONSTER_WEAPON" << endl;
}

CBody_Effigy_Fire* CBody_Effigy_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Effigy_Fire* pInstance = new CBody_Effigy_Fire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Effigy_Fire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Effigy_Fire::Clone(void* pArg)
{
	CBody_Effigy_Fire* pInstance = new CBody_Effigy_Fire(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Effigy_Fire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Effigy_Fire::Free()
{
	__super::Free();

	Safe_Release(m_pAuraTransform);
	Safe_Release(m_pMagicCircleTransform);
	Safe_Release(m_pParticleTransform);

	Safe_Release(m_pWeaponColliderCom[0]);
	Safe_Release(m_pWeaponColliderCom[1]);
}