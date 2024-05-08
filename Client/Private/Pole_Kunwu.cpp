#include "stdafx.h"
#include "Pole_Kunwu.h"

#include "Part_Body.h"
#include "Player.h"
#include "Piercing_Trail.h"
#include "Slash_Trail.h"

#include "GameInstance.h"

CPole_Kunwu::CPole_Kunwu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPart_Weapon(pDevice, pContext)
{
}

CPole_Kunwu::CPole_Kunwu(const CPole_Kunwu& rhs)
    : CPart_Weapon(rhs)
{
}

//HRESULT CPole_Kunwu::Bind_ShaderResources_Trail()
//{
//	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
//		return E_FAIL;
//
//	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
//		return E_FAIL;
//
//	if (FAILED(m_pTrailTextureCom->Bind_ShaderResource(m_pTrailShaderCom, "g_Texture", 0)))
//		return E_FAIL;
//
//	if (FAILED(m_pTrailTextureCom->Bind_ShaderResource(m_pTrailShaderCom, "g_DiffuseTexture", 1)))
//		return E_FAIL;
//
//	if (FAILED(m_pTrailShaderCom->Bind_RawValue("g_vOriginalColor", &m_vTrailOriginalColor, sizeof(_float4))))
//		return E_FAIL;
//
//	if (FAILED(m_pTrailShaderCom->Bind_RawValue("g_vAttenuationColor", &m_vTrailAttenuationColor, sizeof(_float4))))
//		return E_FAIL; 
//
//
//	return S_OK;
//}

//_matrix CPole_Kunwu::Get_CombinedMatrix()
//{
//	_matrix CombinedMatrix = (XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Trail_Low"))) * XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) * m_pParentTransformCom->Get_WorldMatrix();
//
//	return CombinedMatrix;
//}

HRESULT CPole_Kunwu::Invalidate_Animation(_float fTimeDelta)
{
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_ZHONGLI));
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	if (eState == FSM_IDLE || eState == FSM_WALK || eState == FSM_SPLINT || eState == FSM_JUMP 
		|| eState == FSM_RUN || eState == FSM_DASH
		|| eState == FSM_WALK_RUN || eState == FSM_WALK_IDLE || eState == FSM_RUN_WALK || eState == FSM_RUN_STOP || eState == FSM_SPLINT_RUN
		|| eState == FSM_FLY_START || eState == FSM_FLY_NORMAL || eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
		|| eState == FSM_FALL_LOW || eState == FSM_FALL_HIGH || eState == FSM_DROP_DOWN
		|| eState == FSM_FALL_ATTACK_END1 || eState == FSM_FALL_ATTACK_END2)
	{
		if (m_bHand == TRUE)
		{
			m_bHand = FALSE;
			m_fDissolve = 1.f;
			m_pGameInstance->PlaySoundW(L"CreateWeapon.mp3", 1.f);
		}

		if (m_fDissolve > 0.f)
			m_fDissolve -= 0.02f;
		else
			m_fDissolve = 0.f;

		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
		m_fAttackEndTime = 0.f;
	}
	else if (eState == FSM_ATTACK_END)
	{
		m_fAttackEndTime += fTimeDelta;
		if (m_fAttackEndTime >= 2.2f && m_fAttackEndTime < 4.4f)
		{
			if (m_fDissolve < 1.f)
				m_fDissolve += 0.04f;
			else
				m_fDissolve = 1.f;
			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
		}
		else if (m_fAttackEndTime >= 4.4f) /* 이펙트 효과 추가해야함*/
			m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("PRIVATE_WeaponRootSword"));
	}
	else
	{
		m_bHand = TRUE;
		m_fDissolve = 0.f;
		m_pModelCom->Set_PartRootBone("RootNode", pModel->Get_Bone("WeaponR"));
		m_fAttackEndTime = 0.f;
	}

	return S_OK;
}

