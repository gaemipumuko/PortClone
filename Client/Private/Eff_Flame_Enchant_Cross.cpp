#include "stdafx.h"
#include "Eff_Flame_Enchant_Cross.h"

#include "GameInstance.h"

#include "Camera.h"

CEff_Flame_Enchant_Cross::CEff_Flame_Enchant_Cross(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CEff_Flame_Enchant_Cross::CEff_Flame_Enchant_Cross(const CEff_Flame_Enchant_Cross& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CEff_Flame_Enchant_Cross::Add_Component(void* pArg)
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cross"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Flame_Enchant"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;	
	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::Initialize_Clone(void* pArg)
{
	//if (FAILED(__super::Initialize_Clone(pArg)))
	//	return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		FLAMEENCHANTCROSS_DESC* pDesc = (FLAMEENCHANTCROSS_DESC*)pArg;
		m_pParentModelCom = pDesc->pParentModelCom;
		m_pParentCombineWorldMatrix = pDesc->pCombineWorldMatrix;

		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentCombineWorldMatrix));

	}
	else
		return E_FAIL;

	//m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(90.f));
	m_pTransformCom->Set_Scale({ 0.02f, 0.02f, 0.02f, 0.f });

	m_fLifeTime = 15.f;
	//m_fLifeTime = 15.f;

	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta;

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentCombineWorldMatrix));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Flame_Enchant_Cross::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_FLAME_ENCHANT_CROSS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CEff_Flame_Enchant_Cross* CEff_Flame_Enchant_Cross::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Flame_Enchant_Cross* pInstance = new CEff_Flame_Enchant_Cross(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Flame_Enchant_Cross"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Flame_Enchant_Cross::Clone(void* pArg)
{
	CEff_Flame_Enchant_Cross* pInstance = new CEff_Flame_Enchant_Cross(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Flame_Enchant_Cross"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Flame_Enchant_Cross::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}