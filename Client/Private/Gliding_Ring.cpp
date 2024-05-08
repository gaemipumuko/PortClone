#include "stdafx.h"
#include "Gliding_Ring.h"

#include "GameInstance.h"
#include "Player.h"

CGliding_Ring::CGliding_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CGliding_Ring::CGliding_Ring(const CGliding_Ring& rhs)
	: CCreature(rhs)
{
}

HRESULT CGliding_Ring::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Gliding_Ring"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGliding_Ring::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGliding_Ring::Add_Parts(void* pArg)
{
	return S_OK;
}

HRESULT CGliding_Ring::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGliding_Ring::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CGliding_Ring::Add_Components(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		GLIDINGRING_DESC* GlidingDesc = static_cast<GLIDINGRING_DESC*>(pArg);
		m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(GlidingDesc->fRadian));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, GlidingDesc->vPos);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f,0.f,0.f,1.f });

	return S_OK;
}

HRESULT CGliding_Ring::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;
		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);

		_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];
		// 임시 
		if (fDistance <= 2.1f)	/* 플레이어와 충돌 */
		{
			m_pGameInstance->StopSound(CHANNELID::SOUND_CH27);
			m_pGameInstance->PlaySoundW(L"RingEat.wav", 1.f, CHANNELID::SOUND_CH27);
			Set_Destroy(TRUE);
			m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, m_pTransformCom, 0.5f);
		}
	}

	return S_OK;
}

HRESULT CGliding_Ring::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGliding_Ring::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	}

	return S_OK;
}

HRESULT CGliding_Ring::Render()
{
	if (FAILED(CGliding_Ring::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(VTXMESH_GLIDING_RING)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CGliding_Ring* CGliding_Ring::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGliding_Ring* pInstance = new CGliding_Ring(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGliding_Ring"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGliding_Ring::Clone(void* pArg)
{
	CGliding_Ring* pInstance = new CGliding_Ring(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGliding_Ring"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGliding_Ring::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
