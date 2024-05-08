#include "stdafx.h"
#include "Skill_Pillar.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"
#include "CharacterController.h"

CSkill_Pillar::CSkill_Pillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_Pillar::CSkill_Pillar(const CSkill_Pillar& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_Pillar::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pillar"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_SPHERE */
	CBounding_OBB::BOUNDING_OBB_DESC BoundingDesc = {};
	//BoundingDesc.vExtents = _float3(15.f, 20.f, 15.f);
	BoundingDesc.vExtents = _float3(2.5f, 2.f, 2.5f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_SKILL, &m_iColliderID, m_pColliderCom, this);
	

	return S_OK;
}

HRESULT CSkill_Pillar::Bind_ShaderResources()
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
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPillarLineLight", &m_fPillarLineLight, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFadeIn", &m_fFadeIn, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Pillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Pillar::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CSkill_Pillar::Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
		XMStoreFloat3(&m_vDir, pDesc->vDir);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Look_At_Direction(pDesc->vDir);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({ 0.1f, 0.1f, 0.1f, 0.f });
	m_fLifeTime = 30.f;


	_float4 vIdentityPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pController = CCharacterController::Create(3.3f, 0.3f, 0.f, vIdentityPos, m_pTransformCom);

	if (nullptr == m_pController)
		return E_FAIL;

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pController->Set_Position(vPosition);

	m_pGameInstance->PlaySoundW(L"Zhongli_ElementHold", 1.f);

	return S_OK;
}

HRESULT CSkill_Pillar::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_Pillar::Tick(_float fTimeDelta)
{
	if (m_fUp < 0.f)
	{
		m_fUp += fTimeDelta * 20.f;
		if (m_fUp >= -4.f && m_fUp <= -1.0f)
		{
			vector<CGameObject*> vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_SKILL, m_iColliderID, COLL_MONSTER_BODY);
			for (auto& HitMonsters : vecHitMonsters)
			{
				if (m_bAttackStop == TRUE)
					return S_OK;

				_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
				m_pGameInstance->Set_IsCollision(COLL_MONSTER_BODY, iHitID, FALSE);
				m_HitMonsterIDList.push_back(iHitID); // 맞은 몬스터

				static_cast<CPartObject*>(HitMonsters)->Set_Damage(static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_ElementDamage() * 0.5f, ELEMENT_EARTH, FSM_OBJECT_END);

				if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STORMTERROR)
				{
					m_bAttackStop = TRUE;
					return S_OK;
				}
			}
		}
		else if(m_HitMonsterIDList.empty() == FALSE)
		{
			for (auto iter : m_HitMonsterIDList) /* 충돌한 몬스터 초기화 */
			{
				m_pGameInstance->Set_IsCollision(COLL_MONSTER_BODY, iter, TRUE);
			}
			m_HitMonsterIDList.clear();
		}
	}
	else
	{
		m_fUp = 0.f;
	}

	if (m_bDead == FALSE)
	{
		m_fLifeTime -= fTimeDelta;
		if (m_fLifeTime <= 0.f)
		{
			m_fLifeTime = 0.f;
			m_bDead = TRUE;
		}

		m_fWaveDuration += fTimeDelta;
		if (m_fWaveDuration >= 1.5f) /* 지속 공격 */
		{
			CEffect::EFFECT_DESC tDesc = {};
			tDesc.eEffectID = EFFECT_PILLAR_WAVE;
			tDesc.fLifeTime = 2.f;
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vSelfPos.m128_f32[1] += 0.1f;

			tDesc.vSpawnPos = vSelfPos;
			tDesc.vScale = { 5.f, 5.f, 5.f, 0.f };
			tDesc.vDir = XMLoadFloat3(&m_vDir);

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Pillar_Wave", L"Layer_Effect", &tDesc)))
				return E_FAIL;
			
			m_fWaveDuration = 0.f;
			vector<CGameObject*> vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_SKILL, m_iColliderID, COLL_MONSTER_BODY);
			for (auto& HitMonsters : vecHitMonsters)
			{
				static_cast<CPartObject*>(HitMonsters)->Set_Damage(static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_ElementDamage(), ELEMENT_EARTH);
			}
		}
	}
	else
	{
		m_fLifeTime += fTimeDelta;
		m_fFadeIn += fTimeDelta;
		if (m_fLifeTime >= 1.f)
		{
			m_pGameInstance->Release_Collider_Object(COLL_PLAYER_SKILL, m_iColliderID);
			Set_Destroy(TRUE);
		}
	}

	if (m_fPillarLineLight < 1.2f)
		m_fPillarLineLight += fTimeDelta * 0.3f;
	else
		m_fPillarLineLight = -0.2f;
	

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_WorldMatrix._42 += m_fUp;

	/*_float4x4 ColliderMatrix = m_WorldMatrix;
	ColliderMatrix._11 = 1.f;
	ColliderMatrix._22 = 1.f;
	ColliderMatrix._33 = 1.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderMatrix));*/
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	m_pColliderCom->Update(WorldMatrix);

	return S_OK;
}

HRESULT CSkill_Pillar::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CSkill_Pillar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			continue;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(VTXMESH_ZHONGLIPILLAR)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_Pillar* CSkill_Pillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Pillar* pInstance = new CSkill_Pillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Pillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Pillar::Clone(void* pArg)
{
	CSkill_Pillar* pInstance = new CSkill_Pillar(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Pillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Pillar::Free()
{
	__super::Free();

	Safe_Release(m_pController);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}