#include "stdafx.h"
#include "Skill_Phoenix.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"

CSkill_Phoenix::CSkill_Phoenix(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_Phoenix::CSkill_Phoenix(const CSkill_Phoenix& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_Phoenix::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Phoenix"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_SPHERE */

	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc;

	tBoundingDesc.fRadius = 3.5f;
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_SKILL, &m_iColliderID, m_pColliderCom, this);
	

	return S_OK;
}

HRESULT CSkill_Phoenix::Bind_ShaderResources()
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

void CSkill_Phoenix::Collision_Monster()
{
	if (m_bAttackStop == TRUE)
		return;

	vector<CGameObject*> vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_SKILL, m_iColliderID, COLL_MONSTER_BODY);
	for (auto& HitMonsters : vecHitMonsters)
	{
		static_cast<CPartObject*>(HitMonsters)->Set_Damage(static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_ElementDamage(), ELEMENT_FIRE);
		_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
		m_pGameInstance->Set_IsCollision(COLL_MONSTER_BODY, iHitID, FALSE);
		m_HitMonsterIDList.push_back(iHitID);

		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STORMTERROR)
		{
			m_bAttackStop = TRUE;
			return;
		}
	}
}

void CSkill_Phoenix::Release_Collision()
{
	for (auto iter : m_HitMonsterIDList) /* 충돌했던 몬스터 검사 제외 초기화 */
	{
		m_pGameInstance->Set_IsCollision(COLL_MONSTER_BODY, iter, TRUE);
	}
	m_HitMonsterIDList.clear();

	if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STORMTERROR)
		m_bAttackStop = FALSE;
}

HRESULT CSkill_Phoenix::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Phoenix::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Look_At_Direction(pDesc->vDir);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({ 30.f, 30.f, 30.f, 0.f });
	
	m_pModelCom->Set_Animation(0);

	return S_OK;
}

HRESULT CSkill_Phoenix::PreviousTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	_float4x4 ColliderMatrix;
	XMStoreFloat4x4(&ColliderMatrix,
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("RootNode")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMLoadFloat4x4(&m_WorldMatrix)
	);

	ColliderMatrix._11 = 1.f;
	ColliderMatrix._12 = 0.f;
	ColliderMatrix._13 = 0.f;
	ColliderMatrix._14 = 0.f;

	ColliderMatrix._21 = 0.f;
	ColliderMatrix._22 = 1.f;
	ColliderMatrix._23 = 0.f;
	ColliderMatrix._24 = 0.f;

	ColliderMatrix._31 = 0.f;
	ColliderMatrix._32 = 0.f;
	ColliderMatrix._33 = 1.f;
	ColliderMatrix._34 = 0.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderMatrix));

	return S_OK;
}

HRESULT CSkill_Phoenix::Tick(_float fTimeDelta)
{
	_float fRate = {};
	if (m_fLifeTime > 1.f)
		m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta);
	else
	{
		if (m_fLifeTime > 0.2f && m_bParticle == FALSE)
		{
			//m_pModelCom->Get_CombinedMatrix("HitBox")
			m_pGameInstance->Play_Particle(TEXT("FeatherFA"), Get_ParticleTag(TEXT("FeatherFA")), PARTICLE_FEATHERA, TEX_PARTICLE_FEATHERA, nullptr , m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Feather1"), Get_ParticleTag(TEXT("Feather1")), PARTICLE_FEATHER, TEX_PARTICLE_FEATHER, nullptr, m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Feather2"), Get_ParticleTag(TEXT("Feather2")), PARTICLE_FEATHER, TEX_PARTICLE_FEATHER, nullptr, m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Feather2"), Get_ParticleTag(TEXT("Feather2")), PARTICLE_FEATHER, TEX_PARTICLE_FEATHER, nullptr, m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Feather3"), Get_ParticleTag(TEXT("Feather3")), PARTICLE_FEATHER, TEX_PARTICLE_FEATHER, nullptr, m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Feather3"), Get_ParticleTag(TEXT("Feather3")), PARTICLE_FEATHER, TEX_PARTICLE_FEATHER, nullptr, m_pTransformCom, 2.f);

			m_pGameInstance->Play_Particle(TEXT("Explode_1"), Get_ParticleTag(TEXT("Explode_1")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_SMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_2"), Get_ParticleTag(TEXT("Explode_2")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_SMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_3"), Get_ParticleTag(TEXT("Explode_3")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_SMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_4"), Get_ParticleTag(TEXT("Explode_4")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_SMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_5"), Get_ParticleTag(TEXT("Explode_5")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_SMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);

			m_pGameInstance->Play_Particle(TEXT("Explode_A"), Get_ParticleTag(TEXT("Explode_A")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_BASESMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_B"), Get_ParticleTag(TEXT("Explode_B")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_BASESMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_C"), Get_ParticleTag(TEXT("Explode_C")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_BASESMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);
			m_pGameInstance->Play_Particle(TEXT("Explode_D"), Get_ParticleTag(TEXT("Explode_D")), PARTICLE_PHOENIX_SMOKE, TEX_PARTICLE_BASESMOKE, m_pModelCom->Get_CombinedMatrix("RootNode"), m_pTransformCom, 2.f);

			m_bParticle = TRUE;
		}
		m_pModelCom->Play_Animation(TRUE, m_pTransformCom, fTimeDelta * 0.38f);
	}

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_fLifeTime += fTimeDelta;
	m_fHitDuration += fTimeDelta;
	if (m_fHitDuration >= 0.25f)
	{
		m_fHitDuration = 0.f;
		CSkill_Phoenix::Release_Collision();
	}

	if (m_fLifeTime > 1.f)
	{
		vPos.m128_f32[1] += fTimeDelta;
		vPos += XMVector3Normalize(vLook) * 0.3f;
		m_pTransformCom->Look_At_Position(vPos);
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vPos += XMVector3Normalize(vLook) * 0.3f;
		//vPos.m128_f32[1] += fTimeDelta * 20.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		CSkill_Phoenix::Collision_Monster();
	}
	else
	{
		vPos += XMVector3Normalize(vLook) * 0.1f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		CSkill_Phoenix::Collision_Monster();
	}
	if (m_fLifeTime > 3.f)
	{
		CSkill_Phoenix::Release_Collision();

		m_pGameInstance->Release_Collider_Object(COLL_PLAYER_SKILL, m_iColliderID);
		Set_Destroy(TRUE);
	}

	return S_OK;
}

HRESULT CSkill_Phoenix::LateTick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	//m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CSkill_Phoenix::Render()
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

		_bool bLightMap = { };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PHOENIX)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_Phoenix* CSkill_Phoenix::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Phoenix* pInstance = new CSkill_Phoenix(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Phoenix"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Phoenix::Clone(void* pArg)
{
	CSkill_Phoenix* pInstance = new CSkill_Phoenix(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Phoenix"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Phoenix::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}