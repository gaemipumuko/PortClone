#include "stdafx.h"
#include "Eff_Barbara_Normal_Atk.h"

#include "GameInstance.h"
#include "Player.h"
#include "Eff_Impact_Glow.h"

CEff_Barbara_Normal_Atk::CEff_Barbara_Normal_Atk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Barbara_Normal_Atk::CEff_Barbara_Normal_Atk(const CEff_Barbara_Normal_Atk& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Barbara_Normal_Atk::Add_Component(void* pArg)
{
	__super::Add_Component(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Ground_Stomp"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Barbara_Glow_Star"),
		TEXT("Com_Texture_Ramp"), (CComponent**)&m_pStarTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.fLifeTime = pDesc->fLifeTime;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

	}
	else
		return E_FAIL;

	m_fLifeTime = 2.f;

	m_vStarScale = {0.3f,0.3f};
	m_vCircleScale = {0.2f,0.2f};

	m_pGameInstance->Play_Particle(TEXT("Circle_Star_Small"), Get_ParticleTag(TEXT("Circle_Star_Small")), PARTICLE_STAR_COLOR, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star_Small1"), Get_ParticleTag(TEXT("Circle_Star_Small1")), PARTICLE_STAR_COLOR, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star_Small2"), Get_ParticleTag(TEXT("Circle_Star_Small2")), PARTICLE_STAR_COLOR, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star_Small3"), Get_ParticleTag(TEXT("Circle_Star_Small3")), PARTICLE_STAR_COLOR, TEX_PARTICLE_STAR, nullptr, m_pTransformCom, 0.5f);
	m_pGameInstance->Play_Particle(TEXT("Circle_Star_Small"), Get_ParticleTag(TEXT("Circle_Star_Small")), PARTICLE_DEFAULT, TEX_PARTICLE_BUBBLE, nullptr, m_pTransformCom, 0.5f);

	CImpact_Glow::IMPACT_DESC descc = {};
	descc.eEffectID = EFFECT_IMPACT_RADIAL;
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.pParent = (CGameObject*)this;
	descc.vSpawnPos = vSelfPos;
	descc.vScale = { 1.f, 1.5f, 1.f, 1.f };
	descc.eELEMENT = ELEMENT_WATER;


	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::Tick(_float fTimeDelta)
{	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Dead(TRUE);
	}

	m_fTimeAcc += fTimeDelta;

	if (m_vStarScale.x > 0.f)
	{
		m_vStarScale.x -= m_fTimeAcc / 10.f;
		m_vStarScale.y -= m_fTimeAcc / 10.f;
	}

	if (m_vCircleScale.x < 1.f)
	{
		m_vCircleScale.x += m_fTimeAcc / 2.f;
		m_vCircleScale.y += m_fTimeAcc / 2.f;
	}


	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Atk::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
		
		_vector vCircle_Scale = { m_vCircleScale.x, m_vCircleScale.y, 1.f, 0.f };
		
		m_pTransformCom->Set_Scale(vCircle_Scale);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		_float fCircleFadeTime = m_fTimeAcc*2.f;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &fCircleFadeTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_BARBARA_NORMAL_ATK_RING)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		_vector vStar_Scale = { m_vStarScale.x, m_vStarScale.y, 1.f, 0.f };

		m_pTransformCom->Set_Scale(vStar_Scale);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pStarTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_BARBARA_NORMAL_ATK_STAR)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		m_pTransformCom->Set_WorldFloat4x4(m_CombinedWorldMatrix);

	return S_OK;
}

CEff_Barbara_Normal_Atk* CEff_Barbara_Normal_Atk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Barbara_Normal_Atk* pInstance = new CEff_Barbara_Normal_Atk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Barbara_Normal_Atk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Barbara_Normal_Atk::Clone(void* pArg)
{
	CEff_Barbara_Normal_Atk* pInstance = new CEff_Barbara_Normal_Atk(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Barbara_Normal_Atk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Barbara_Normal_Atk::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
		Safe_Release(m_pStarTextureCom);
	}
}