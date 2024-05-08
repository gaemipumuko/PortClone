#include "stdafx.h"
#include "FX_EffigyFireTrsFish.h"

#include "Effect.h"
#include "Eff_Impact_Glow.h"

#include "DeciderObject.h"
#include "GameInstance.h"

#define NUM_SPLASH 5

CFX_EffigyFireTrsFish::CFX_EffigyFireTrsFish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireTrsFish::CFX_EffigyFireTrsFish(const CFX_EffigyFireTrsFish& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireTrsFish::Add_Component(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfTorus"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_TrsFish"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireTrsFish::Bind_ShaderResources()
{
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

HRESULT CFX_EffigyFireTrsFish::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrsFish::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	TRSFISH_DESC* pTrsCowDesc = static_cast<TRSFISH_DESC*>(pArg);

	m_pTransformCom = pTrsCowDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	m_pBoneMatrix = pTrsCowDesc->pBoneMatrix;

	m_fInitY = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] + 0.2f;
	XMStoreFloat4x4(&m_TrsFishMatrix, XMMatrixIdentity());

	/* Invalidate Matrix */
	_matrix TrsFishMatrix = XMLoadFloat4x4(m_pBoneMatrix) * m_pTransformCom->Get_WorldMatrix();

	memcpy(m_TrsFishMatrix.m[3], &TrsFishMatrix.r[3], sizeof _float4);
	m_TrsFishMatrix._42 = m_fInitY;

	m_TrsFishMatrix._11 = 2.f;
	m_TrsFishMatrix._22 = 0.f;
	m_TrsFishMatrix._33 = 2.f;

	/* Create Thorn Effect */
	CImpact_Glow::IMPACT_DESC tImpactDesc = { };

	tImpactDesc.eELEMENT = ELEMENT_FIRE;
	tImpactDesc.pParent = (CGameObject*)this;
	tImpactDesc.iFanSize = 85;
	tImpactDesc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
	tImpactDesc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tImpactDesc.vScale = { 2.f, 3.2f, 1.f, 1.f };
	tImpactDesc.fRadius = 3.f;

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tImpactDesc)))
		return E_FAIL;

	/* Create Splash Effect */
	for (_int iSplash = 0; iSplash < NUM_SPLASH; ++iSplash)
	{
		CEffect::EFFECT_DESC tSplashDesc = {};
		tSplashDesc.fLifeTime = 1.f;
		tSplashDesc.vSpawnPos = XMLoadFloat4((_float4*)m_TrsFishMatrix.m[3]) +
			_vector{ m_pGameInstance->Get_Float_Random(-1.5f, 1.5f), m_pGameInstance->Get_Float_Random(-1.f, 1.f), m_pGameInstance->Get_Float_Random(-1.5f, 1.5f), 0.f };
		tSplashDesc.vScale = { 2.f, 1.f, 1.f, 0.f };

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Liquid_Splash", L"Layer_Effect", &tSplashDesc)))
			return E_FAIL;
	}

	/* Particle */
	wstring strParticleTag1 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_1");
	wstring strParticleTag2 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_2");
	wstring strParticleTag3 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_3");
	wstring strParticleTag4 = Get_ParticleTag(L"Effigy_PaperBurn_Eruption_1_4");

	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_1", strParticleTag1,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pBoneMatrix, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_2", strParticleTag2,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pBoneMatrix, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_3", strParticleTag3,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pBoneMatrix, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_Eruption_1_4", strParticleTag4,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, m_pBoneMatrix, m_pTransformCom);

	m_pGameInstance->Stop_Particle(strParticleTag1, 5.f);
	m_pGameInstance->Stop_Particle(strParticleTag2, 5.f);
	m_pGameInstance->Stop_Particle(strParticleTag3, 5.f);
	m_pGameInstance->Stop_Particle(strParticleTag4, 5.f);

	return S_OK;
}

HRESULT CFX_EffigyFireTrsFish::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrsFish::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_vFrame.x += fTimeDelta;
		m_vFrame.y = 10.f - (((m_vFrame.x - 0.5f) * 2.f + 1.f) * 5.f);

		if (m_bDissolveEffect == FALSE && m_vFrame.x > 0.5f)
		{
			m_bDissolveEffect = TRUE;
		}

		if (m_vFrame.x > 1.f)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;
		}
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrsFish::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_EffigyFireTrsFish::Render()
{
	static _int iIndex_1 = 0;

	if (FAILED(CFX_EffigyFireTrsFish::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_TrsFishMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex_1, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_vFrame.y, sizeof(_float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(m_bDissolveEffect == FALSE ? MESHFX_EFFIGYFIRE_IMPACT : MESHFX_EFFIGYFIRE_IMPACT_DISSOLVE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CFX_EffigyFireTrsFish* CFX_EffigyFireTrsFish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireTrsFish* pInstance = new CFX_EffigyFireTrsFish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireTrsFish"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireTrsFish::Clone(void* pArg)
{
	CFX_EffigyFireTrsFish* pInstance = new CFX_EffigyFireTrsFish(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireTrsFish"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireTrsFish::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
