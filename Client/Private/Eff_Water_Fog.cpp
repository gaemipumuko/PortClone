#include "stdafx.h"
#include "Eff_Water_Fog.h"

#include "GameInstance.h"
#include "Player.h"

CWater_Fog::CWater_Fog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CWater_Fog::CWater_Fog(const CWater_Fog& rhs)
	: CEffect(rhs)
{
}

HRESULT CWater_Fog::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Water_Fog"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Barbara_Color_Ramp"),
		TEXT("Com_Texture_Ramp"), (CComponent**)&m_pRampTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater_Fog::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater_Fog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWater_Fog::Initialize_Clone(void* pArg)
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

	return S_OK;
}

HRESULT CWater_Fog::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CWater_Fog::Tick(_float fTimeDelta)
{	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
		Set_Dead(TRUE);
	}

	m_fTimeAcc += fTimeDelta;
	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CWater_Fog::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CWater_Fog::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pRampTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_WATER_FOG)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

	return S_OK;
}

CWater_Fog* CWater_Fog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWater_Fog* pInstance = new CWater_Fog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CWater_Fog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWater_Fog::Clone(void* pArg)
{
	CWater_Fog* pInstance = new CWater_Fog(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CWater_Fog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWater_Fog::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
		Safe_Release(m_pRampTextureCom);
	}
}