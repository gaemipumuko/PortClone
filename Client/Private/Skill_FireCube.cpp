#include "stdafx.h"
#include "Skill_FireCube.h"
#include "FX_EffigyFireExplode.h"

#include "GameInstance.h"

#include "Andrius.h"
#include "Camera.h"

#include "Effect.h"
#include "Player.h"
#include "Part_Body.h"

CSkill_FireCube::CSkill_FireCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_FireCube::CSkill_FireCube(const CSkill_FireCube& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_FireCube::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FireCube"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	/*CBounding_OBB::BOUNDING_OBB_DESC tBoundingDesc = {};
	tBoundingDesc.vExtents = _float3(0.25f, 1.f, 0.25f);
	tBoundingDesc.vCenter = _float3(0.f, tBoundingDesc.vExtents.y, 0.f);*/

	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 1.f;
	tBoundingDesc.vCenter = _float3(0.f, tBoundingDesc.fRadius, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CSkill_FireCube::Bind_ShaderResources()
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

HRESULT CSkill_FireCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FireCube::Initialize_Clone(void* pArg)
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
		m_vDir = pDesc->vDir;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Set_Scale(pDesc->vScale);
		m_pTransformCom->Look_At_Direction(m_vDir);

		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
		m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
	}
	else
		return E_FAIL;

	m_pTransformCom->Set_ActiveYLerp(TRUE);

	/* Particle */
	wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_Large");
	wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_2_Large");
	wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_3_Large");
	wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_4_Large");

	m_ParticleTags.push_back(strParticle_Large1);
	m_ParticleTags.push_back(strParticle_Large2);
	m_ParticleTags.push_back(strParticle_Large3);
	m_ParticleTags.push_back(strParticle_Large4);

	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Large", strParticle_Large1,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2_Large", strParticle_Large2,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3_Large", strParticle_Large3,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4_Large", strParticle_Large4,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom);

	return S_OK;
}

HRESULT CSkill_FireCube::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_FireCube::Tick(_float fTimeDelta)
{
	// 플레이어 정보 구해놓기
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	// 내 정보들 구해놓기
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vSelfUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

	// 플레이어를 향하는 방향벡터
	vPlayerPos.m128_f32[1] += 0.5f;
	_vector vDirectionToPlayer = XMVector3Normalize(vPlayerPos - vSelfPos);

	m_fLifeTime -= fTimeDelta;
	m_fMoveTimer += fTimeDelta;

	if (m_bIsCollwithSomething == FALSE)
	{
		vector<CGameObject*> PlayerCollision = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
		for (auto& Coll : PlayerCollision)
		{
			m_bIsCollwithSomething = TRUE;
			_float4 vDir = {};
			XMStoreFloat4(&vDir, vDirectionToPlayer);
			PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(Coll)->Get_CreatureId());
			pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(1024, 1457), ELEMENT_FIRE, FSM_HIT_THROW, 1.f, 5.f, vDir);
		}
	}

	// 땅쪽에 부딪히거나 다른 무언가와 충돌했다면? 
	//if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= 0.5f /* || 다른 무언가, ex) 플레이어와 충돌했다면 */)
	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= 10.9661f /* || 다른 무언가, ex) 플레이어와 충돌했다면 */)
		m_bIsCollwithSomething = TRUE;

	if (XMVectorGetX(XMVector3Length(vPlayerPos - vSelfPos)) <= 4.f)
		m_bIsPlayerNearby = TRUE;

	// 무언가와 충돌했거나, LiftTime이 모두 소진되었다면?
	if (m_bIsCollwithSomething || m_fLifeTime <= 0.f)
	{
		for (auto& iter : m_ParticleTags)
			m_pGameInstance->Stop_Particle(iter);
		m_ParticleTags.clear();

		CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
		tExplodeDesc.fScale = 1.5f;
		tExplodeDesc.fExplodeExpand = 3.f;
		tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
		tExplodeDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 1.f, 0.f, 0.f };

		m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

		m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
		m_bAttack = TRUE;
		m_pGameInstance->PlaySoundW(L"EffigyFire_Chopping.mp3");
		Set_Destroy(TRUE);
	}

	// 처음 1초 정도는 직진하다가 선형보간
	if (m_fMoveTimer >= 1.f && !m_bIsPlayerNearby)
	{
		m_pTransformCom->Look_At_Direction_FreeRot(vDirectionToPlayer, vSelfUp);
		m_vPrevDirToPlayer = vDirectionToPlayer;
	}
	else if(m_bIsPlayerNearby)
	{
		m_pTransformCom->Look_At_Direction_FreeRot(m_vPrevDirToPlayer, vSelfUp);
	}

	m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 15.f);
	m_pTransformCom->Go_Straight(fTimeDelta * 3.6f);
	//m_pTransformCom->Go_Straight(fTimeDelta * 1.8f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

	return S_OK;
}

HRESULT CSkill_FireCube::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);

	return S_OK;
}

HRESULT CSkill_FireCube::Render()
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
		
		if (FAILED(m_pShaderCom->Begin(VTXMESH_EFFIGY_FIRECUBE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkill_FireCube::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(VTXMESH_SHADOW)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_FireCube* CSkill_FireCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FireCube* pInstance = new CSkill_FireCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_FireCube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FireCube::Clone(void* pArg)
{
	CSkill_FireCube* pInstance = new CSkill_FireCube(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_FireCube")); 
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FireCube::Free()
{
	__super::Free();

	for (auto& iter : m_ParticleTags)
		m_pGameInstance->Stop_Particle(iter);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}