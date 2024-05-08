#include "stdafx.h"
#include "Effect_Rush_Circle.h"

#include "GameInstance.h"

CEffect_Rush_Circle::CEffect_Rush_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Rush_Circle::CEffect_Rush_Circle(const CEffect_Rush_Circle& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Rush_Circle::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Rush_Circle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Rush_Circle::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Rush_Circle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Rush_Circle::Initialize_Clone(void* pArg)
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
		m_EffectDesc.vDir = pDesc->vDir;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
		m_pTransformCom->Look_At_Direction(m_EffectDesc.vDir);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Rush_Circle::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEffect_Rush_Circle::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	_vector vSelfScale = m_pTransformCom->Get_Scale();

	vSelfScale.m128_f32[0] += fTimeDelta * 10.f;
	vSelfScale.m128_f32[1] += fTimeDelta * 10.f;
	m_pTransformCom->Set_Scale(vSelfScale);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEffect_Rush_Circle::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CEffect_Rush_Circle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_RUSH_CIRCLE)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEffect_Rush_Circle* CEffect_Rush_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Rush_Circle* pInstance = new CEffect_Rush_Circle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEffect_Rush_Circle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Rush_Circle::Clone(void* pArg)
{
	CEffect_Rush_Circle* pInstance = new CEffect_Rush_Circle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEffect_Rush_Circle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Rush_Circle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}