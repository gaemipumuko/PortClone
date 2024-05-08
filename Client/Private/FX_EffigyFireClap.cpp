#include "stdafx.h"
#include "FX_EffigyFireClap.h"

#include "Eff_Impact_Glow.h"
#include "FX_EffigyFireExplode.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFireClap::CFX_EffigyFireClap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireClap::CFX_EffigyFireClap(const CFX_EffigyFireClap& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireClap::Add_Component(void* pArg)
{
	return S_OK;
}

HRESULT CFX_EffigyFireClap::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CFX_EffigyFireClap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireClap::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	CLAP_DESC* pClapDesc = static_cast<CLAP_DESC*>(pArg);

	m_pTransformCom = pClapDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	m_pBoneMatrix = pClapDesc->pBoneMatrix;

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

	/* Explode */
	CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
	tExplodeDesc.fExplodeExpand	= 3.f;
	tExplodeDesc.ePosType		= CAssetFX::ASFX_DESC::POS_FIX;
	tExplodeDesc.vInitPosition	= m_pTransformCom->Get_State(CTransform::STATE_POSITION) 
		+ m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 2.f
		+ _vector{ 0.f, 2.f, 0.f, 0.f };

	m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

	/* Thorn */
	CImpact_Glow::IMPACT_DESC tThornDesc = { };
	tThornDesc.eEffectID	= EFFECT_IMPACT_PLANE;
	tThornDesc.pParent		= (CGameObject*)this;
	tThornDesc.eELEMENT		= ELEMENT_FIRE;

	tThornDesc.iNumSpike	= { 20 };
	tThornDesc.iFanSize		= { 180 };
	tThornDesc.fRadius		= { 3.f };
	tThornDesc.vScale		= _vector{ 1.f, 1.6f, 1.f, 1.f } * 3.f;

	tThornDesc.iRotAxisY	= 0;
	tThornDesc.iRotation	= 1;

	tThornDesc.vSpawnPos	= m_pTransformCom->Get_State(CTransform::STATE_POSITION) 
		+ m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 3.f
		+ _vector{ 0.f, 2.f, 0.f, 0.f };

	tThornDesc.ePlaneDir = CImpact_Glow::EFF_PLANE_LEFT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &tThornDesc)))
		return E_FAIL;

	tThornDesc.ePlaneDir = CImpact_Glow::EFF_PLANE_RIGHT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &tThornDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireClap::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireClap::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_vFrame.x += fTimeDelta;

		if (m_vFrame.x > 0.5f)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;
		}
	}

	return S_OK;
}

HRESULT CFX_EffigyFireClap::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CFX_EffigyFireClap::Render()
{
	if (FAILED(CFX_EffigyFireClap::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CFX_EffigyFireClap* CFX_EffigyFireClap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireClap* pInstance = new CFX_EffigyFireClap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireClap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireClap::Clone(void* pArg)
{
	CFX_EffigyFireClap* pInstance = new CFX_EffigyFireClap(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireClap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireClap::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
