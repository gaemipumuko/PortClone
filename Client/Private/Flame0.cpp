#include "stdafx.h"
#include "Flame0.h"

#include "GameInstance.h"
#include "Player.h"

CFlame0::CFlame0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CFlame0::CFlame0(const CFlame0& rhs)
	: CEffect(rhs)
{
}

HRESULT CFlame0::Add_Component(void* pArg)
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

	/* For.Com_Texture_Ramp */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fire_Ramp"),
		TEXT("Com_Texture_Ramp"), (CComponent**)&m_pRampTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlame0::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlame0::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlame0::Initialize_Clone(void* pArg)
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

	if (m_EffectDesc.eEffectID == EFFECT_FLAME2)
	{
		m_vUVPos = { 0.5f,0.f };
	}
	else
	{
		m_pGameInstance->Play_Particle(TEXT("Heat_Smoke"), Get_ParticleTag(TEXT("Heat_Smoke")), PARTICLE_HEAT_SMOKE, TEX_PARTICLE_HEAT_SMOKE, nullptr, m_pTransformCom, 1.f);
		m_vUVPos = { 0.f,0.f };
	}

	m_pGameInstance->Play_Particle(TEXT("FIRE"), Get_ParticleTag(TEXT("FIRE")), PARTICLE_FEATHERA, TEX_PARTICLE_FIRE, nullptr, m_pTransformCom, 1.f);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vFromPlayer = vPosition - vPlayerPos;
	//vFromPlayer.m128_f32[1] = 0.f;
	vFromPlayer = XMVector3Normalize(vFromPlayer);
	_float fRadian = acos(XMVectorGetX(XMVector3Dot(vFromPlayer, XMVectorSet(1.f, 0.f, 0.f, 0.f))));
	_float Degree = XMConvertToDegrees(fRadian) + 270.f;
	m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(Degree));
	m_fLifeTime = 1.f;

	return S_OK;
}

HRESULT CFlame0::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CFlame0::Tick(_float fTimeDelta)
{	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
		Set_Dead(TRUE);
	}

	m_fTimeAcc += fTimeDelta;
	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vFromPlayer = vPosition - vPlayerPos;
	//vFromPlayer.m128_f32[2] = 0.f;
	vFromPlayer = XMVector3Normalize(vFromPlayer);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition + vFromPlayer*0.015f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CFlame0::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_EFFECT, this);

	return S_OK;
}

HRESULT CFlame0::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_EffectDesc.eEffectID == EFFECT_FLAME0)
	{

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
			return E_FAIL;

		if (FAILED(m_pRampTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 5)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_FLAME0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
			return E_FAIL;

		if (FAILED(m_pRampTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 5)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vUVPos, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_FLAME1)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	return S_OK;
}

CFlame0* CFlame0::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlame0* pInstance = new CFlame0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFlame0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFlame0::Clone(void* pArg)
{
	CFlame0* pInstance = new CFlame0(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFlame0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlame0::Free()
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