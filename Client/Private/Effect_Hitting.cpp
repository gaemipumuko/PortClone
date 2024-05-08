#include "stdafx.h"

#include "Eff_Impact_Glow.h"
#include "Effect_Hitting.h"

#include "GameInstance.h"

CEffect_Hitting::CEffect_Hitting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Hitting::CEffect_Hitting(const CEffect_Hitting& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Hitting::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Hitting"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Hitting::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Hitting::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Hitting::Initialize_Clone(void* pArg)
{
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

	CImpact_Glow::IMPACT_DESC descc = {};
	descc.eEffectID = EFFECT_IMPACT_RADIAL;
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.pParent = (CGameObject*)this;
	descc.vSpawnPos = vSelfPos;
	descc.vScale = { 1.f, 1.5f, 1.f, 1.f };

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_Effect", &descc)))
		return E_FAIL;

	m_fLifeTime = 2.f;

	return S_OK;
}

HRESULT CEffect_Hitting::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEffect_Hitting::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta;
	_vector vSelfScale = m_pTransformCom->Get_Scale();

	if (vSelfScale.m128_f32[0] >= 3.f)		// 일정 수치까지 크기를 줄이기
	{
		vSelfScale.m128_f32[0] -= fTimeDelta * 10.f;
		vSelfScale.m128_f32[1] -= fTimeDelta * 10.f;
		m_pTransformCom->Set_Scale(vSelfScale);
	}
	
	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;
	//m_fColorRate1 += fTimeDelta * 2.5f;
	m_fColorRate2 += fTimeDelta * 3.f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEffect_Hitting::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEffect_Hitting::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_vector vSelfScale = m_pTransformCom->Get_Scale();
	vSelfScale.m128_f32[0] = 1.f;
	vSelfScale.m128_f32[1] = 1.f;
	m_pTransformCom->Set_Scale(vSelfScale);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fColorRate2, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_HITTING_2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// -----------------------------------------------------

	//vSelfScale = m_pTransformCom->Get_Scale();
	//vSelfScale.m128_f32[0] = 2.5f;
	//vSelfScale.m128_f32[1] = 2.5f;
	//m_pTransformCom->Set_Scale(vSelfScale);

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
	//	return E_FAIL;

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fColorRate1, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Begin(VTXTEX_HITTING_1)))
	//	return E_FAIL;

	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;

	// -----------------------------------------------------

	m_vScale.m128_f32[1] += 0.05f;
	m_vScale.m128_f32[0] += 0.05f;
	m_pTransformCom->Set_Scale(m_vScale);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fColorRate0, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_HITTING_0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEffect_Hitting* CEffect_Hitting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Hitting* pInstance = new CEffect_Hitting(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEffect_Hitting"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Hitting::Clone(void* pArg)
{
	CEffect_Hitting* pInstance = new CEffect_Hitting(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEffect_Hitting"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Hitting::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}