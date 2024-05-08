#include "stdafx.h"
#include "Elemental_Particle.h"

#include "GameInstance.h"

CElemental_Particle::CElemental_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CElemental(pDevice, pContext)
{
}

CElemental_Particle::CElemental_Particle(const CElemental_Particle& rhs)
    : CElemental(rhs)
{
}

HRESULT CElemental_Particle::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

    return S_OK;
}

HRESULT CElemental_Particle::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vAttColor, sizeof(_float4))))
		return E_FAIL;

	_float4	vUVRate = { m_fFrame, 0.f, 0.f, 0.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &vUVRate, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CElemental_Particle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CElemental_Particle::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

	if (FAILED(CElemental_Particle::Add_Component(pArg)))
		return E_FAIL;

	ELEMENTAL_DESC* pDesc = static_cast<ELEMENTAL_DESC*>(pArg);

	_vector	vCollectionPosition = pDesc->vInitPosition;
	vCollectionPosition.m128_f32[0] += m_pGameInstance->Get_Float_Random(-1.f, 1.f);
	vCollectionPosition.m128_f32[2] += m_pGameInstance->Get_Float_Random(-1.f, 1.f);

	m_pTransformCom->Set_Scale({ 0.001f, 0.001f, 0.001f, 0.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCollectionPosition);

    return S_OK;
}

HRESULT CElemental_Particle::PreviousTick(_float fTimeDelta)
{
	_vector vPosition		= m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPosition = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);
	vTargetPosition.m128_f32[1]	+= 2.f;

	if (abs(XMVectorGetX(XMVector4Length(vPosition - vTargetPosition))) < 0.8f)
	{
		m_bIntersect = TRUE;
		
		// 여기에 원소 올리는 함수
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		if (pPlayer == nullptr)
		{
			MSG_BOX(TEXT("Failed to Find : Player"));
			return E_FAIL;
		}
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());
		CPart_Status* pStatus = static_cast<CPart_Status*>(static_cast<CPlayer*>(pPlayer)->Get_PartObject(CPlayer::PART_STATUS, iIndex));
		if (pStatus->Get_CurEnergy() < pStatus->Get_ElementalEnergy())
			pStatus->Add_CurEnergy(1.f);
	}

    return S_OK;
}

HRESULT CElemental_Particle::Tick(_float fTimeDelta)
{
	m_fFrame += fTimeDelta * 2.5f;

	_vector vPosition		= m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPosition = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);
	vTargetPosition.m128_f32[1] += 1.75f;

	_vector	vDirection		= XMVector4Normalize(vTargetPosition - vPosition);
	if (abs(vTargetPosition.m128_f32[1] - vPosition.m128_f32[1]) < 0.5f)
		vDirection.m128_f32[1] = 0.f;

	vDirection.m128_f32[0]	= (vDirection.m128_f32[0] * min(m_fFrame, 5.f));
	vDirection.m128_f32[2]	= (vDirection.m128_f32[2] * min(m_fFrame, 5.f));

	m_pTransformCom->Move_To_Dir(vDirection, fTimeDelta * 1.4f);

    return S_OK;
}

HRESULT CElemental_Particle::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CElemental_Particle::Render()
{
	if (FAILED(CElemental_Particle::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_ELEMENTAL)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CElemental_Particle* CElemental_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElemental_Particle* pInstance = new CElemental_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CElemental_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElemental_Particle::Clone(void* pArg)
{
	CElemental_Particle* pInstance = new CElemental_Particle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CElemental_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElemental_Particle::Free()
{
	__super::Free();
}
