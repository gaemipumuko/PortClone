#include "stdafx.h"
#include "Skill_FallingStoneTrail.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"
#include "SkillObj.h"

CSkill_FallingStoneTrail::CSkill_FallingStoneTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_FallingStoneTrail::CSkill_FallingStoneTrail(const CSkill_FallingStoneTrail& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_FallingStoneTrail::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_FallingStone"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meteor_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Collider_SPHERE */

	CBounding_SPHERE::BOUNDING_SPHERE_DESC BoundingDesc;

	BoundingDesc.fRadius = 5.f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_SKILL, &m_iColliderID, m_pColliderCom, this);
	

	return S_OK;
}

HRESULT CSkill_FallingStoneTrail::Bind_ShaderResources()
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

HRESULT CSkill_FallingStoneTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FallingStoneTrail::Initialize_Clone(void* pArg)
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

	m_pTransformCom->Set_Scale({ 0.06f, 0.06f, 0.06f, 0.f });

	return S_OK;
}

HRESULT CSkill_FallingStoneTrail::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_FallingStoneTrail::Tick(_float fTimeDelta)
{
	if (m_pModelCom->isFunctionTrue() == TRUE)
	{
		

		if (m_bAttack == FALSE)
		{



			CEffect::EFFECT_DESC descc = {};
			descc.eEffectID = EFFECT_HITTING_STAR;
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vSelfPos.m128_f32[1] += 1.f;
			vSelfPos += vSelfLook * 2.f;

			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.f, 1.f, 0.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Zhongli_Sp_Explosion", L"Layer_Effect", &vSelfPos)))
				return E_FAIL;

			descc.fLifeTime = 1.f;
			vSelfPos.m128_f32[1] += 0.01f;


									// 바람 스킬 범위 이펙트
			CSkillObj::SKILLOBJ_DESC desc = {};
			desc.eSkillObjID = SKILLOBJ_WIND_DEFINED;
			desc.fLifeTime = 2.f;
			desc.vSpawnPos = vSelfPos;
			desc.vScale = { 0.05f, 0.05f, 0.05f, 0.f };
			desc.vColor = {1.f,1.f,1.f,1.f};

			// 스킬 범위 바람 생성
			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Skill_WindRange", L"Layer_Skill", &desc)))
				return E_FAIL;
			//for (_int i = 0; i < 6; i++)
			//{

			//_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			//vSelfPos.m128_f32[0] -= i- 3.f;
			//vSelfPos.m128_f32[2] -= i- 3.f;

			////vSelfPos += vSelfLook * 2.f;
			//descc.vSpawnPos = vSelfPos;
			//descc.vScale = { 1.f, 1.f, 1.f, 0.f };

			//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Ground_Stomp", L"Layer_Effect", &descc)))
			//	return E_FAIL;

			//}

			m_bAttack = TRUE;


		}
	}

	if (m_bAttack == TRUE)
	{
		_vector vScale = m_pTransformCom->Get_Scale();
		vScale.m128_f32[0] *= (1.f + fTimeDelta*5.f);
		vScale.m128_f32[1] *= (1.f + fTimeDelta*5.f);
		vScale.m128_f32[2] *= (1.f + fTimeDelta*5.f);

		m_pTransformCom->Set_Scale(vScale);

		m_fLifeTime += fTimeDelta;
		//if (m_fLifeTime >= 0.3f)
		if (m_fLifeTime >= 0.05f)
		{
			CEffect::EFFECT_DESC descc = {};
			descc.eEffectID = EFFECT_HITTING_STAR;
			descc.fLifeTime = 5.f;
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vSelfPos.m128_f32[1] += 1.f;
			vSelfPos += vSelfLook * 2.f;

			descc.vSpawnPos = vSelfPos;
			descc.vScale = { 1.f, 1.f, 1.f, 0.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Zhongli_Sp_Explosion", L"Layer_Effect", &vSelfPos)))
				return E_FAIL;

			m_pGameInstance->Play_Particle(TEXT("Smoke0"), Get_ParticleTag(TEXT("Smoke0")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke1"), Get_ParticleTag(TEXT("Smoke1")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke2"), Get_ParticleTag(TEXT("Smoke2")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke3"), Get_ParticleTag(TEXT("Smoke3")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke0"), Get_ParticleTag(TEXT("Smoke0")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke1"), Get_ParticleTag(TEXT("Smoke1")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke2"), Get_ParticleTag(TEXT("Smoke2")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
			m_pGameInstance->Play_Particle(TEXT("Smoke3"), Get_ParticleTag(TEXT("Smoke3")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);

			m_pGameInstance->Release_Collider_Object(COLL_PLAYER_SKILL, m_iColliderID);
			Set_Destroy(TRUE);
		}
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());


	_float4x4 ColliderMatrix;
	XMStoreFloat4x4(&ColliderMatrix, 
		XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Bone001")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMLoadFloat4x4(&m_WorldMatrix)
	);

	ColliderMatrix._11 = 1.f;
	ColliderMatrix._22 = 1.f;
	ColliderMatrix._33 = 1.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderMatrix));

	m_fTimeAcc += fTimeDelta;



	return S_OK;
}

HRESULT CSkill_FallingStoneTrail::LateTick(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(2);
	m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_FallingStoneTrail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		_bool bLightMap = { };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
			return E_FAIL;

	/*	if (i == 1 &&m_fTimeAcc >=0.2f)
		{

			_float fOffset = m_fTimeAcc * i;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fOffset, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_METEOR_TRAIL)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
		}*/
		else if (i == 2 && m_fTimeAcc >= 0.3f)
		{

			_float fOffset = m_fTimeAcc * i*1.1f;

			//_vector vScale = m_pTransformCom->Get_Scale();
			//_vector vOrigin = vScale;
			//vScale.m128_f32[0] *= 1.5f;
			//vScale.m128_f32[1] *= 1.5f;
			//vScale.m128_f32[2] *= 1.5f;

			//m_pTransformCom->Set_Scale(vScale);


			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fOffset, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_METEOR_TRAIL)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;

			//m_pTransformCom->Set_Scale(vOrigin);
		}

	}

	return S_OK;
}

CSkill_FallingStoneTrail* CSkill_FallingStoneTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FallingStoneTrail* pInstance = new CSkill_FallingStoneTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_FallingStoneTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FallingStoneTrail::Clone(void* pArg)
{
	CSkill_FallingStoneTrail* pInstance = new CSkill_FallingStoneTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_FallingStoneTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FallingStoneTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}