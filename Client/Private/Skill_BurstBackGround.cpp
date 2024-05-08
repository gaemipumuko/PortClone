#include "stdafx.h"
#include "Skill_BurstBackGround.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"

CSkill_BurstBackGround::CSkill_BurstBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_BurstBackGround::CSkill_BurstBackGround(const CSkill_BurstBackGround& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_BurstBackGround::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Burst_BackGround"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Background_Mask"),
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

HRESULT CSkill_BurstBackGround::Bind_ShaderResources()
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

HRESULT CSkill_BurstBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_BurstBackGround::Initialize_Clone(void* pArg)
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

	m_pTransformCom->Set_Scale({ 0.3f, 0.3f, 0.3f, 0.f });
		
	//m_pTransformCom->Rotation({ 1.f,0.f,0.f,0.f }, XMConvertToRadians(90.f));

	m_pTransformCom->RotationFromCurrentState(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CSkill_BurstBackGround::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_BurstBackGround::Tick(_float fTimeDelta)
{

	CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera")));
	if (pCamera->Get_SceneId(CAMSCENEID_ZHONGLI) == SCENE_TWO)
	{
		m_pGameInstance->Release_Collider_Object(COLL_PLAYER_SKILL, m_iColliderID);
		Set_Destroy(TRUE);
	}

	if (m_fTimeAcc < 0.2f)
		m_fTimeAcc += fTimeDelta;

	m_vSpinUV.x += fTimeDelta / 77.f;



	/*m_fLifeTime += fTimeDelta;
	if (m_fLifeTime >= 1.f)
	{
		m_pGameInstance->Release_Collider_Object(COLL_PLAYER_SKILL, m_iColliderID);
		Set_Destroy(TRUE);
	}*/

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_BurstBackGround::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_BurstBackGround::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpinRate", &m_vSpinUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	_int		iNumMeshes = m_pModelCom->Get_NumMeshes();
		
	for (_int i = (iNumMeshes-1); i >= 0; i--)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			continue;

		if (i == 0)
		{

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXMESH_BURST_RAY)))
				return E_FAIL;
		}
		else
		{

			if (FAILED(m_pShaderCom->Begin(VTXMESH_BURST_BACKGROUND)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_BurstBackGround* CSkill_BurstBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_BurstBackGround* pInstance = new CSkill_BurstBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_BurstBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_BurstBackGround::Clone(void* pArg)
{
	CSkill_BurstBackGround* pInstance = new CSkill_BurstBackGround(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_BurstBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_BurstBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}