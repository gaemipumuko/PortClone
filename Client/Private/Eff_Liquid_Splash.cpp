#include "stdafx.h"
#include "Eff_Liquid_Splash.h"

#include "GameInstance.h"

CEff_Liquid_Splash::CEff_Liquid_Splash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Liquid_Splash::CEff_Liquid_Splash(const CEff_Liquid_Splash& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Liquid_Splash::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Liquid_Splash"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Liquid_Splash::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Liquid_Splash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Liquid_Splash::Initialize_Clone(void* pArg)
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

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
	}
	else
		return E_FAIL;

	m_EffectDesc.fLifeTime = 3.f;

	if (m_EffectDesc.eEffectID == EFFECT_IMPACT_WATER_SPLASH)
	{
		m_vColor = { 0.75f,1.f,1.f, 1.f };
	}
	else
	{
		m_vColor = { 255.f / 255.f, 216.f / 255.f, 80.f / 255.f, 1.f };
	}

	m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSplashPos;
	_float4x4 SplashMat = { };
	

	for (_int i = 0; i < m_iNumSplash; i++)
	{
		m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(90.f * static_cast<_float>(i)));
		_float fRightRand = m_pGameInstance->Get_Float_Random(-1.f, 1.f);
		_float fUpRand = m_pGameInstance->Get_Float_Random(-0.2f, 0.2f);
		_float fRand = m_pGameInstance->Get_Float_Random(-1.f, 1.f);
		vSplashPos = vPos + vRight* fRightRand + vLook * fRand + vUp * fUpRand;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSplashPos);
		m_pTransformCom->Set_Scale({2.f,2.f,1.f,1.f});
		SplashMat = m_pTransformCom->Get_WorldFloat4x4();
		m_SplashMat.push_back(SplashMat);
	}

	m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	return S_OK;
}

HRESULT CEff_Liquid_Splash::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Liquid_Splash::Tick(_float fTimeDelta)
{
	m_EffectDesc.fLifeTime -= fTimeDelta;
	if (m_EffectDesc.fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}

	m_fTimeAcc += fTimeDelta;
	m_fFrameRate += fTimeDelta * 4.f;

	if (m_fTimeAcc < 0.1f)
	{

	_vector vScale = m_EffectDesc.vScale;
	vScale.m128_f32[0] += m_fTimeAcc * 10.f;
	vScale.m128_f32[1] += m_fTimeAcc * 10.f;
	m_pTransformCom->Set_Scale(vScale);
	}
	if (m_fTimeAcc > 0.1f)
	{
		_vector vScale = m_pTransformCom->Get_Scale();
		vScale.m128_f32[0] += fTimeDelta*2.f;
		vScale.m128_f32[1] += fTimeDelta*2.f;
		m_pTransformCom->Set_Scale(vScale);
		m_pTransformCom->Go_Down(fTimeDelta/6.f);


	}


	if (m_fFrameRate >= 0.03f && m_bTerminated == false)
	{
		m_iCurrentFrameCol++;
		//_vector vScale = m_pTransformCom->Get_Scale();
		//vScale.m128_f32[0] *= 1.1f;
		//vScale.m128_f32[1] *= 1.1f;
		//m_pTransformCom->Set_Scale(vScale);
		if (m_iCurrentFrameCol == m_iFrameCol)
		{
			m_iCurrentFrameCol = 1;
			m_iCurrentFrameRow++;
			if (m_iCurrentFrameRow == m_iFrameRow + 1)
			{
				m_bTerminated = true;
			}
		}
		m_fFrameRate = 0.f;
	}
	m_vUVPos = { 1.f / m_iFrameCol * static_cast<_float>(m_iCurrentFrameCol - 1), 1.f / m_iFrameRow * static_cast<_float>(m_iCurrentFrameRow - 1) };

	m_vUVScale = { 1.f / m_iFrameCol ,1.f / m_iFrameRow };


	return S_OK;
}

HRESULT CEff_Liquid_Splash::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Liquid_Splash::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_LIQUID_SPLASH)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//SEQUENCE

	if (m_bTerminated == false)
	{

		for (_int i = 0; i < m_iNumSplash; i++)
		{

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_SplashMat[i])))
				return E_FAIL;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 1)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate2", &m_vUVPos, sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVScaleRate", &m_vUVScale, sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_LIQUID_SPRAY)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}


	return S_OK;
}

CEff_Liquid_Splash* CEff_Liquid_Splash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Liquid_Splash* pInstance = new CEff_Liquid_Splash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Liquid_Splash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Liquid_Splash::Clone(void* pArg)
{
	CEff_Liquid_Splash* pInstance = new CEff_Liquid_Splash(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Liquid_Splash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Liquid_Splash::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	}
}