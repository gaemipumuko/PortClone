#include "stdafx.h"
#include "Raiden_Light.h"
#include "Player.h"
#include "GameInstance.h"

CRaiden_Light::CRaiden_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CRaiden_Light::CRaiden_Light(const CRaiden_Light& rhs)
	: CEffect(rhs)
{
}

HRESULT CRaiden_Light::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Raiden_Light"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRaiden_Light::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRaiden_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRaiden_Light::Initialize_Clone(void* pArg)
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
		m_EffectDesc.fSpeed = pDesc->fSpeed;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CRaiden_Light::PreviousTick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	
	_vector vScale = m_pTransformCom->Get_Scale();
	vScale.m128_f32[1] = vScale.m128_f32[1] - fTimeDelta * 5.f;
	if (vScale.m128_f32[1] < 0.2f)
		vScale.m128_f32[1] = 0.2f;
	m_pTransformCom->Set_Scale(vScale);
	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
	
	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.m128_f32[1] = 0.f; XMVector3Normalize(vPlayerLook);
	vPlayerPos.m128_f32[1] += 1.5f;
	vPlayerPos += vPlayerLook;
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = XMVector3Length(vPlayerPos - vPos).m128_f32[0];
	
	if(fDistance > 0.5f)
		m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 5.f);
	
	if (m_fLifeTime > 3.f)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook.m128_f32[1] = 0.f; XMVector3Normalize(vLook);
		vPos += vLook * 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	
	if (m_EffectDesc.vDir.m128_f32[0] != 0.f && m_EffectDesc.vDir.m128_f32[1] != 0.f && m_EffectDesc.vDir.m128_f32[2] != 0.f)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_EffectDesc.vDir * fTimeDelta * m_EffectDesc.fSpeed;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	return S_OK;
}

HRESULT CRaiden_Light::Tick(_float fTimeDelta)
{
	if (m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CRaiden_Light::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	// m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CRaiden_Light::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fColorRate, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_RAIDEN_LIGHT)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CRaiden_Light* CRaiden_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRaiden_Light* pInstance = new CRaiden_Light(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CRaiden_Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRaiden_Light::Clone(void* pArg)
{
	CRaiden_Light* pInstance = new CRaiden_Light(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CRaiden_Light"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRaiden_Light::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}