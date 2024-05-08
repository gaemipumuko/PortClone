#include "stdafx.h"
#include "FX_EffigyFireExplode.h"

#include "Eff_Impact_Glow.h"
#include "DeciderObject.h"
#include "GameInstance.h"

#define EFF_SCALE 2.f

CFX_EffigyFireExplode::CFX_EffigyFireExplode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireExplode::CFX_EffigyFireExplode(const CFX_EffigyFireExplode& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireExplode::Add_Component(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TexFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Explode"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireExplode::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireExplode::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireExplode::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	EXPLODE_DESC* pExplodeDesc = static_cast<EXPLODE_DESC*>(pArg);

	m_fExplodeScale = max(pExplodeDesc->fExplodeExpand, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pExplodeDesc->vInitPosition);

	if(pExplodeDesc->fScale <= 0.f)
		m_pTransformCom->Set_Scale({ 30.f, 30.f, 1.f, 0.f });
	else
		m_pTransformCom->Set_Scale({ 30.f * pExplodeDesc->fScale, 30.f * pExplodeDesc->fScale, 1.f, 0.f });

	for (_int iSmoke = 0; iSmoke < NUM_SMOKE; ++iSmoke)
	{
		m_SmokeCorrection[iSmoke]	= { m_pGameInstance->Get_Float_Random(-0.5f, 0.5f), m_pGameInstance->Get_Float_Random(-0.5f, 0.5f), m_pGameInstance->Get_Float_Random(-0.5f, 0.5f), 0.f };
		m_DSmokeCorrection[iSmoke]	= { m_pGameInstance->Get_Float_Random(-0.5f, 0.5f), m_pGameInstance->Get_Float_Random(-0.5f, 0.5f), m_pGameInstance->Get_Float_Random(-0.5f, 0.5f), 0.f };
	}

	m_vUVRate = { 4.f, 4.f, 0.f, 0.f };
	m_pGameInstance->Play_Particle(L"Effigy_Explode_Sparks", Get_ParticleTag(L"Effigy_Explode_Sparks"), PARTICLE_EFFIGY_SPARK, TEX_PARTICLE_SPARK, nullptr, m_pTransformCom, 2.f);

	_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	_float fVolume = 1.f / (fLength * 0.25f);
	if (fVolume > 1.f)
		fVolume = 1.f;

	m_pGameInstance->PlaySoundW(L"EffigyFire_Chopping.mp3", fVolume);

	return S_OK;
}

HRESULT CFX_EffigyFireExplode::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireExplode::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_fFrame += fTimeDelta;

		/* BillBoard */
		m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

		/* Glow */
		if (m_bGlow == TRUE)
		{
			if (m_fFrame < 0.4f)
				XMStoreFloat4x4(&m_GlowMatrix, m_pTransformCom->Calculate_Scale(
					{ m_fFrame / 0.4f * EFF_SCALE * m_fExplodeScale, m_fFrame / 0.4f * EFF_SCALE * m_fExplodeScale, 1.f, 0.f }));
			else
				m_bGlow = FALSE;
		}

		/* Flare */
		if (m_bFlare == TRUE)
		{
			if (m_fFrame < 0.1f)
				XMStoreFloat4x4(&m_FlareMatrix, m_pTransformCom->Calculate_Scale(
					{ m_fFrame / 0.1f * EFF_SCALE * m_fExplodeScale, m_fFrame / 0.1f * EFF_SCALE * m_fExplodeScale, 1.f, 0.f }));
			else if (m_fFrame < 0.3f)
				XMStoreFloat4x4(&m_FlareMatrix, m_pTransformCom->Calculate_Scale(
					{ (1.f - (m_fFrame - 0.1f) / 0.3f) * EFF_SCALE * m_fExplodeScale, (1.f - (m_fFrame - 0.1f) / 0.3f) * EFF_SCALE * m_fExplodeScale, 1.f, 0.f }));
			else
				m_bFlare = FALSE;
		}

		/* Smoke */
		if (m_bSmoke == TRUE)
		{
			if (m_fFrame < 0.25f)
				XMStoreFloat4x4(&m_SmokeMatrix, m_pTransformCom->Calculate_Scale(
					{ m_fFrame / 0.4f * EFF_SCALE * m_fExplodeScale, m_fFrame / 0.4f * EFF_SCALE * m_fExplodeScale, 1.f, 0.f }));
			else if (m_fFrame > 0.5f)
				m_bSmoke = FALSE;

			_float fUV = static_cast<_float>(floor(m_fFrame / (0.5f / (m_vUVRate.x * m_vUVRate.y))));

			m_vUVRate.z = floor(fmod(fUV, m_vUVRate.x));
			m_vUVRate.w = floor(fUV / m_vUVRate.y);
		}

		if (m_bThorn == FALSE && m_fFrame > 0.1f)
		{
			m_bThorn = TRUE;

			/* °¡½Ã */
			CImpact_Glow::IMPACT_DESC	tImpactDesc = { };
			tImpactDesc.vScale = { 1.f * EFF_SCALE * m_fExplodeScale, 1.5f * EFF_SCALE * m_fExplodeScale, 1.f, 1.f };
			tImpactDesc.pParent = (CGameObject*)this;
			tImpactDesc.eEffectID = EFFECT_IMPACT_RADIAL;
			tImpactDesc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &tImpactDesc)))
				return E_FAIL;
		}

		/* Fin */
		if (m_fFrame > 0.5f)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;
		}
	}

	return S_OK;
}

