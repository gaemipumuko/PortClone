#include "stdafx.h"
#include "Skill_IceShockWave.h"
#include "Andrius.h"
#include "Camera.h"
#include "Effect.h"
#include "Player.h"
#include "Part_Body.h"
#include "Eff_Impact_Glow.h"

#include "GameInstance.h"

CSkill_IceShockWave::CSkill_IceShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_IceShockWave::CSkill_IceShockWave(const CSkill_IceShockWave& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_IceShockWave::Add_Component(void* pArg)
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

	/* For.Com_Collider_SPHERE */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.fRadius = 2.f;
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CSkill_IceShockWave::Bind_ShaderResources()
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

HRESULT CSkill_IceShockWave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_IceShockWave::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = static_cast<SKILLOBJ_DESC*>(pArg);

		m_eSkill = pDesc->eSkillObjID;
		XMStoreFloat3(&m_vDir, pDesc->vDir);

		m_fLifeTime = pDesc->fLifeTime;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({ 0.4f, 0.4f, 0.4f, 0.f });
	m_fLifeTime = 2.f;

	//m_pGameInstance->Stop_Particle(TEXT("Shard"));
	//m_pGameInstance->Play_Particle(TEXT("Shard"), Get_ParticleTag(TEXT("Shard")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);

	//m_pGameInstance->Stop_Particle(TEXT("Death"));
	//m_pGameInstance->Play_Particle(TEXT("Death"), Get_ParticleTag(TEXT("Death")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom);
	//m_pGameInstance->Stop_Particle(TEXT("Shard"));
	m_pGameInstance->Play_Particle(TEXT("Shard"), Get_ParticleTag(TEXT("Shard")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Shard1"), Get_ParticleTag(TEXT("Shard1")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Shard2"), Get_ParticleTag(TEXT("Shard2")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Shard3"), Get_ParticleTag(TEXT("Shard3	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Shard4"), Get_ParticleTag(TEXT("Shard4	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Shard5"), Get_ParticleTag(TEXT("Shard5	")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);

	CImpact_Glow::IMPACT_DESC descc = {};
	descc.eEffectID = EFFECT_IMPACT_PLANE;
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.pParent = (CGameObject*)this;
	descc.vSpawnPos = vSelfPos;
	descc.vScale = { 3.f, 3.f, 1.f, 1.f };
	descc.fRadius = { 1.f };
	descc.iNumSpike = { 8 };
	descc.iFanSize = { 75 };
	descc.eELEMENT = ELEMENT_FROST;

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_IceShockWave::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_IceShockWave::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
		m_bAttack = TRUE;
		m_HitPlayerIndex.clear();
		Set_Destroy(TRUE);
	}

	m_pTransformCom->Look_At_Direction(XMLoadFloat3(&m_vDir));
	m_pTransformCom->Go_Up(fTimeDelta * 2.f);

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

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(3210, 5123), ELEMENT_FROST, FSM_HIT_THROW, 8.f, 0.f, vDir);


			_int iID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iID, FALSE);
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

		pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(3210, 5123), ELEMENT_FROST, FSM_HIT_THROW, 8.f);
	}

	return S_OK;
}

HRESULT CSkill_IceShockWave::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);//
#endif

	return S_OK;
}

HRESULT CSkill_IceShockWave::Render()
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

		if (FAILED(m_pShaderCom->Begin(VTXMESH_NONCULL)))
			return E_FAIL;

		//if (FAILED(m_pModelCom->Render(i)))
		//	return E_FAIL;
	}

	return S_OK;
}

CSkill_IceShockWave* CSkill_IceShockWave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_IceShockWave* pInstance = new CSkill_IceShockWave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_IceShockWave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_IceShockWave::Clone(void* pArg)
{
	CSkill_IceShockWave* pInstance = new CSkill_IceShockWave(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_IceShockWave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_IceShockWave::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}