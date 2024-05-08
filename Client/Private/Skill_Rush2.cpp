#include "stdafx.h"
#include "Skill_Rush2.h"

#include "GameInstance.h"

#include "Andrius.h"
#include "Camera.h"

#include "Effect.h"

CSkill_Rush2::CSkill_Rush2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_Rush2::CSkill_Rush2(const CSkill_Rush2& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_Rush2::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rush2"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Rush2::Bind_ShaderResources()
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

HRESULT CSkill_Rush2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Rush2::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = static_cast<SKILLOBJ_DESC*>(pArg);

		m_eSkill = pDesc->eSkillObjID;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Set_Scale(pDesc->vScale);
		m_pTransformCom->Look_At_Direction(pDesc->vDir);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Rush2::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_Rush2::Tick(_float fTimeDelta)
{
	CAndrius* pAndrius = static_cast<CAndrius*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Boss"));
	_bool bIsRush= pAndrius->Get_IsRushStarted();

	CTransform* pAndriusTransform
		= static_cast<CTransform*>(m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Boss"), TEXT("Com_Transform")));
	_vector vAndriusPos = pAndriusTransform->Get_State(CTransform::STATE_POSITION);
	_vector vAndriusLook = XMVector3Normalize(pAndriusTransform->Get_State(CTransform::STATE_LOOK));
	_vector vAndriusUp = XMVector3Normalize(pAndriusTransform->Get_State(CTransform::STATE_UP));
	
	vAndriusPos += vAndriusLook * 6.f;
	vAndriusPos += vAndriusUp * 2.f;
	
	_vector vSelfScale = m_pTransformCom->Get_Scale();
	m_pTransformCom->Set_Scale(vSelfScale);
	m_pTransformCom->Look_At_Direction(vAndriusLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAndriusPos);
	//m_pTransformCom->RotationFromCurrentState(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(5.f));
	//m_pTransformCom->Go_Straight(fTimeDelta);

	if (bIsRush == FALSE)
		Set_Destroy(TRUE);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_Rush2::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_Rush2::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVScaleRate", &m_vScaleUV, sizeof(_float2))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpinRate", &m_vSpinUV, sizeof(_float2))))
	//	return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(VTXMESH_RUSH2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_Rush2* CSkill_Rush2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Rush2* pInstance = new CSkill_Rush2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Rush2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Rush2::Clone(void* pArg)
{
	CSkill_Rush2* pInstance = new CSkill_Rush2(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Rush2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Rush2::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}