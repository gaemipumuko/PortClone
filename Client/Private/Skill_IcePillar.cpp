#include "stdafx.h"
#include "Skill_IcePillar.h"

#include "GameInstance.h"

#include "Andrius.h"
#include "Camera.h"

#include "Effect.h"

CSkill_IcePillar::CSkill_IcePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CSkill_IcePillar::CSkill_IcePillar(const CSkill_IcePillar& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CSkill_IcePillar::Add_Component(void* pArg)
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_IceCone"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	///* For.Com_Collider_SPHERE */
	//CBounding_SPHERE::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	//BoundingDesc.fRadius = 1.f;
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	//if (FAILED(CGameObject::Add_Component(LEVEL_WORLD, TEXT("Prototype_Component_Collider_SPHERE"),
	//	TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CSkill_IcePillar::Bind_ShaderResources()
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

HRESULT CSkill_IcePillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_IcePillar::Initialize_Clone(void* pArg)
{
	//if (FAILED(__super::Initialize_Clone(pArg)))
	//	return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_vector* vSpawnPos = (_vector*)pArg;
		vSpawnPos->m128_f32[1] += 1.5f;
		//vSpawnPos->m128_f32[1] += 5.f;
		vSpawnPos->m128_f32[3] = 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vSpawnPos);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({ 0.4f, 0.4f, 0.4f, 0.f });
	m_fLifeTime = 2.f;

	return S_OK;
}

HRESULT CSkill_IcePillar::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_IcePillar::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_pTransformCom->Go_Down(fTimeDelta * 2.f);

	// ------------------------------------------------------------- 임시
	//CTransform* pMonsterTransform = static_cast<CTransform*>(
	//	m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Monster"), TEXT("Com_Transform")));
	//_vector vLook = pMonsterTransform->Get_State(CTransform::STATE_LOOK);
	//m_pTransformCom->Look_At_Direction(vLook);
	//m_pTransformCom->Go_Straight(fTimeDelta);
	// -------------------------------------------------------------

	if (!m_bIsCollGround && m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= -1.f)
	{
		m_bIsCollGround = TRUE;

		_vector vFlameSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vFlameSpawnPos.m128_f32[1] = 0.5f;

		CEffect::EFFECT_DESC tDesc = {};
		tDesc.vSpawnPos = vFlameSpawnPos;

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Effect_Flame"), TEXT("Layer_Flame"), &tDesc)))
			return E_FAIL;
	}

	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_IcePillar::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_IcePillar::Render()
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

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_IcePillar* CSkill_IcePillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_IcePillar* pInstance = new CSkill_IcePillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_IcePillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_IcePillar::Clone(void* pArg)
{
	CSkill_IcePillar* pInstance = new CSkill_IcePillar(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_IcePillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_IcePillar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}