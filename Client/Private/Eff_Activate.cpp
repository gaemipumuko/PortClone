#include "stdafx.h"
#include "Eff_Activate.h"
#include "GameInstance.h"
#include "Camera.h"


CEff_Activate::CEff_Activate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Activate::CEff_Activate(const CEff_Activate& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Activate::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_WindField"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Activate::Bind_ShaderResources()
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

HRESULT CEff_Activate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Activate::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = (EFFECT_DESC*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		//m_pTransformCom->Look_At_Direction(pDesc->vDir);
		
		m_pTransformCom->Set_Scale({ 
			0.0025f * pDesc->vScale.m128_f32[0], 
			0.0004f * pDesc->vScale.m128_f32[1], 
			0.0025f * pDesc->vScale.m128_f32[2], 0.f
			});

		m_pTransformCom->RotationFromCurrentState(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRadians.x));
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRadians.y));
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(pDesc->vRadians.z));
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({0.1f,0.1f,0.1f, 0.f});

	return S_OK;
}

HRESULT CEff_Activate::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEff_Activate::Tick(_float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;
	
	_int iMax = m_pModelCom->Get_AnimationCount();

	if (m_pGameInstance->Key_Down('L'))
	{
		if (m_iAnim < iMax-1)
			m_iAnim++;
		else
			m_iAnim = 0;
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Activate::LateTick(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(m_iAnim);
	m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Activate::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
				return E_FAIL;

			_bool bLightMap = { };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_EFF_ACTIVATE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;

	}

	return S_OK;
}

CEff_Activate* CEff_Activate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Activate* pInstance = new CEff_Activate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Activate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Activate::Clone(void* pArg)
{
	CEff_Activate* pInstance = new CEff_Activate(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Activate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Activate::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}