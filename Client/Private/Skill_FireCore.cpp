#include "stdafx.h"
#include "Skill_FireCore.h"
#include "Camera.h"
#include "Eff_Effigy_Recharge_Trail.h"
#include "Player.h"
#include "SkillObj.h"

#include "GameInstance.h"

CSkill_FireCore::CSkill_FireCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CSkill_FireCore::CSkill_FireCore(const CSkill_FireCore& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CSkill_FireCore::Invalidate_Animation(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_FireCore::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FireCore"),
	//if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FirePillar"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 0.5f;
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CSkill_FireCore::Bind_ShaderResources()
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

HRESULT CSkill_FireCore::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FireCore::Initialize_Clone(void* pArg)
{
	if (FAILED(CSkill_FireCore::Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	FIRECORE_DESC* pDesc = static_cast<FIRECORE_DESC*>(pArg);

	m_eSkill = pDesc->eSkillObjID;

	_vector vSpawnPos = pDesc->vSpawnPos;
	vSpawnPos.m128_f32[1] += 1.f;
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




	CEffect::EFFECT_DESC tTrailDesc = {};
	tTrailDesc.fLifeTime = 5.f;
	_vector vLinkPos = pDesc->vLinkPos;
	_vector vCorePos = pDesc->vSpawnPos;
	_vector vPos = (pDesc->vLinkPos + pDesc->vSpawnPos) * 0.5f;
	vPos.m128_f32[3] = 1.f;

	tTrailDesc.vSpawnPos = vPos;
	tTrailDesc.vScale = { 10.f, 1.f, 1.f, 0.f };
	tTrailDesc.vDir = XMVector3Normalize(vPos - pDesc->vSpawnPos);

	m_pTrail = (CEff_Effigy_Recharge_Trail*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Eff_Effigy_Recharge_Trail"))->Clone(&tTrailDesc);

	return S_OK;
}

HRESULT CSkill_FireCore::PreviousTick(_float fTimeDelta)
{
	CGameObject::Add_Target(this);

	m_pTrail->PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CSkill_FireCore::Tick(_float fTimeDelta)
{
	if (m_eReceiveElement == ELEMENT_WATER)
	{
		m_eReceiveElement = ELEMENT_END;
		m_fHp -= 1.f;
	}

	if (m_pGameInstance->Key_Down('9'))
	{
		m_fHp -= 0.4f;
	}
	
	if (m_fHp <= 0 || m_bIgniteSuccess == TRUE)
	{
		m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
		m_bAttack = TRUE;
		Set_Destroy(TRUE);
		CGameObject::Clear_Target();
	}

	//m_pTransformCom->RotationFromCurrentState(XMVectorSet(1.f, 0.f, 0.f, 0.f), fTimeDelta * 1.f);
	//m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
	m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 1.f);

	_float4x4 ColliderPos = m_WorldMatrix;
	ColliderPos._11 = 1.f;
	ColliderPos._22 = 1.f;
	ColliderPos._33 = 1.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderPos));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pTrail->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CSkill_FireCore::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	CGameObject::Clear_Target();

	m_pTrail->LateTick(fTimeDelta);

	return S_OK;
}

HRESULT CSkill_FireCore::Render()
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
		
		//if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
		//if (FAILED(m_pShaderCom->Begin(VTXMESH_NONCULL)))
		if (FAILED(m_pShaderCom->Begin(VTXMESH_EFFIGY_FIRECORE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	m_pColliderCom->Render();

	return S_OK;
}

CSkill_FireCore* CSkill_FireCore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FireCore* pInstance = new CSkill_FireCore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_FireCore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FireCore::Clone(void* pArg)
{
	CSkill_FireCore* pInstance = new CSkill_FireCore(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_FireCore")); 
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FireCore::Free()
{
	CGameObject::Free();

	Safe_Release(m_pTrail);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}