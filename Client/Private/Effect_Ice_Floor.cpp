#include "stdafx.h"
#include "Effect_Ice_Floor.h"

#include "GameInstance.h"
#include "Player.h"
#include "Part_Body.h"

CEffect_Ice_Floor::CEffect_Ice_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Ice_Floor::CEffect_Ice_Floor(const CEffect_Ice_Floor& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Ice_Floor::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ice_Floor"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Collider_SPHERE */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.fRadius = 2.f;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CEffect_Ice_Floor::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Ice_Floor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Ice_Floor::Initialize_Clone(void* pArg)
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

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		/*WorldMatrix.r[0].m128_f32[0] = 1.f;
		WorldMatrix.r[0].m128_f32[1] = 0.f;
		WorldMatrix.r[0].m128_f32[2] = 0.f;
		WorldMatrix.r[0].m128_f32[3] = 0.f;

		WorldMatrix.r[1].m128_f32[0] = 0.f;
		WorldMatrix.r[1].m128_f32[1] = 1.f;
		WorldMatrix.r[1].m128_f32[2] = 0.f;
		WorldMatrix.r[1].m128_f32[3] = 0.f;

		WorldMatrix.r[2].m128_f32[0] = 0.f;
		WorldMatrix.r[2].m128_f32[1] = 0.f;
		WorldMatrix.r[2].m128_f32[2] = 1.f;
		WorldMatrix.r[2].m128_f32[3] = 0.f;*/

		WorldMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		WorldMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		m_pColliderCom->Update(WorldMatrix);
		
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Ice_Floor::PreviousTick(_float fTimeDelta)
{
	if (m_fColorRate <= 0.7f)
	{
		vector<CGameObject*> PlayerColl = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		for (auto PlayerBody : PlayerColl)
		{
			PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());
			pPlayer->Set_MoveSpeed(0.6f, eIndex, 0.2f);
			//pPlayer->Set_Damage(0.f, ELEMENT_FROST, FSM_END);
			pPlayer->Set_AutoDamage(eIndex, 0.f, ELEMENT_FROST, FSM_END);
		}
	}

	return S_OK;
}

HRESULT CEffect_Ice_Floor::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;

	_vector vSelfScale = m_pTransformCom->Get_Scale();

	switch (m_EffectDesc.eEffectID)
	{
	case Client::EFFECT_ICE_FLOOR_ICECONE:
	{
		if (vSelfScale.m128_f32[0] <= 5.f && m_fLifeTime > 0.f)		// 커지면서 나타나기
		{
			vSelfScale.m128_f32[0] += fTimeDelta * 30.f;
			vSelfScale.m128_f32[1] += fTimeDelta * 30.f;
			m_pTransformCom->Set_Scale(vSelfScale);
		}

		if (m_fLifeTime <= 0.f)		// 녹아내리기
			m_fColorRate += fTimeDelta / 10.f;

		if (m_fColorRate >= 1.f)		// 다 녹으면 지웟
		{
			m_pGameInstance->Release_Collider_Object(COLL_MONSTER_SKILL, m_iColliderID);
			Set_Destroy(TRUE);
		}
	}
	break;
	}

	m_vScaleUV = { 4.f, 4.f };

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEffect_Ice_Floor::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CEffect_Ice_Floor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fColorRate, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVScaleRate", &m_vScaleUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpinRate", &m_vSpinUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_ICE_FLOOR)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEffect_Ice_Floor* CEffect_Ice_Floor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Ice_Floor* pInstance = new CEffect_Ice_Floor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEffect_Ice_Floor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Ice_Floor::Clone(void* pArg)
{
	CEffect_Ice_Floor* pInstance = new CEffect_Ice_Floor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEffect_Ice_Floor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Ice_Floor::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
}