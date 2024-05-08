#include "stdafx.h"
#include "FX_Meteorite.h"
#include "FX_EffigyFireExplode.h"

#include "DeciderObject.h"
#include "Slash_Trail.h"
#include "FX_CircularIndicator.h"
#include "Player.h"
#include "Part_Body.h"
#include "GameInstance.h"

CFX_Meteorite::CFX_Meteorite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_Meteorite::CFX_Meteorite(const CFX_Meteorite& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_Meteorite::Add_Component(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Meteorite"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meteorite"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundDesc = {};
	tBoundDesc.fRadius = 2.f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Meteorite::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vMeteoriteColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_Meteorite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_Meteorite::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	METEO_DESC* pMeteoDesc = static_cast<METEO_DESC*>(pArg);

	m_vDirection		= pMeteoDesc->vDirection;
	m_vTargetPosition	= pMeteoDesc->vTargetPosition;
	m_vMeteoriteColor	= pMeteoDesc->vMeteoriteColor;

	m_fSpeed = pMeteoDesc->fSpeed;
	if (m_fSpeed <= 0.f)
		m_fSpeed = 1.f;

	m_fScale = pMeteoDesc->fScale;
	if (m_fScale <= 0.f)
		m_fScale = 1.f;
	else
	{
		CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundDesc = {};
		tBoundDesc.fRadius = m_fScale;
		m_pColliderCom->Update_Desc(&tBoundDesc);
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pMeteoDesc->vInitPosition);

	/* Show Indicator */
	CFX_CircularIndicator::CIND_DESC tIndicatorDesc = { };
	tIndicatorDesc.fDistance		= pMeteoDesc->fDistance;
	tIndicatorDesc.fScale			= m_fScale;
	tIndicatorDesc.pTransform		= m_pTransformCom;
	tIndicatorDesc.vInitPosition	= XMLoadFloat4(&pMeteoDesc->vTargetPosition) + _vector{ 0.f, 0.2f, 0.f, 0.f };

	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	m_strIndicatorTag = Get_ParticleTag(L"CircularIndicator");
	m_pGameInstance->Play_AssetFX(L"CircularIndicator", m_strIndicatorTag, &tIndicatorDesc);

	return S_OK;
}

HRESULT CFX_Meteorite::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_Meteorite::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_vFrame.x += fTimeDelta;	/* Acc Frame */
		m_vFrame.y += fTimeDelta;	/* Sword Trail */
		m_vFrame.z = fmod(m_vFrame.x, 1.f);	/* Shader Frame */

		/* Move To */
		m_pTransformCom->Move_To_Dir(XMLoadFloat4(&m_vDirection) * -1.f, fTimeDelta * 10.f * m_fSpeed);


		_matrix ColliderMatrix = m_pTransformCom->Get_WorldMatrix();
		ColliderMatrix.r[0] = XMVector3Normalize(ColliderMatrix.r[0]);
		ColliderMatrix.r[1] = XMVector3Normalize(ColliderMatrix.r[1]);
		ColliderMatrix.r[2] = XMVector3Normalize(ColliderMatrix.r[2]);
		m_pColliderCom->Update(ColliderMatrix);

		_bool bCollision = FALSE;
		vector<CGameObject*> HitPlayer = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);
		for (auto PlayerBody : HitPlayer)
		{
			bCollision = TRUE;

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());

			_float4 vDir = {};
			XMStoreFloat4(&vDir, XMVector3Normalize(pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			vDir.w = 1.f;

			pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(443, 523), ELEMENT_WATER, FSM_HIT_THROW, 6.f, 4.f, vDir);
		}

		/* Sword Trail */
		if (m_vFrame.y > 0.1f)
		{
			// CSlash_Trail::SLASH_DESC tDesc = {};
			// tDesc.pParentsTransform = m_pTransformCom;
			// tDesc.fHeight = 0.f;
			// tDesc.fAngle = m_pGameInstance->Get_Float_Random(0.f, 360.f);
			// tDesc.fTrailSpeed = 1.5f;
			// tDesc.bTrailReverse = FALSE;
			// 
			// if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
			// 	return E_FAIL;

			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.eTrail_Type = CSlash_Trail::TRAIL_RANDOM;
			tDesc.pParentsTransform = m_pTransformCom;
			tDesc.eElementColor = ELEMENT_FROST;
			tDesc.fTrailSpeed = m_pGameInstance->Get_Float_Random(1.f, 3.f);
			tDesc.fLifeTime = m_pGameInstance->Get_Float_Random(0.5f, 1.f);
			_float fRandomScale = m_pGameInstance->Get_Float_Random(0.5f, 1.f);
			tDesc.vScale = XMVectorSet(fRandomScale * m_fScale, fRandomScale * m_fScale, 1.f, 0.f);
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;

			m_vFrame.y = 0.f;
		}

		/* Check Explode Condition */
		if (bCollision == TRUE || m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < m_vTargetPosition.y)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;

			m_pGameInstance->Stop_AssetFX(m_strIndicatorTag);

			CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
			tExplodeDesc.fExplodeExpand = 3.f;
			tExplodeDesc.fScale = m_fScale;
			tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tExplodeDesc.vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vector{ 0.f, 1.f, 0.f, 0.f };

			m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);
		}
	}

	return S_OK;
}

HRESULT CFX_Meteorite::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_Meteorite::Render()
{
	if (FAILED(CFX_Meteorite::Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_METEORITE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	m_pColliderCom->Render();

	return S_OK;
}

CFX_Meteorite* CFX_Meteorite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_Meteorite* pInstance = new CFX_Meteorite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_Meteorite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_Meteorite::Clone(void* pArg)
{
	CFX_Meteorite* pInstance = new CFX_Meteorite(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_Meteorite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_Meteorite::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
}
