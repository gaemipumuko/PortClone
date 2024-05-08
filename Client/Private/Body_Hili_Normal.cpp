#include "stdafx.h"
#include "Body_Hili_Normal.h"
#include "Hili_Normal.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

CBody_Hili_Normal::CBody_Hili_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Hili_Normal::CBody_Hili_Normal(const CBody_Hili_Normal& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Hili_Normal::Invalidate_Animation(_float fTimeDelta)
{
	FSM_HILI_STATE eState = static_cast<FSM_HILI_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	_float fTime = fTimeDelta;
	if (m_bPetrification == TRUE && (eState != FSM_HILI_DIE_2 && eState != FSM_HILI_DIE_1 && eState != FSM_HILI_RUNATTACK_END))
	{
		return S_OK;
	}

	switch (eState)
	{
	case FSM_HILI_ALERT:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(19);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(19))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_RUN);
		}
		break;

	case FSM_HILI_IDLE:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(11);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);
	break;

	case FSM_HILI_SIT_START:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(24);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(24))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SIT);
		}
		break;

	case FSM_HILI_SIT:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(26);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(26))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SIT_IDLE);
		}
		break;

	case FSM_HILI_SIT_IDLE:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(25);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(25))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SIT);
		}
		break;

	case FSM_HILI_SLEEP_START:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(28);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(28))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SLEEP_1);
		}
		break;

	case FSM_HILI_SLEEP_1:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(29);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(29))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SLEEP_IDLE);
		}
		break;

	case FSM_HILI_SLEEP_2:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(30);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(30))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SLEEP_1);
		}
		break;

	case FSM_HILI_SLEEP_IDLE:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(31);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(31))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_SLEEP_2);
		}
		break;

	case FSM_HILI_DANCE:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(20);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);
		break;

	case FSM_HILI_WALKFORWARD:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(14);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);
	break;

	case FSM_HILI_WALKBACK:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(13);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);
		break;

	case FSM_HILI_RUN:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(7);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);
	break;

	case FSM_HILI_RUNATTACK_START:
	{
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(9);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		
		if (m_bRunAtkSound1 == FALSE && fRate > 81.875f)
		{
			m_bRunAtkSound1 = TRUE;
			m_pGameInstance->PlaySoundW(L"Hili_3Atk_1.mp3");
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(9) * 0.9f)
		{
			m_bRunAtkSound1 = FALSE;
			m_pParentFSMCom->Change_ForceState(FSM_HILI_RUNATTACK);
		}
	}
	break;

	case FSM_HILI_RUNATTACK:
	{
		m_pModelCom->Set_ConvertSpeed(5);
		m_vPrevRootTranslation = m_vRootTranslation;

		m_pModelCom->Set_Animation(10);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);

		if (m_bRunAtkSound2 == FALSE && fRate > 12.f)
		{
			m_bRunAtkSound2 = TRUE;
			m_pGameInstance->PlaySoundW(L"Hili_3Atk_2.mp3");
		}

		_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		vCurrentTranslation.m128_f32[1] = 0.f;
		Set_ControllerTranslation(vCurrentTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(10) * 0.9f)
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_RUNATTACK_END);
		}
	}
		
	break;

	case FSM_HILI_RUNATTACK_END:
	{
		if (m_bRunAtkSound3 == FALSE)
		{
			m_bRunAtkSound3 = TRUE;
			m_pGameInstance->PlaySoundW(L"Hili_3Atk_3.mp3");
		}

		m_pModelCom->Set_ConvertSpeed(5);
		m_vPrevRootTranslation = m_vRootTranslation;
		m_pModelCom->Set_Animation(8);

		if (m_bPetrification == TRUE)
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(8));
		}
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		
		if(fRate == 0.f)
			m_vPrevRootTranslation = m_vRootTranslation;

		if (fRate < m_pModelCom->Get_AnimationDuration(8) * 0.1f)
		{
			_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
			vCurrentTranslation.m128_f32[1] = 0.f;
			Set_ControllerTranslation(vCurrentTranslation);
		}

		if (fRate >= m_pModelCom->Get_AnimationDuration(8) * 0.9f)
		{
			m_bRunAtkSound1 = FALSE;
			m_bRunAtkSound2 = FALSE;
			m_bRunAtkSound3 = FALSE;

			m_pParentFSMCom->Change_ForceState(FSM_HILI_WALKBACK);
		}
	}
	break;

	case FSM_HILI_HIT_L:
	{
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(2);

		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		
		if (fRate >= (m_pModelCom->Get_AnimationDuration(2)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_WALKBACK);
		}
	}
		break;

	case FSM_HILI_HIT_H:
		m_pModelCom->Set_ConvertSpeed(100);
		m_pModelCom->Set_Animation(1);

		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(1)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_WALKBACK);
		}
		break;

	case FSM_HILI_DIE_1:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(21);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(21)))
		{
			m_bDead = TRUE;
		}
		m_pParentController->Sleep_Controller();
		break;

	case FSM_HILI_DIE_2:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(22);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		/*if (m_bSetDieAnim == FALSE)
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(22) * 0.6f);
			m_bSetDieAnim = TRUE;
		}*/
		if (fRate >= (m_pModelCom->Get_AnimationDuration(22)))
		{
			if (m_fDissolve < 1.f)
			{
				m_fDissolve += 0.005f;
				m_vDissolveColor = _float3(1.f, 0.f, 0.f);
			}
			else
				m_bDead = TRUE;
		}
		m_pParentController->Sleep_Controller();
		break;

	case FSM_HILI_HIT_THROW_START:
	{
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(3);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(3) * 0.4f))
		{
			_float fHp = static_cast<CHili_Normal*>(m_pParent)->Get_Hp();
			if (fHp <= 0.f)
				m_pParentFSMCom->Change_ForceState(FSM_HILI_HIT_THROW_DIE);
			else
				m_pParentFSMCom->Change_ForceState(FSM_HILI_HIT_THROW_STAND);
		}
	}
		break;

	case FSM_HILI_HIT_THROW:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(4);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(4) * 0.9f))
		{
			_float fHp = static_cast<CHili_Normal*>(m_pParent)->Get_Hp();
			if (fHp <= 0.f)
				m_pParentFSMCom->Change_ForceState(FSM_HILI_HIT_THROW_DIE);
			else
				m_pParentFSMCom->Change_ForceState(FSM_HILI_HIT_THROW_STAND);
		}
		break;

	case FSM_HILI_HIT_THROW_STAND:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(6);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(6)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_HILI_WALKBACK);
		}
		break;

	case FSM_HILI_HIT_THROW_DIE:
		m_pModelCom->Set_ConvertSpeed(5);
		m_pModelCom->Set_Animation(5);
		fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(5)))
		{
			m_bDead = TRUE;
		}
		m_pParentController->Sleep_Controller();
		break;
	}

	return S_OK;
}

HRESULT CBody_Hili_Normal::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.6f, 1.f, 0.6f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBody_Hili_Normal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Hili_Normal::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Hili_Normal::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Hili_Normal::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Hili_Normal::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Hili_Normal::Render()
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

void CBody_Hili_Normal::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);
}

CBody_Hili_Normal* CBody_Hili_Normal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Hili_Normal* pInstance = new CBody_Hili_Normal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Hili_Normal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Hili_Normal::Clone(void* pArg)
{
	CBody_Hili_Normal* pInstance = new CBody_Hili_Normal(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Hili_Normal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Hili_Normal::Free()
{
	__super::Free();
}