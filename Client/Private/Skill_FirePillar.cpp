#include "stdafx.h"
#include "Skill_FirePillar.h"

#include "GameInstance.h"

#include "Camera.h"
#include "FX_EffigyFireExplode.h"

#include "Effect.h"
#include "Player.h"

CSkill_FirePillar::CSkill_FirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_FirePillar::CSkill_FirePillar(const CSkill_FirePillar& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_FirePillar::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FirePillar"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 1.5f;
	tBoundingDesc.vCenter = _float3(0.f, tBoundingDesc.fRadius, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CSkill_FirePillar::Bind_ShaderResources()
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

HRESULT CSkill_FirePillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FirePillar::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = static_cast<SKILLOBJ_DESC*>(pArg);

		m_eSkill = pDesc->eSkillObjID;
		m_fLifeTime = pDesc->fLifeTime;

		_vector vSpawnPos = pDesc->vSpawnPos;
		vSpawnPos.m128_f32[1] -= 20.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSpawnPos);
		m_pTransformCom->Set_Scale(pDesc->vScale);

		CAssetFX::ASFX_DESC tAsfxDesc = { };
		tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
		tAsfxDesc.vInitPosition = pDesc->vSpawnPos + _vector{ 0.f, 0.01f, 0.f, 0.f };
		
		m_pGameInstance->Play_AssetFX(L"EffigyFirePillar", Get_ParticleTag(L"EffigyFirePillar"), &tAsfxDesc); 
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_FirePillar::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_FirePillar::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
		m_bAttack = TRUE;
		Set_Destroy(TRUE);
	}

	if (!m_bIsReachTop && m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= 11.9661f)
	{
		m_pTransformCom->Go_Up(fTimeDelta * 4.f);
	}
	else if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] > 11.9661f)
	{
		m_fStopTime += fTimeDelta;
		if (m_fStopTime >= 0.5f)
			m_bIsReachTop = TRUE;
	}
	
	if (m_bIsReachTop == TRUE)
	{
		m_fDissapearSpeed += fTimeDelta * 1.5f;
		m_pTransformCom->Go_Down(fTimeDelta * m_fDissapearSpeed);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	_float4x4 ColliderPos = m_WorldMatrix;
	ColliderPos._11 = 1.f;
	ColliderPos._22 = 1.f;
	ColliderPos._33 = 1.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderPos));

	if (!m_bIsCollGround && m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] > 10.9661f)
	{
		m_bIsCollGround = TRUE;

		m_pGameInstance->PlaySoundW(L"Andrius_IceConeBreak.mp3", 0.5f);

		CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
		tExplodeDesc.fScale = 1.5f;
		tExplodeDesc.fExplodeExpand = 3.f;
		tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
		tExplodeDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

		/* Particle */
		wstring strParticleTag1 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_1");
		wstring strParticleTag2 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_2");
		wstring strParticleTag3 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_3");
		wstring strParticleTag4 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_4");

		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_1", strParticleTag1,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_2", strParticleTag2,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_3", strParticleTag3,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);
		m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_4", strParticleTag4,
			PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);

		m_pGameInstance->Stop_Particle(strParticleTag1, 5.f);
		m_pGameInstance->Stop_Particle(strParticleTag2, 5.f);
		m_pGameInstance->Stop_Particle(strParticleTag3, 5.f);
		m_pGameInstance->Stop_Particle(strParticleTag4, 5.f);

		if (m_bAttack == FALSE)	
		{
			vector<CGameObject*> vecHitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
			if (vecHitPlayer.size() != 0)
			{
				m_bAttack = TRUE;
				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
				pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(100, 200), ELEMENT_FIRE);
			}
		}
	}

	return S_OK;
}

HRESULT CSkill_FirePillar::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_FirePillar::Render()
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

	m_pColliderCom->Render();

	return S_OK;
}

CSkill_FirePillar* CSkill_FirePillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FirePillar* pInstance = new CSkill_FirePillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_FirePillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FirePillar::Clone(void* pArg)
{
	CSkill_FirePillar* pInstance = new CSkill_FirePillar(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_FirePillar")); 
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FirePillar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}