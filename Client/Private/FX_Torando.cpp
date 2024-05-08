#include "stdafx.h"
#include "FX_Tornado.h"

#include "GameInstance.h"

CFX_Torando::CFX_Torando(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_Torando::CFX_Torando(const CFX_Torando& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_Torando::Add_Component(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tornado"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tornado"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Torando::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	_float4	vUVRate = { m_fFrame, m_fFrame * 1.4f, 0.f, 0.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &vUVRate, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Torando::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_Torando::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
	
	m_pTransformCom = pAsfxDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	m_pTransformCom->Set_Scale({ 2.f, 4.f, 2.f, 0.f });

	m_strParticleTag[0] = { Get_ParticleTag(L"Tornado_1") };
	m_strParticleTag[1] = { Get_ParticleTag(L"Tornado_2") };

	m_pGameInstance->Play_Particle(L"Tornado_1", m_strParticleTag[0], 9, 7, nullptr, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Tornado_2", m_strParticleTag[1], 9, 8, nullptr, m_pTransformCom);

	return S_OK;
}

HRESULT CFX_Torando::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Torando::Tick(_float fTimeDelta)
{
	/* FX Frame */
	m_fFrame += fTimeDelta;
	m_fFrame = fmod(m_fFrame, 1.f);

	/* Turn */
	m_pTransformCom->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);

	return S_OK;
}

HRESULT CFX_Torando::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_Torando::Render()
{
	if (FAILED(CFX_Torando::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_TORNADO)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CFX_Torando* CFX_Torando::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_Torando* pInstance = new CFX_Torando(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_Torando"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_Torando::Clone(void* pArg)
{
	CFX_Torando* pInstance = new CFX_Torando(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_Torando"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_Torando::Free()
{
	__super::Free();

	m_pGameInstance->Stop_Particle(m_strParticleTag[0]);
	m_pGameInstance->Stop_Particle(m_strParticleTag[1]);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
