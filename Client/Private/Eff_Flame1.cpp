#include "stdafx.h"
#include "Eff_Flame1.h"

#include "GameInstance.h"

CEff_Flame1::CEff_Flame1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Flame1::CEff_Flame1(const CEff_Flame1& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Flame1::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Fire_Diff"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fire_Ramp"),
		TEXT("Com_Texture_Ramp"), (CComponent**)&m_pRampTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Flame1::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Flame1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Flame1::Initialize_Clone(void* pArg)
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

		if (m_EffectDesc.eEffectID == EFFECT_FLAME1)
		{
			m_vUVPos = { 0.f,0.f };
		}
		else
		{
			m_vUVPos = { 0.5f,0.f };
		}
	}
	else
		return E_FAIL;



	m_pGameInstance->Play_Particle(TEXT("Heat_Smoke"), Get_ParticleTag(TEXT("Heat_Smoke")), PARTICLE_HEAT_SMOKE, TEX_PARTICLE_HEAT_SMOKE, nullptr, m_pTransformCom, 1.f);

	
	//플레이어 시계 돌리기 로직

	CTransform* pPlayerTransform
		= static_cast<CTransform*>(m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vFromPlayer = vPosition - vPlayerPos;
	vFromPlayer = XMVector3Normalize(vFromPlayer);
	_float fRadian = acos(XMVectorGetX(XMVector3Dot(vFromPlayer, XMVectorSet(1.f, 0.f, 0.f, 0.f))));
	_float Degree = XMConvertToDegrees(fRadian) + 270.f;
	m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(Degree));
	m_fLifeTime = 1.f;


	return S_OK;
}

HRESULT CEff_Flame1::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Flame1::Tick(_float fTimeDelta)
{	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
		Set_Dead(TRUE);
	}

	m_fTimeAcc += fTimeDelta;
	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	
	//Player에서 멀어지기

	CTransform* pPlayerTransform
		= static_cast<CTransform*>(m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vFromPlayer = vPosition - vPlayerPos;
	//vFromPlayer.m128_f32[2] = 0.f;
	vFromPlayer = XMVector3Normalize(vFromPlayer);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition + vFromPlayer*0.01f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Flame1::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_EFFECT, this);

	return S_OK;
}

HRESULT CEff_Flame1::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_vector vSelfScale = m_pTransformCom->Get_Scale();
	//vSelfScale.m128_f32[0] += m_fTimeAcc/10.f;
	//vSelfScale.m128_f32[1] += m_fTimeAcc/10.f;
	m_pTransformCom->Set_Scale(vSelfScale);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",3)))
		return E_FAIL;

	if (FAILED(m_pRampTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 5)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vUVPos, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_FLAME1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEff_Flame1* CEff_Flame1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Flame1* pInstance = new CEff_Flame1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Flame1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Flame1::Clone(void* pArg)
{
	CEff_Flame1* pInstance = new CEff_Flame1(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Flame1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Flame1::Free()
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