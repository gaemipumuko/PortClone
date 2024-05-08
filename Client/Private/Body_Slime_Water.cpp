#include "stdafx.h"
#include "Body_Slime_Water.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

CBody_Slime_Water::CBody_Slime_Water(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Slime_Water::CBody_Slime_Water(const CBody_Slime_Water& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Slime_Water::Invalidate_Animation(_float fTimeDelta)
{
	FSM_SLIME_STATE eState = static_cast<FSM_SLIME_STATE>(m_pParentFSMCom->Get_CurrentState());
	


	_float fRate = 0.f;
	_float fDuration = 0.f;
	_float fTime = fTimeDelta;
	if (m_bPetrification == TRUE)
		fTime = 0.f;

	switch (eState)
	{
	case FSM_SLIME_IDLE:
		m_pModelCom->Set_Animation(9);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTime * 0.7f, m_vRootTranslation);

		if (fRate == 0.f)
			m_bSoundPlayed = false;

		fDuration = m_pModelCom->Get_AnimationCurrentDuration();
		if (fDuration == 0.f)
			m_bSoundPlayed = false;

		if (fDuration >= m_pModelCom->Get_AnimationDuration(9) * 0.5f)
		{
			if (m_bSoundPlayed == false)
			{
				m_pGameInstance->PlaySoundW(L"Slime_Move.mp3");
				m_bSoundPlayed = true;
			}
		}

	break;

	case FSM_SLIME_WALK:
		m_pModelCom->Set_Animation(11);
		fRate = m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= m_pModelCom->Get_AnimationDuration(11))
		{
			m_pModelCom->Set_AnimationCurrentDuration(m_pModelCom->Get_AnimationDuration(11) * 0.f);
		}

		fDuration = m_pModelCom->Get_AnimationCurrentDuration();
		if (fDuration == 0.f)
			m_bSoundPlayed = false;

		if (fDuration >= m_pModelCom->Get_AnimationDuration(11) * 0.5f)
		{
			if (m_bSoundPlayed == false)
			{
				m_pGameInstance->PlaySoundW(L"Slime_Move.mp3");
				m_bSoundPlayed = true;
			}
		}
	
	break;

	case FSM_SLIME_TURN:
		m_pModelCom->Set_Animation(10);
		m_pModelCom->Play_RootAnimation(TRUE, m_pParentTransformCom, fTime, m_vRootTranslation);

		fDuration = m_pModelCom->Get_AnimationCurrentDuration();
		if (fDuration == 0.f)
			m_bSoundPlayed = false;

		if (fDuration >= m_pModelCom->Get_AnimationDuration(10) * 0.5f)
		{
			if (m_bSoundPlayed == false)
			{
				m_pGameInstance->PlaySoundW(L"Slime_Move.mp3");
				m_bSoundPlayed = true;
			}
		}

	break;

	case FSM_SLIME_ATTACK:
	{
		m_vPrevRootTranslation = m_vRootTranslation;
		m_pModelCom->Set_Animation(8);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		Set_ControllerTranslation(vCurrentTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(8)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_SLIME_TURN);
		}

		fDuration = m_pModelCom->Get_AnimationCurrentDuration();
		if (fDuration == 0.f)
			m_bSoundPlayed = false;

		if (fDuration >= m_pModelCom->Get_AnimationDuration(8) * 0.48f)
		{
			if (m_bSoundPlayed == false)
			{
				m_pGameInstance->PlaySoundW(L"Slime_Attack.wav");
				m_bSoundPlayed = true;
			}
		}
	}
	break;

	case FSM_SLIME_HIT_L:
		m_pModelCom->Set_Animation(3);

		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(3)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_SLIME_TURN);
		}
		break;

	case FSM_SLIME_HIT_H:
		m_pModelCom->Set_Animation(2);

		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime, m_vRootTranslation);

		if (fRate >= (m_pModelCom->Get_AnimationDuration(2)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_SLIME_TURN);
		}
		break;

	case FSM_SLIME_HIT_THROW_START:
	{
		m_vPrevRootTranslation = m_vRootTranslation;
		m_pModelCom->Set_Animation(6);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		vCurrentTranslation.m128_f32[1] = 0.f;
		Set_ControllerTranslation(vCurrentTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(6) * 0.6f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_SLIME_HIT_THROW);
		}
	}
		
		break;

	case FSM_SLIME_HIT_THROW:
	{
		m_vPrevRootTranslation = m_vRootTranslation;
		m_pModelCom->Set_Animation(4);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		vCurrentTranslation.m128_f32[1] = 0.f;
		Set_ControllerTranslation(vCurrentTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(4) * 0.6f))
		{
			m_pParentFSMCom->Change_ForceState(FSM_SLIME_HIT_THROW_DOWN);
		}
	}
	break;

	case FSM_SLIME_HIT_THROW_DOWN:
	{
		m_vPrevRootTranslation = m_vRootTranslation;
		m_pModelCom->Set_Animation(5);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime * 2.f, m_vRootTranslation);
		_vector vCurrentTranslation = m_vPrevRootTranslation - m_vRootTranslation;
		vCurrentTranslation.m128_f32[1] = 0.f;
		Set_ControllerTranslation(vCurrentTranslation);
		if (fRate >= (m_pModelCom->Get_AnimationDuration(5)))
		{
			m_pParentFSMCom->Change_ForceState(FSM_SLIME_TURN);
		}
	}
		break;

	case FSM_SLIME_DIE1:
		m_pModelCom->Set_Animation(1);
		fRate = m_pModelCom->Play_RootAnimation(FALSE, m_pParentTransformCom, fTime * 1.5f, m_vRootTranslation);

		fDuration = m_pModelCom->Get_AnimationCurrentDuration();
		if (fDuration == 0.f)
			m_bSoundPlayed = false;

		if (fDuration >= m_pModelCom->Get_AnimationDuration(1) * 0.6f)
		{
			if (m_bSoundPlayed == false)
			{
				m_pGameInstance->PlaySoundW(L"SlimeDie.wav");
				m_bSoundPlayed = true;
			}
		}

		if (fRate >= (m_pModelCom->Get_AnimationDuration(1)))
		{
			m_bDead = TRUE;
		}
		m_pParentController->Sleep_Controller();
		break;
	}

	return S_OK;
}

HRESULT CBody_Slime_Water::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.2f, 0.2f, 0.2f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBody_Slime_Water::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Slime_Water::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Slime_Water::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Slime_Water::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Slime_Water::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	if (isHostElement(ELEMENT_WATER) == FALSE)
	{
		_bool bResetElement = FALSE;
		if (m_eReceiveElement == ELEMENT_END)
			bResetElement = TRUE;
		CPart_Body::Set_HostElement(ELEMENT_WATER, TRUE);
		if (bResetElement == TRUE)
			m_eReceiveElement = ELEMENT_END;
	}

	return S_OK;
}

HRESULT CBody_Slime_Water::Render()
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
	}

	return S_OK;
}

void CBody_Slime_Water::Release_ColliderID()
{
	m_pGameInstance->Release_Collider_Object(COLL_MONSTER_BODY, m_iColliderID);
}

CBody_Slime_Water* CBody_Slime_Water::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Slime_Water* pInstance = new CBody_Slime_Water(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Slime_Water"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Slime_Water::Clone(void* pArg)
{
	CBody_Slime_Water* pInstance = new CBody_Slime_Water(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Slime_Water"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Slime_Water::Free()
{
	__super::Free();
}