#include "stdafx.h"
#include "SkyBox.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"

#include "GameInstance.h"

CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkyBox::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	SKYBOX_DESC* pSkyBoxDesc = (SKYBOX_DESC*)pArg;
	{
		m_eLevel = pSkyBoxDesc->eLevel;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkyBox::Tick(_float fTimeDelta)
{
	if (LEVEL_WORLD == m_eLevel)
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(0.01f));

	return S_OK;
}

HRESULT CSkyBox::LateTick(_float fTimeDelta)
{
	if (m_eRenderUI == RUI_CHAR)
		return S_OK;

	_float4x4 ViewMatrix;

	XMStoreFloat4x4(&ViewMatrix, m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vector*)&ViewMatrix.m[3][0]);

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_PRIORITY, this);

	return S_OK;
}

HRESULT CSkyBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_EFF_DVALIN_SKYBOX)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkyBox::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_eLevel)
	{
	case Client::LEVEL_WORLD:

		/* For.Com_Model */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_WorldSkyBox"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	case Client::LEVEL_TEMPLE_LION:
	case Client::LEVEL_STRONGHOLD_ABYSSMAGE:

		/* For.Com_Model */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_DungeonSkyBox"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	case Client::LEVEL_STORMTERROR:

		/* For.Com_Model */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_DvalinSkyBox"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		break;
	}

	return S_OK;
}

HRESULT CSkyBox::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	switch (m_eLevel)
	{
	case Client::LEVEL_WORLD:
	case Client::LEVEL_TEMPLE_LION:
	case Client::LEVEL_STRONGHOLD_ABYSSMAGE:
	case Client::LEVEL_QUEST:
	{
		m_vSkyBoxScale = _float2(1.0f, 1.0f);
		m_vSkyBoxOffset = _float2(0.0f, 0.0f);

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSkyBoxScale", &m_vSkyBoxScale, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSkyBoxOffset", &m_vSkyBoxOffset, sizeof(_float2))))
			return E_FAIL;
	}
		break;
	case Client::LEVEL_STORMTERROR:
	{
		m_vSkyBoxScale = _float2(4.0f, 5.0f);
		m_vSkyBoxOffset = _float2(0.0f, -0.7f);

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSkyBoxScale", &m_vSkyBoxScale, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSkyBoxOffset", &m_vSkyBoxOffset, sizeof(_float2))))
			return E_FAIL;
	}
		break;
	}

	return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkyBox* pInstance = new CSkyBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkyBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
	CSkyBox* pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkyBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
