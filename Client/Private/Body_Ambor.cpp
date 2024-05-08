﻿#include "stdafx.h"
#include "Body_Ambor.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

#define	HAIR_MESH_INDEX 1

CBody_Ambor::CBody_Ambor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Ambor::CBody_Ambor(const CBody_Ambor& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Ambor::Invalidate_Animation(_float fTimeDelta)
{
	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	switch (eState)
	{
	case FSM_NPC_IDLE:
		m_pModelCom->Set_ConvertSpeed(1.f);
		m_pModelCom->Set_Animation(1);
		m_pModelCom->Play_PlayerRootAnimation_NoHeight(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation, 0.875f);
	break;

	case FSM_NPC_RUN:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_WALK:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(2);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_CLAP:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(3);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(3))
		{
			m_pParentFSMCom->Change_State(FSM_NPC_IDLE);
		}
	}
		break;

	case FSM_NPC_TALK1:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(4);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(4))
		{
			m_pParentFSMCom->Change_State(FSM_NPC_IDLE);
		}
	}
	break;

	case FSM_NPC_TALK2:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(5);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(5))
		{
			m_pParentFSMCom->Change_State(FSM_NPC_IDLE);
		}
	}
	break;

	case FSM_NPC_TALK3:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(6);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(6))
		{
			m_pParentFSMCom->Change_State(FSM_NPC_IDLE);
		}
	}
	break;

	case FSM_NPC_TURN_START:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(8);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.5f, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(8) * 0.8f)
		{
			m_pParentFSMCom->Change_State(FSM_NPC_TURN_END);
		}
	}
	break;

	case FSM_NPC_TURN_END:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(7);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 1.5f, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(7))
		{
			m_pParentFSMCom->Change_State(FSM_NPC_IDLE);
		}
	}
	break;
	}

	return S_OK;
}

HRESULT CBody_Ambor::Add_Component(void* pArg)
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

	m_pGameInstance->Add_Collider_Object(COLL_NPC_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBody_Ambor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ambor::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ambor::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ambor::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ambor::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ambor::Render()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bFaceMesh", &(bFaceMesh = (m_iFaceMeshIndex == siMesh)), sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRampIndex",
			&(fRampIndex = (siMesh != HAIR_MESH_INDEX ? (m_eCreatureID * 2.f) : ((m_eCreatureID * 2.f) + 1.f))), sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
		{
			if (m_fDissolve == 0.f)
			{
				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP)))
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

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP_DISSOLVE)))
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

void CBody_Ambor::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_NPC_BODY, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Ambor Body COLL_NPC_BODY" << endl;
	else
		cout << "Release Ambor Body COLL_NPC_BODY" << endl;
}

CBody_Ambor* CBody_Ambor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Ambor* pInstance = new CBody_Ambor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Ambor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Ambor::Clone(void* pArg)
{
	CBody_Ambor* pInstance = new CBody_Ambor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Ambor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Ambor::Free()
{
	__super::Free();
}