HRESULT CPole_Kunwu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPole_Kunwu::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(1.f, 1.5f, 1.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y - 1.f, -1.f);
	BoundingDesc.vRadians = _float3(XMConvertToRadians(90.f), 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_PLAYER_WEAPON, &m_iColliderID, m_pColliderCom, this);

	m_vDissolveColor = _float3(1.f, 1.f, 0.f);
	m_fDissolveSize = 0.2f;

	return S_OK;
}

HRESULT CPole_Kunwu::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

    return S_OK;
}

HRESULT CPole_Kunwu::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_PlayerIndex(CHARACTER_ZHONGLI));
	m_vRootTranslation = static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Get_RootTranslation();


	_matrix CombineWorldMatrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);
	CombineWorldMatrix.r[0] = XMVector3Normalize(CombineWorldMatrix.r[0]);
	CombineWorldMatrix.r[1] = XMVector3Normalize(CombineWorldMatrix.r[1]);
	CombineWorldMatrix.r[2] = XMVector3Normalize(CombineWorldMatrix.r[2]);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("RootNode")) *
		XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) *
		CombineWorldMatrix);

	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));


	Invalidate_Animation(fTimeDelta);

	m_pModelCom->Invalidate_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CPole_Kunwu::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

		///* Render Trail */
		//_matrix TrailMatrix = { };
		//_vector	vLow = { }, vHigh = { };

		//TrailMatrix =
		//	XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Trail_Low")) *
		//	XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
		//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
		//vLow = TrailMatrix.r[3];

		//TrailMatrix =
		//	XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("Trail_High")) *
		//	XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
		//	XMLoadFloat4x4(&m_CombinedWorldMatrix);
		//vHigh = TrailMatrix.r[3];

		//if (eState == FSM_ATTACK_READY ||
		//	eState == FSM_ATTACK1 ||
		//	eState == FSM_ATTACK2 ||
		//	eState == FSM_ATTACK3 ||
		//	eState == FSM_ATTACK4 ||
		//	eState == FSM_ATTACK5 ||
		//	eState == FSM_ATTACK6)
		//{
		//	m_pTrailBufferCom->Set_PlayTrail(TRUE);
		//	m_pTrailBufferCom->Append_TrailVertex(fTimeDelta, vLow, vHigh);
		//}
		//else
		//{
		//	m_pTrailBufferCom->Set_PlayTrail(FALSE);
		//}

		//m_pTrailBufferCom->Invalidate_VertexBuff(fTimeDelta);
		//
		//if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3], 5.f))
		//{
		//	m_pRendererCom->Add_RenderGroup(CRenderer::RG_TRAIL, this);
		//}


	return S_OK;
}

//HRESULT CPole_Kunwu::Render_Trail()
//{
//	if (FAILED(Bind_ShaderResources_Trail()))
//		return E_FAIL;
//
//	/* Shader */
//	if (FAILED(m_pTrailShaderCom->Begin(TRAIL_DEFAULT)))
//		return E_FAIL;
//
//	/* VIBuffer */
//	if (FAILED(m_pTrailBufferCom->Render()))
//		return E_FAIL;
//
//	return S_OK;
//}

HRESULT CPole_Kunwu::Render()
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState == FSM_ATTACK_CHARGED1 ||
		eState == FSM_ELEMENTAL_BURST || eState == FSM_ELEMENTAL_BURST_IDLE ||
		eState == FSM_HIT_THROW ||
		eState == FSM_HIT_THROW_FLY || eState == FSM_HIT_ROLLING || eState == FSM_SHOW)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CPole_Kunwu* CPole_Kunwu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPole_Kunwu* pInstance = new CPole_Kunwu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPole_Kunwu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPole_Kunwu::Clone(void* pArg)
{
	CPole_Kunwu* pInstance = new CPole_Kunwu(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPole_Kunwu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPole_Kunwu::Free()
{
	__super::Free();
}
