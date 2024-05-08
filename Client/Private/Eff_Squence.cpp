#include "stdafx.h"
#include "Eff_Squence.h"

#include "Camera.h"
#include "GameInstance.h"

CEff_Squence::CEff_Squence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Squence::CEff_Squence(const CEff_Squence& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Squence::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Squence"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEff_Squence::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Squence::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Squence::Initialize_Clone(void* pArg)
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

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
	}
	else
		return E_FAIL;

	m_EffectDesc.fLifeTime = 5.f;

	m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(180.f));


	return S_OK;
}

HRESULT CEff_Squence::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Squence::Tick(_float fTimeDelta)
{
	m_EffectDesc.fLifeTime -= fTimeDelta;
	if (m_EffectDesc.fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}


	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 0.03f)
	{
		m_iCurrentFrameCol++;
		_vector vScale = m_pTransformCom->Get_Scale();
		vScale.m128_f32[0] *= 1.1f;
		vScale.m128_f32[1] *= 1.1f;
		m_pTransformCom->Set_Scale(vScale);
		if (m_iCurrentFrameCol == m_iFrameCol)
		{
			m_iCurrentFrameCol = 1;
			m_iCurrentFrameRow++;
			if (m_iCurrentFrameRow == m_iFrameRow+1)
			{
				Set_Destroy(TRUE);
			}
		}
		m_fTimeAcc = 0.f;
	}
	m_vUVPos = { 1.f / m_iFrameCol * static_cast<_float>(m_iCurrentFrameCol - 1), 1.f / m_iFrameRow * static_cast<_float>(m_iCurrentFrameRow - 1) };

	m_vUVScale = { 1.f / m_iFrameCol ,1.f / m_iFrameRow };
	//m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Squence::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Squence::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//_vector vSelfScale = m_pTransformCom->Get_Scale();
	//vSelfScale.m128_f32[0] += 0.01f;
	//vSelfScale.m128_f32[1] += 0.01f;
	//m_pTransformCom->Set_Scale(vSelfScale);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate2", &m_vUVPos, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVScaleRate", &m_vUVScale, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_SQUENCE)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEff_Squence* CEff_Squence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Squence* pInstance = new CEff_Squence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Squence"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Squence::Clone(void* pArg)
{
	CEff_Squence* pInstance = new CEff_Squence(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Squence"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Squence::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	}
}