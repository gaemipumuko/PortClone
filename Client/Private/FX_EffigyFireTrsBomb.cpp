#include "stdafx.h"
#include "FX_EffigyFireTrsBomb.h"

#include "Eff_Impact_Glow.h"
#include "FX_EffigyFireExplode.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFireTrsBomb::CFX_EffigyFireTrsBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireTrsBomb::CFX_EffigyFireTrsBomb(const CFX_EffigyFireTrsBomb& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireTrsBomb::Add_Component(void* pArg)
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	TRSBOMB_DESC* pTrsBombDesc = static_cast<TRSBOMB_DESC*>(pArg);

	m_pTransformCom = pTrsBombDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	m_pBoneMatrix = pTrsBombDesc->pBoneMatrix;

	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_vFrame.x += fTimeDelta;

		if (m_bTrsBombEff == FALSE)
		{
			_matrix GlowMatrix = XMLoadFloat4x4(m_pBoneMatrix) * m_pTransformCom->Get_WorldMatrix();

			CImpact_Glow::IMPACT_DESC tImpactDesc = { };

			tImpactDesc.eELEMENT = ELEMENT_FIRE;
			tImpactDesc.pParent = (CGameObject*)this;
			tImpactDesc.iFanSize = 180;
			tImpactDesc.iNumSpike = 30;
			tImpactDesc.eEffectID = EFFECT_IMPACT_PLANE;
			tImpactDesc.vSpawnPos = GlowMatrix.r[3];
			tImpactDesc.vScale = { 2.f, 3.2f, 2.f, 1.f };
			tImpactDesc.fRadius = 0.1f;

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &tImpactDesc)))
				return E_FAIL;

			/* Explode */
			CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
			tExplodeDesc.fExplodeExpand = 3.f;
			tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tExplodeDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION)
				+ _vector{ 0.f, 2.f, 0.f, 0.f };

			m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);

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

			m_bTrsBombEff = TRUE;
		}

		if (m_vFrame.x > 1.5f)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;
		}
	}

	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CFX_EffigyFireTrsBomb::Render()
{
	if (FAILED(CFX_EffigyFireTrsBomb::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CFX_EffigyFireTrsBomb* CFX_EffigyFireTrsBomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireTrsBomb* pInstance = new CFX_EffigyFireTrsBomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireTrsBomb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireTrsBomb::Clone(void* pArg)
{
	CFX_EffigyFireTrsBomb* pInstance = new CFX_EffigyFireTrsBomb(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireTrsBomb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireTrsBomb::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