HRESULT CFX_EffigyFireExplode::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_EffigyFireExplode::Render()
{
	static _int iTexSmoke = 0;
	static _int iTexDSmoke = 1;

	if (FAILED(CFX_EffigyFireExplode::Bind_ShaderResources()))
		return E_FAIL;

	/* Glow */
	if (m_bGlow == TRUE)
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_GlowMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(TEXFX_GLOW)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	/* Smoke */
	if (m_bSmoke == TRUE)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vUVRate, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iTexDSmoke, sizeof(_int))))
			return E_FAIL;

		for (_int iSmoke = 0; iSmoke < NUM_SMOKE; ++iSmoke)
		{
			_float4x4 SmokeMatrix = { };
			XMStoreFloat4x4(&SmokeMatrix, 
				XMMatrixRotationZ(XMConvertToRadians(15.f * iSmoke)) * XMMatrixTranslation(m_DSmokeCorrection[iSmoke].x, m_DSmokeCorrection[iSmoke].y, m_DSmokeCorrection[iSmoke].z) * XMLoadFloat4x4(&m_SmokeMatrix));

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", SmokeMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(TEXFX_SMOKE)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iTexSmoke, sizeof(_int))))
			return E_FAIL;

		for (_int iSmoke = 0; iSmoke < NUM_SMOKE; ++iSmoke)
		{
			_float4x4 SmokeMatrix = { };
			XMStoreFloat4x4(&SmokeMatrix, 
				XMMatrixRotationZ(XMConvertToRadians(15.f * iSmoke)) * XMMatrixTranslation(m_SmokeCorrection[iSmoke].x, m_SmokeCorrection[iSmoke].y, m_SmokeCorrection[iSmoke].z) * XMLoadFloat4x4(&m_SmokeMatrix));

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", SmokeMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(TEXFX_SMOKE)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	/* Flare */
	if (m_bFlare == TRUE)
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_FlareMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(TEXFX_FLARE)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

CFX_EffigyFireExplode* CFX_EffigyFireExplode::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireExplode* pInstance = new CFX_EffigyFireExplode(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireExplode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireExplode::Clone(void* pArg)
{
	CFX_EffigyFireExplode* pInstance = new CFX_EffigyFireExplode(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireExplode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireExplode::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
