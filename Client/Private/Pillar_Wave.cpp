#include "stdafx.h"
#include "Pillar_Wave.h"

#include "GameInstance.h"

CPillar_Wave::CPillar_Wave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CPillar_Wave::CPillar_Wave(const CPillar_Wave& rhs)
	: CEffect(rhs)
{
}

HRESULT CPillar_Wave::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pillar_Wave"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPillar_Wave::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPillar_Wave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPillar_Wave::Initialize_Clone(void* pArg)
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

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	}
	else
		return E_FAIL;

	m_EffectDesc.fLifeTime = 2.f;

	m_pTransformCom->Look_At_Direction(m_EffectDesc.vDir);
	m_pTransformCom->RotationFromCurrentState(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));


	return S_OK;
}

HRESULT CPillar_Wave::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CPillar_Wave::Tick(_float fTimeDelta)
{
	m_EffectDesc.fLifeTime -= fTimeDelta;
	if (m_EffectDesc.fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}

	//m_pTransformCom->Rotation({ 1.f,0.f,0.f,0.f }, XMConvertToRadians(90.f));

	m_fTimeAcc += fTimeDelta;

	//ũ�� Ŀ����
	if (m_fTimeAcc <= 1.f)
	{
		_float fSize = sin(XMConvertToRadians(m_fTimeAcc * 90.f)) * m_EffectDesc.vScale.m128_f32[0];
		m_pTransformCom->Set_Scale(XMVectorSet(fSize, fSize, fSize, 1.f));
	}


	return S_OK;
}

HRESULT CPillar_Wave::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CPillar_Wave::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_LIGHTWAVE)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CPillar_Wave* CPillar_Wave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPillar_Wave* pInstance = new CPillar_Wave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPillar_Wave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPillar_Wave::Clone(void* pArg)
{
	CPillar_Wave* pInstance = new CPillar_Wave(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPillar_Wave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPillar_Wave::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	}
}