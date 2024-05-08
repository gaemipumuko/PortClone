#include "stdafx.h"
#include "Skill_IceCone.h"

#include "GameInstance.h"

#include "Andrius.h"
#include "Camera.h"

#include "Effect.h"
#include "Player.h"
#include "Part_Body.h"

CSkill_IceCone::CSkill_IceCone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_IceCone::CSkill_IceCone(const CSkill_IceCone& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_IceCone::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_IceCone"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	/*CBounding_OBB::BOUNDING_OBB_DESC tBoundingDesc = {};
	tBoundingDesc.vExtents = _float3(0.25f, 1.f, 0.25f);
	tBoundingDesc.vCenter = _float3(0.f, tBoundingDesc.vExtents.y, 0.f);*/

	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 1.5f;
	tBoundingDesc.vCenter = _float3(0.f, tBoundingDesc.fRadius, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CSkill_IceCone::Bind_ShaderResources()
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

HRESULT CSkill_IceCone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_IceCone::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = static_cast<SKILLOBJ_DESC*>(pArg);

		m_eSkill = pDesc->eSkillObjID;
		m_fLifeTime = pDesc->fLifeTime;

		_vector vSpawnPos = pDesc->vSpawnPos;
		vSpawnPos.m128_f32[1] += 8.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSpawnPos);
		m_pTransformCom->Set_Scale(pDesc->vScale);

		// 바람 스킬 범위 이펙트
		CSkillObj::SKILLOBJ_DESC desc = {};
		desc.eSkillObjID = SKILLOBJ_WINDRANGE_ICECONE;
		desc.fLifeTime = 1.1f;
		desc.vSpawnPos = pDesc->vSpawnPos;
		desc.vScale = { 0.04f, 0.04f, 0.04f, 0.f };

		// 스킬 범위 바람 생성
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
			return E_FAIL;
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_IceCone::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_IceCone::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
		m_bAttack = TRUE;
		Set_Destroy(TRUE);
	}

	m_pTransformCom->Go_Down(fTimeDelta * 2.f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	_float4x4 ColliderPos = m_WorldMatrix;
	ColliderPos._11 = 1.f;
	ColliderPos._22 = 1.f;
	ColliderPos._33 = 1.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderPos));


	if (m_eSkill != SKILLOBJ_ABYSS_ICECONE)
	{
		if (!m_bIsCollGround && m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= -1.f)
		{
			m_bIsCollGround = TRUE;

			_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			_float fVolume = 1.f / (fLength * 0.25f);
			if (fVolume > 1.f)
				fVolume = 1.f;
			m_pGameInstance->PlaySoundW(L"Andrius_IceConeBreak.mp3", fVolume);

			_vector vEffectSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vEffectSpawnPos.m128_f32[1] = 0.5f;


			// 얼음 바닥 생성
			CEffect::EFFECT_DESC desc = {};
			desc.eEffectID = EFFECT_ICE_FLOOR_ICECONE;
			desc.fLifeTime = 1.5f;
			desc.vScale = { 0.1f, 0.1f, 0.1f, 0.f };

			//vEffectSpawnPos.m128_f32[1] = 0.015f;
			vEffectSpawnPos.m128_f32[1] = m_pGameInstance->Get_Float_Random(0.111f, 0.112f);
			desc.vSpawnPos = vEffectSpawnPos;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Effect_Ice_Floor"), TEXT("Layer_Ice_Floor"), &desc)))
				return E_FAIL;

			m_pGameInstance->Play_Particle(TEXT("Shard"), Get_ParticleTag(TEXT("Shard")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard1"), Get_ParticleTag(TEXT("Shard1")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard2"), Get_ParticleTag(TEXT("Shard2")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard3"), Get_ParticleTag(TEXT("Shard3	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard4"), Get_ParticleTag(TEXT("Shard4	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard5"), Get_ParticleTag(TEXT("Shard5	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);

			if (m_bAttack == FALSE)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
				vector<CGameObject*> vecHitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);

				if (pPlayer->Get_CombatMode() == FALSE)
				{
					if (vecHitPlayer.size() != 0)
					{
						m_bAttack = TRUE;
						_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
						pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(100, 200), ELEMENT_FROST);
					}
				}
				else
				{
					for (auto PlayerBody : vecHitPlayer)
					{
						PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());
						pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(100, 200), ELEMENT_FROST);
					}
				}
			}
		}
	}
	else if (m_eSkill == SKILLOBJ_ABYSS_ICECONE)
	{
		if (!m_bIsCollGround && m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= -1.f)
		{
			m_bIsCollGround = TRUE;

			m_pGameInstance->PlaySoundW(L"Andrius_IceConeBreak.mp3", 0.5f);

			// 얼음 바닥 생성
			CEffect::EFFECT_DESC desc = {};
			desc.eEffectID = EFFECT_ICE_FLOOR_ICECONE;
			desc.fLifeTime = 1.5f;
			desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };

			_vector vEffectSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vEffectSpawnPos.m128_f32[1] = 0.5f;

			//vEffectSpawnPos.m128_f32[1] = 0.015f;
			vEffectSpawnPos.m128_f32[1] = m_pGameInstance->Get_Float_Random(0.111f, 0.112f);
			desc.vSpawnPos = vEffectSpawnPos;
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Effect_Ice_Floor"), TEXT("Layer_Ice_Floor"), &desc)))
				return E_FAIL;

			m_pGameInstance->Play_Particle(TEXT("Shard"), Get_ParticleTag(TEXT("Shard")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard1"), Get_ParticleTag(TEXT("Shard1")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard2"), Get_ParticleTag(TEXT("Shard2")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard3"), Get_ParticleTag(TEXT("Shard3	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard4"), Get_ParticleTag(TEXT("Shard4	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Shard5"), Get_ParticleTag(TEXT("Shard5	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
			m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);

			if (m_bAttack == FALSE)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
				vector<CGameObject*> vecHitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);

				if (pPlayer->Get_CombatMode() == FALSE)
				{
					if (vecHitPlayer.size() != 0)
					{
						m_bAttack = TRUE;
						_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
						pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(100, 200), ELEMENT_FROST);
					}
				}
				else
				{
					for (auto PlayerBody : vecHitPlayer)
					{
						PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());
						pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(100, 200), ELEMENT_FROST);
					}
				}
			}
		}
	}

	return S_OK;
}

HRESULT CSkill_IceCone::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_IceCone::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/
		
		if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_IceCone* CSkill_IceCone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_IceCone* pInstance = new CSkill_IceCone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_IceCone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_IceCone::Clone(void* pArg)
{
	CSkill_IceCone* pInstance = new CSkill_IceCone(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_IceCone")); 
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_IceCone::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}