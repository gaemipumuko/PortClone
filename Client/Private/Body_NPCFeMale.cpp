#include "stdafx.h"
#include "Body_NPCFemale.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

#define	HAIR_MESH_INDEX 1

CBody_NPCFeMale::CBody_NPCFeMale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_NPCFeMale::CBody_NPCFeMale(const CBody_NPCFeMale& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_NPCFeMale::Invalidate_Animation(_float fTimeDelta)
{
	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	switch (eState)
	{
	case FSM_NPC_IDLE:
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(19);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	break;

	case FSM_NPC_CLAP:
		m_pModelCom->Set_Animation(8);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_WALK:
		m_pModelCom->Set_Animation(42);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_TALK1:
		m_pModelCom->Set_Animation(36);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_TALK2:
		m_pModelCom->Set_Animation(37);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_TALK3:
		m_pModelCom->Set_Animation(38);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
		break;

	case FSM_NPC_AKIMBO_START:
	{
		m_pModelCom->Set_Animation(1);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(1))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_AKIMBO_LOOP);
		}
	}
	break;

	case FSM_NPC_AKIMBO_LOOP:
	{
		m_pModelCom->Set_Animation(2);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(2))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_AKIMBO_END);
		}
	}
	break;

	case FSM_NPC_AKIMBO_END:
	{
		m_pModelCom->Set_Animation(0);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta * 0.8f, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(0))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_IDLE);
		}
	}
	break;

	case FSM_NPC_HOLDARM_START:
	{
		m_pModelCom->Set_Animation(17);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(17))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_HOLDARM_LOOP);
		}
	}
	break;

	case FSM_NPC_HOLDARM_LOOP:
	{
		m_pModelCom->Set_Animation(18);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(18))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_HOLDARM_END);
		}
	}
	break;

	case FSM_NPC_HOLDARM_END:
	{
		m_pModelCom->Set_Animation(16);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(16))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_IDLE);
		}
	}
	break;

	case FSM_NPC_SITHAND_START:
	{
		m_pModelCom->Set_ConvertSpeed(2.f);
		m_pModelCom->Set_Animation(31);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(31))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_SITHAND_LOOP);
		}
	}
	break;

	case FSM_NPC_SITHAND_LOOP:
	{
		m_pModelCom->Set_ConvertSpeed(5.f);
		m_pModelCom->Set_Animation(32);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(32))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_SITHAND_END);
		}
	}
	break;

	case FSM_NPC_SITHAND_END:
	{
		m_pModelCom->Set_Animation(30);
		_float fRate = m_pModelCom->Play_PlayerRootAnimation(FALSE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);

		if (fRate >= m_pModelCom->Get_AnimationDuration(30))
		{
			m_pParentFSMCom->Change_ForceState(FSM_NPC_SITHAND_START);
		}
	}
	break;
	}

	return S_OK;
}

HRESULT CBody_NPCFeMale::Add_Component(void* pArg)
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

HRESULT CBody_NPCFeMale::Set_HairColor()
{
	switch (m_eCreatureID)
	{
	case NPC_JILLIANA:
		m_eHairColor = HAIR_BROWN;
		m_iHairMesh = 2;
		break;
	case NPC_LYNN:
		m_eHairColor = HAIR_BLACK;
		m_iHairMesh = 3;
		break;
	case NPC_MARJORIE:
		m_eHairColor = HAIR_LIGHTBROWN;
		m_iHairMesh = 2;
		break;
	case NPC_MARGARET:
		m_eHairColor = HAIR_YELLOWBROWN;
		m_iHairMesh = 3;
		break;
	case NPC_SARA:
		m_eHairColor = HAIR_LIGHTBROWN;
		m_iHairMesh = 3;
		break;
	case NPC_BLANCHE:
		m_eHairColor = HAIR_BROWN;
		m_iHairMesh = 3;
		break;
	case NPC_BEATRICE:
		m_eHairColor = HAIR_YELLOWBROWN;
		m_iHairMesh = 3;
		break;
	case NPC_EURY:
		m_eHairColor = HAIR_LIGHTBROWN;
		m_iHairMesh = 2;
		break;
	case NPC_DONNA:
		m_eHairColor = HAIR_BROWN;
		m_iHairMesh = 2;
		break;
	case NPC_NORMA:
		m_eHairColor = HAIR_BROWN;
		m_iHairMesh = 2;
		break;
	case NPC_ANNA:
		m_eHairColor = HAIR_BROWN;
		m_iHairMesh = 3;
		break;
	case NPC_GLORY:
		m_eHairColor = HAIR_LIGHTBROWN;
		m_iHairMesh = 3;
		break;
	case NPC_SUSIE:
		m_eHairColor = HAIR_YELLOWBROWN;
		m_iHairMesh = 2;
		break;
	case NPC_MICHELLE:
		m_eHairColor = HAIR_YELLOWBROWN;
		m_iHairMesh = 3;
		break;
	}
	return S_OK;
}

HRESULT CBody_NPCFeMale::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_NPCFeMale::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(CBody_NPCFeMale::Set_HairColor()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 1.1f,1.1f,1.1f,1.f });

	return S_OK;
}

HRESULT CBody_NPCFeMale::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_NPCFeMale::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_NPCFeMale::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_NPCFeMale::Render()
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
				if (siMesh == m_iHairMesh)
				{
					switch (m_eHairColor)
					{
					case HAIR_BLACK:
						if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_BLACKHAIR)))
							return E_FAIL;
						break;
					case HAIR_BROWN:
						if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_BROWNHAIR)))
							return E_FAIL;
						break;
					case HAIR_LIGHTBROWN:
						if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_LIGHTBROWNHAIR)))
							return E_FAIL;
						break;
					case HAIR_YELLOWBROWN:
						if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_YELLOWBROWNHAIR)))
							return E_FAIL;
						break;
					}
				}
				else
				{
					if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP)))
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

void CBody_NPCFeMale::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_NPC_BODY, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed NPCFeMale Body COLL_NPC_BODY" << endl;
	else
		cout << "Release NPCFeMale Body COLL_NPC_BODY" << endl;
}

CBody_NPCFeMale* CBody_NPCFeMale::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_NPCFeMale* pInstance = new CBody_NPCFeMale(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_NPCFeMale"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_NPCFeMale::Clone(void* pArg)
{
	CBody_NPCFeMale* pInstance = new CBody_NPCFeMale(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_NPCFeMale"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_NPCFeMale::Free()
{
	__super::Free();
}