#include "stdafx.h"
#include "Effect_Flame.h"

#include "GameInstance.h"

#include "Claymore_Wolfmound.h"

CEffect_Flame::CEffect_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Flame::CEffect_Flame(const CEffect_Flame& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Flame::Add_Component(void* pArg)
{
	__super::Add_Component(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Flame"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Flame::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrameTime", &m_fFrameTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Flame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Flame::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.fLifeTime = pDesc->fLifeTime;	
		m_EffectDesc.eParent = pDesc->eParent;
		m_EffectDesc.pParent = pDesc->pParent;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
	}

	//if(m_EffectDesc.eEffectID == EFFECT_FLAME)
	//{
	//	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(3.f, 1.f, 3.f, 1.f));
	//}
	//else
	//{
	//	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 1.f, 3.f, 1.f));
	//}

	return S_OK;
}

HRESULT CEffect_Flame::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEffect_Flame::Tick(_float fTimeDelta)
{
	if (m_fFrameTime < 1000.f)
		m_fFrameTime += fTimeDelta;
	else
		m_fFrameTime -= 1000.f;

	//if (0.f < m_fLifeTime)
	//{
	//	m_fTimeDelta += fTimeDelta;
	//	if (m_fTimeDelta >= m_fLifeTime)
	//		Set_Dead();
	//}

	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);
	
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEffect_Flame::LateTick(_float fTimeDelta)
{
	if (nullptr != m_EffectDesc.pParent)
	{
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() * static_cast<CClaymore_Wolfmound*>(m_EffectDesc.pParent)->Get_CombinedMatrix());
	}
	else
	{
		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

		m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CEffect_Flame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXEFF_FLAME)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEffect_Flame* CEffect_Flame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Flame* pInstance = new CEffect_Flame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEffect_Flame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Flame::Clone(void* pArg)
{
	CEffect_Flame* pInstance = new CEffect_Flame(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEffect_Flame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Flame::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}