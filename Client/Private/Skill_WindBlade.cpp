#include "stdafx.h"
#include "Skill_WindBlade.h"

#include "GameInstance.h"

#include "Andrius.h"
#include "Camera.h"

#include "Effect.h"
#include "Player.h"
#include "Part_Body.h"

CSkill_WindBlade::CSkill_WindBlade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_WindBlade::CSkill_WindBlade(const CSkill_WindBlade& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_WindBlade::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WindBlade"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC tDesc = {};
	tDesc.vExtents = _float3(0.5f, 2.f, 0.25f);
	tDesc.vCenter = _float3(0.f, tDesc.vExtents.y, 0.f);
	tDesc.vRadians = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &tDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CSkill_WindBlade::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_WindBlade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_WindBlade::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = static_cast<SKILLOBJ_DESC*>(pArg);

		m_eSkill = pDesc->eSkillObjID;

		m_fLifeTime = pDesc->fLifeTime;
		m_vMoveDir = pDesc->vDir;

		_vector vSpawnPos = pDesc->vSpawnPos;
		vSpawnPos.m128_f32[1] += 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSpawnPos);
		m_pTransformCom->Set_Scale(pDesc->vScale);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CSkill_WindBlade::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_WindBlade::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	m_vSpinUV.x += fTimeDelta / 3;
	//m_vSpinUV.y += fTimeDelta;

	if (m_fLifeTime <= 0.f)
	{
		m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
		m_HitPlayerIndex.clear();
		Set_Destroy(TRUE);
	}

	m_pTransformCom->Look_At_Direction(m_vMoveDir);
	//m_pTransformCom->Go_Straight(fTimeDelta * 0.5f);
	m_pTransformCom->Go_Straight(fTimeDelta * 4.f);

	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());


	_float4x4 ColliderPos = m_WorldMatrix;
	ColliderPos._11 = 1.f;
	ColliderPos._22 = 1.f;
	ColliderPos._33 = 1.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderPos));

	/*if (m_bAttack == FALSE)
	{
		vector<CGameObject*> vecHitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
		if (vecHitPlayer.size() != 0)
		{
			m_bAttack = TRUE;
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();

			_float4 vDir = {};
			XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			vDir.w = 1.f;

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(333, 444), ELEMENT_WIND, FSM_HIT_THROW, 3.f, 1.5f, vDir);
		}
	}*/

	vector<CGameObject*> vecHitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	for (auto pPlayerBody : vecHitPlayer)
	{
		PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
		_bool bContinue = FALSE;
		for (auto iter : m_HitPlayerIndex)
		{
			if (iter == eIndex)
			{
				bContinue = TRUE;
				break;
			}
		}
		
		if (bContinue == TRUE)
			continue;

		m_HitPlayerIndex.push_back(eIndex);

		_float4 vDir = {};
		XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		vDir.w = 1.f;

		pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(333, 444), ELEMENT_WIND, FSM_HIT_THROW, 3.f, 1.5f, vDir);
	}

	return S_OK;
}

HRESULT CSkill_WindBlade::LateTick(_float fTimeDelta)
{
	//m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_WindBlade::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpinRate", &m_vSpinUV, sizeof(_float2))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(VTXMESH_WIND_BLADE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_WindBlade* CSkill_WindBlade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_WindBlade* pInstance = new CSkill_WindBlade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_WindBlade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_WindBlade::Clone(void* pArg)
{
	CSkill_WindBlade* pInstance = new CSkill_WindBlade(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_WindBlade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_WindBlade::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}