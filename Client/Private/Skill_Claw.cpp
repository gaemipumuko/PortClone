#include "stdafx.h"
#include "Skill_Claw.h"

#include "GameInstance.h"

#include "Camera.h"

CSkill_Claw::CSkill_Claw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CSkill_Claw::CSkill_Claw(const CSkill_Claw& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CSkill_Claw::Add_Component(void* pArg)
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andrius_Claw"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkill_Claw::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Claw::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Claw::Initialize_Clone(void* pArg)
{
	//if (FAILED(__super::Initialize_Clone(pArg)))
	//	return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		/*_vector* vSpawnPos = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vSpawnPos);*/
		CLAW_DESC* pDesc = (CLAW_DESC*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);
		m_pTransformCom->Look_At_Direction(pDesc->vDir * -1.f);

		m_pTransformCom->Set_Scale(pDesc->vScale);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		if(pDesc->bReverse == FALSE)
			m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(90.f));
		else
			m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(270.f));

		m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(pDesc->fAngle));
	}
	else
		return E_FAIL;
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	/*m_pTransformCom->Set_Scale({ 0.05f, 0.05f, 0.08f, 0.f });
	m_pTransformCom->Rotation({ 1.f,0.f,0.f,0.f }, XMConvertToRadians(90.f));*/

	//m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(270.f));
	//m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(90.f));


	m_fLifeTime = 1.f;

	return S_OK;
}

HRESULT CSkill_Claw::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_Claw::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta;
	//m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(m_fTimeAcc*10.f));


	// ------------------------------------------------------------- 임시
	//CTransform* pMonsterTransform = static_cast<CTransform*>(
	//	m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Monster"), TEXT("Com_Transform")));
	//_vector vLook = pMonsterTransform->Get_State(CTransform::STATE_LOOK);
	//m_pTransformCom->Look_At_Direction(vLook);
	//m_pTransformCom->Go_Straight(fTimeDelta);
	// -------------------------------------------------------------



	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_Claw::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_Claw::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_ANDRIUS_CLAW)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_Claw* CSkill_Claw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Claw* pInstance = new CSkill_Claw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Claw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Claw::Clone(void* pArg)
{
	CSkill_Claw* pInstance = new CSkill_Claw(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Claw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Claw::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}