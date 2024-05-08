#include "stdafx.h"
#include "Skill_FallingStone.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"

CSkill_FallingStone::CSkill_FallingStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_FallingStone::CSkill_FallingStone(const CSkill_FallingStone& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_FallingStone::Add_Component(void* pArg)
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

HRESULT CSkill_FallingStone::Bind_ShaderResources()
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

HRESULT CSkill_FallingStone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FallingStone::Initialize_Clone(void* pArg)
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

	m_pGameInstance->PlaySoundW(L"Zhongli_Burst", 2.f);

	return S_OK;
}

HRESULT CSkill_FallingStone::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_FallingStone::Tick(_float fTimeDelta)
{
	if (m_pModelCom->isFunctionTrue() == TRUE)
	{
		if (m_bAttack == FALSE)
		{
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vSelfPos.m128_f32[1] += 1.f;
			vSelfPos += vSelfLook * 2.f;

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Zhongli_Sp_Explosion", L"Layer_Effect", &vSelfPos)))
				return E_FAIL;

			m_bAttack = TRUE;
			vector<CGameObject*> vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_SKILL, m_iColliderID, COLL_MONSTER_BODY);

			for (auto& HitMonsters : vecHitMonsters)
			{
				static_cast<CPartObject*>(HitMonsters)->Set_Damage(static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))->Get_ElementDamage(), ELEMENT_EARTH, FSM_OBJECT_HIT_H, TRUE);
			}
		}
	}

	if (m_bAttack == TRUE)
	{
		m_fLifeTime += fTimeDelta;
		if (m_fLifeTime >= 0.2f)
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

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Zhongli_Sp_Explosion", L"Layer_Effect", &vSelfPos)))
				return E_FAIL;

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

HRESULT CSkill_FallingStone::LateTick(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(0);
	m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_FallingStone::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (i == 0)
		{

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;

			_bool bLightMap = { };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_ADDEXPOSURE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}

	}

	return S_OK;
}

CSkill_FallingStone* CSkill_FallingStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FallingStone* pInstance = new CSkill_FallingStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_FallingStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FallingStone::Clone(void* pArg)
{
	CSkill_FallingStone* pInstance = new CSkill_FallingStone(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_FallingStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FallingStone::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}