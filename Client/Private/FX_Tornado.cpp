#include "stdafx.h"
#include "FX_Tornado.h"
#include "DeciderObject.h"

#include "Player.h"
#include "Part_Body.h"
#include "Eff_Impact_Glow.h"
#include "Slash_Trail.h"

#include "GameInstance.h"

CFX_Tornado::CFX_Tornado(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_Tornado::CFX_Tornado(const CFX_Tornado& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_Tornado::Add_Component(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tornado"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tornado"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/*CBounding_OBB::BOUNDING_OBB_DESC tBoundDesc = {};
	tBoundDesc.vExtents = { 0.7f, 5.f, 0.7f };
	tBoundDesc.vCenter = { 0.f, tBoundDesc.vExtents.y, 0.f };
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &tBoundDesc)))
		return E_FAIL;*/

	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundDesc = {};
	tBoundDesc.fRadius = 1.f;
	tBoundDesc.vCenter = { 0.f, 0.5f, 0.f };
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Tornado::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	_float4	vUVRate = { m_fFrame, m_fFrame * 1.4f, 0.f, 0.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &vUVRate, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Tornado::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_Tornado::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);

	m_pTransformCom = pAsfxDesc->pTransform;
	Safe_AddRef(m_pTransformCom);

	m_pTransformCom->Set_Scale({ 2.f, 4.f, 2.f, 0.f });

	m_strParticleTag[0] = { Get_ParticleTag(L"Tornado_1") };
	m_strParticleTag[1] = { Get_ParticleTag(L"Tornado_2") };

	m_pGameInstance->Play_Particle(L"Tornado_1", m_strParticleTag[0], 9, 7, nullptr, m_pTransformCom);
	m_pGameInstance->Play_Particle(L"Tornado_2", m_strParticleTag[1], 9, 8, nullptr, m_pTransformCom);

	_matrix ColliderMatrix = m_pTransformCom->Get_WorldMatrix();
	ColliderMatrix.r[0] = { 1.f, 0.f, 0.f };
	ColliderMatrix.r[1] = { 0.f, 1.f, 0.f };
	ColliderMatrix.r[2] = { 0.f, 0.f, 1.f };
	m_pColliderCom->Update(ColliderMatrix);

	_uint iCurrentLevel = m_pGameInstance->Get_CurrentLevel();
	for (_uint i = 0; i < 20; ++i)
	{
		CSlash_Trail::SLASH_DESC tDesc = {};
		tDesc.eTrail_Type = CSlash_Trail::TRAIL_RANDOM;
		tDesc.pParentsTransform = m_pTransformCom;
		tDesc.eElementColor = ELEMENT_FROST;
		tDesc.iExceptAxis = CSlash_Trail::AXIS_X + CSlash_Trail::AXIS_Z;
		tDesc.fTrailSpeed = m_pGameInstance->Get_Float_Random(0.7f, 1.5f);
		tDesc.fLifeTime = -1.f;
		tDesc.fHeight = i * 0.6f;
		_float fRandomScale = m_pGameInstance->Get_Float_Random(0.05f, 0.1f);
		tDesc.vScale = XMVectorSet(fRandomScale * (i + 1), fRandomScale * (i + 1), fRandomScale, 0.f);

		CSlash_Trail* p = static_cast<CSlash_Trail*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Effect_Slash_Trail")->Clone(&tDesc));
		m_TrailList.push_back(p);
	}

	return S_OK;
}

HRESULT CFX_Tornado::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		for (auto iter : m_TrailList)
		{
			iter->PreviousTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CFX_Tornado::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* FX Frame */
		m_fFrame += fTimeDelta;
		m_fFrame = fmod(m_fFrame, 1.f);

		/* Turn */
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);

		_matrix ColliderMatrix = m_pTransformCom->Get_WorldMatrix();
		ColliderMatrix.r[0] = { 1.f, 0.f, 0.f };
		ColliderMatrix.r[1] = { 0.f, 1.f, 0.f };
		ColliderMatrix.r[2] = { 0.f, 0.f, 1.f };
		m_pColliderCom->Update(ColliderMatrix);

#pragma region 가시 이펙트
		m_fEffect += fTimeDelta;

		if (m_fEffect > 0.1f)
		{
			m_fEffect = 0.f;
			CImpact_Glow::IMPACT_DESC descc = {};
			descc.eEffectID = EFFECT_IMPACT_PLANE_CIRCULAR;
			descc.eELEMENT = ELEMENT_FROST;
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vSelfPos.m128_f32[1] += 0.2f;
			descc.pParent = (CGameObject*)this;
			descc.vSpawnPos = vSelfPos;

			_float fRandomScale = m_pGameInstance->Get_Float_Random(0.7f, 1.5f);
			descc.vScale = { 1.5f * fRandomScale, 2.1f * fRandomScale, 1.f, 1.f };
			descc.fRadius = { 0.7f };
			descc.iFanSize = m_pGameInstance->Get_Int_Random(60, 75);

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Impact_Glow", L"Layer_PostEffect", &descc)))
				return E_FAIL;
		}
#pragma endregion


#pragma region 플레이어 충돌
		vector<CGameObject*> HitPlayer = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);
		for (auto PlayerBody : HitPlayer)
		{
			m_fDuration = -1.f;
			m_bDuration = TRUE;

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());

			_float4 vDir = {};
			XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			vDir.w = 1.f;

			pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_WIND, FSM_HIT_THROW, 6.f, 4.f, vDir);
		}
#pragma endregion

		for (auto iter : m_TrailList)
		{
			iter->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CFX_Tornado::LateTick(_float fTimeDelta)
{
	for (auto iter : m_TrailList)
	{
		iter->LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_Tornado::Render()
{
	if (FAILED(CFX_Tornado::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_TORNADO)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CFX_Tornado* CFX_Tornado::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_Tornado* pInstance = new CFX_Tornado(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_Tornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_Tornado::Clone(void* pArg)
{
	CFX_Tornado* pInstance = new CFX_Tornado(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_Tornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_Tornado::Free()
{
	__super::Free();

	for (auto iter : m_TrailList)
	{
		Safe_Release(iter);
		iter = nullptr;
	}
	m_TrailList.clear();

	m_pGameInstance->Stop_Particle(m_strParticleTag[0]);
	m_pGameInstance->Stop_Particle(m_strParticleTag[1]);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
}
