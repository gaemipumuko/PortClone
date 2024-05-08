#include "stdafx.h"
#include "FX_EffigyFireTrsCow.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFireTrsCow::CFX_EffigyFireTrsCow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireTrsCow::CFX_EffigyFireTrsCow(const CFX_EffigyFireTrsCow& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireTrsCow::Add_Component(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfSphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_TrsCow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	TRSCOW_DESC* pTrsCowDesc = static_cast<TRSCOW_DESC*>(pArg);

	m_pTransformCom = pTrsCowDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	m_pBoneMatrix = pTrsCowDesc->pBoneMatrix;

	/* Particle */
	m_strConvParticle = Get_ParticleTag(L"Effigy_TrsCowConv");
	m_pGameInstance->Play_Particle(L"Effigy_TrsCowConv", m_strConvParticle, PARTICLE_EFFIGY_SPARK, TEX_PARTICLE_SPARK, m_pBoneMatrix, m_pTransformCom);

	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_vFrame.x += fTimeDelta;
		m_vFrame.y = m_vFrame.x - 0.5f;
		m_vFrame.z = fmod(m_vFrame.x, 1.f);

		if (m_vFrame.x > 2.f)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;
		}
		else if (m_vFrame.x > 0.4f)
		{
			m_bFireAura = TRUE;
			m_pGameInstance->Stop_Particle(m_strConvParticle, 2.f);
		}
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::LateTick(_float fTimeDelta)
{
	if (m_bFireAura == TRUE)
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_EffigyFireTrsCow::Render()
{
	if (FAILED(CFX_EffigyFireTrsCow::Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_TRSCOW)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CFX_EffigyFireTrsCow* CFX_EffigyFireTrsCow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireTrsCow* pInstance = new CFX_EffigyFireTrsCow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireTrsCow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireTrsCow::Clone(void* pArg)
{
	CFX_EffigyFireTrsCow* pInstance = new CFX_EffigyFireTrsCow(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireTrsCow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireTrsCow::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
