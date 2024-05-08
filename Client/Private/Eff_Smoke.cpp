#include "stdafx.h"
#include "Eff_Smoke.h"

#include "GameInstance.h"

CEff_Smoke::CEff_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Smoke::CEff_Smoke(const CEff_Smoke& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Smoke::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEff_Smoke::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_Smoke::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Smoke::Initialize_Clone(void* pArg)
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
		m_EffectDesc.vScale = pDesc->vScale;

		m_fLifeTime = m_EffectDesc.fLifeTime;

		_float fRand = m_pGameInstance->Get_Float_Random(0.f, 360.f);
		m_pTransformCom->Rotation({ 0.f, 0.f, 1.f, 0.f }, fRand);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

		m_eSmokeType = (SMOKE_TYPE)m_pGameInstance->Get_Int_Random(0, 3);
	}
	else
		return E_FAIL;

	//m_pGameInstance->Play_Particle(TEXT("FIRE"), Get_ParticleTag(TEXT("FIRE")), PARTICLE_FEATHERA, TEX_PARTICLE_FIRE, nullptr, m_pTransformCom, 1.f);


	//CTransform* pPlayerTransform
	//	= static_cast<CTransform*>(m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Player"), TEXT("Com_Transform")));

	//_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vFromPlayer = vPosition - vPlayerPos;
	////vFromPlayer.m128_f32[1] = 0.f;
	//vFromPlayer = XMVector3Normalize(vFromPlayer);
	//_float fRadian = acos(XMVectorGetX(XMVector3Dot(vFromPlayer, XMVectorSet(1.f, 0.f, 0.f, 0.f))));
	//_float Degree = XMConvertToDegrees(fRadian) + 270.f;
	//m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(Degree));
	m_fLifeTime = 2.f;

	//m_pGameInstance->Play_Particle(TEXT("Smoke0"), Get_ParticleTag(TEXT("Smoke0")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	//m_pGameInstance->Play_Particle(TEXT("Smoke1"), Get_ParticleTag(TEXT("Smoke1")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	//m_pGameInstance->Play_Particle(TEXT("Smoke2"), Get_ParticleTag(TEXT("Smoke2")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	//m_pGameInstance->Play_Particle(TEXT("Smoke3"), Get_ParticleTag(TEXT("Smoke3")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);

	//m_pGameInstance->Play_Particle(TEXT("FIRE"), Get_ParticleTag(TEXT("FIRE")), PARTICLE_FEATHERA, TEX_PARTICLE_FIRE, nullptr, m_pTransformCom, 1.f);

	m_pGameInstance->Play_Particle(TEXT("Dust0"), Get_ParticleTag(TEXT("Dust0")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("Dust1"), Get_ParticleTag(TEXT("Dust1")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("Dust2"), Get_ParticleTag(TEXT("Dust2")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);
	m_pGameInstance->Play_Particle(TEXT("Dust3"), Get_ParticleTag(TEXT("Dust3")), PARTICLE_DUST, TEX_PARTICLE_DUST, nullptr, m_pTransformCom, 1.f);

	return S_OK;
}

HRESULT CEff_Smoke::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_Smoke::Tick(_float fTimeDelta)
{	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
		Set_Dead(TRUE);
	}
	_vector vSelfScale = m_EffectDesc.vScale;
	vSelfScale.m128_f32[0] += m_fTimeAcc;
	vSelfScale.m128_f32[1] += m_fTimeAcc;
	m_pTransformCom->Set_Scale(vSelfScale);

	m_fTimeAcc += fTimeDelta;

	if (m_pGameInstance->Key_Pressing('O'))
	{
		m_fTimeAcc = 0.f;
	}

	if (m_pGameInstance->Key_Down('P'))
	{
		switch (m_eSmokeType)
		{
		case Client::CEff_Smoke::SMOKE_TYPE_ONE:
			m_eSmokeType = SMOKE_TYPE_TWO;
			m_fPos = { 0.5f,0.f };
			break;
		case Client::CEff_Smoke::SMOKE_TYPE_TWO:
			m_eSmokeType = SMOKE_TYPE_THREE;
			m_fPos = { 0.f,0.5f };
			break;
		case Client::CEff_Smoke::SMOKE_TYPE_THREE:
			m_eSmokeType = SMOKE_TYPE_FOUR;
			m_fPos = { 0.5f,0.5f };
			break;
		case Client::CEff_Smoke::SMOKE_TYPE_FOUR:
			m_eSmokeType = SMOKE_TYPE_ONE;
			m_fPos = { 0.f,0.f };
			break;
		case Client::CEff_Smoke::SMOKE_TYPE_END:
			m_eSmokeType = SMOKE_TYPE_ONE;
			m_fPos = { 0.f,0.f };
			break;
		default:
			break;
		}
	}

	//m_fTimeAcc += fTimeDelta;
	//m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;


	//CTransform* pPlayerTransform
	//	= static_cast<CTransform*>(m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Player"), TEXT("Com_Transform")));

	//_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vFromPlayer = vPosition - vPlayerPos;
	////vFromPlayer.m128_f32[2] = 0.f;
	//vFromPlayer = XMVector3Normalize(vFromPlayer);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition + vFromPlayer*0.01f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Smoke::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CEff_Smoke::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture",0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 2)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVPos", &m_fPos, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_SMOKE)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEff_Smoke* CEff_Smoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Smoke* pInstance = new CEff_Smoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Smoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Smoke::Clone(void* pArg)
{
	CEff_Smoke* pInstance = new CEff_Smoke(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Smoke"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Smoke::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	}
}