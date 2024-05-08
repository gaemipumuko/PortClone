#include "stdafx.h"
#include "FX_EffigyFireExplode.h"
#include "FX_EffigyFireProjectile.h"

#include "Player.h"
#include "Part_Body.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFireProjectile::CFX_EffigyFireProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFireProjectile::CFX_EffigyFireProjectile(const CFX_EffigyFireProjectile& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFireProjectile::Add_Component(void* pArg)
{
	/* For.Com_Model_EffigyFire_Sphere */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Sphere"),
		TEXT("Com_Model_EffigyFire_Sphere"), (CComponent**)&m_pModelCom[FIRE_SPHERE])))
		return E_FAIL;

	/* For.Com_Model_EffigyFire_Cylinder */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Cylinder"),
		TEXT("Com_Model_EffigyFire_Cylinder"), (CComponent**)&m_pModelCom[FIRE_CYLINDER])))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Projectile"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform_Sphere */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Sphere"), (CComponent**)&m_pTransformCom[FIRE_SPHERE])))
		return E_FAIL;

	/* For.Com_Transform_ShellQuad */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_ShellQuad"), (CComponent**)&m_pTransformCom[FIRE_CYLINDER])))
		return E_FAIL;

	/* For.Com_Collider_SPHERE */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC		tColliderDesc = {};
	tColliderDesc.fRadius = 0.7f;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_SKILL, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);

	_vector vInitPosition	= pAsfxDesc->vInitPosition +
		_vector{ m_pGameInstance->Get_Float_Random(-0.1f, 0.1f), 0.f, m_pGameInstance->Get_Float_Random(-0.1f, 0.1f), 0.f };
	_vector vTargetPosition = pAsfxDesc->pTransform->Get_State(CTransform::STATE_POSITION) +
		_vector{ m_pGameInstance->Get_Float_Random(-1.f, 1.f), 0.f, m_pGameInstance->Get_Float_Random(-1.f, 1.f), 0.f };

	m_pTransformCom[FIRE_SPHERE]->Set_State(CTransform::STATE_POSITION, vInitPosition);
	m_pTransformCom[FIRE_SPHERE]->Set_Scale({ 0.005f, 0.005f, 0.005f, 0.f });
	m_pTransformCom[FIRE_SPHERE]->Set_Parabola_Position(vTargetPosition);

	m_pTransformCom[FIRE_CYLINDER]->Set_State(CTransform::STATE_POSITION, vInitPosition);
	m_pTransformCom[FIRE_CYLINDER]->Set_Scale({ 0.003f, 0.003f, 0.003f, 0.f });
	m_pTransformCom[FIRE_CYLINDER]->Set_Parabola_Position(vTargetPosition);

	XMStoreFloat4(&m_vPrevPosition, vTargetPosition);
	XMStoreFloat4(&m_vTargetPosition, vTargetPosition);

	/* Particle */
	wstring strParticle_Large1 = Get_ParticleTag(L"Effigy_PaperBurn_1_Large");
	wstring strParticle_Large2 = Get_ParticleTag(L"Effigy_PaperBurn_2_Large");
	wstring strParticle_Large3 = Get_ParticleTag(L"Effigy_PaperBurn_3_Large");
	wstring strParticle_Large4 = Get_ParticleTag(L"Effigy_PaperBurn_4_Large");

	m_strParticleTag.push_back(strParticle_Large1);
	m_strParticleTag.push_back(strParticle_Large2);
	m_strParticleTag.push_back(strParticle_Large3);
	m_strParticleTag.push_back(strParticle_Large4);

	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_1_Large", strParticle_Large1,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_SPHERE]);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_2_Large", strParticle_Large2,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_SPHERE]);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_3_Large", strParticle_Large3,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_SPHERE]);
	m_pGameInstance->Play_Particle(L"Effigy_PaperBurn_4_Large", strParticle_Large4,
		PARTICLE_EFFIGY_PAPERBURN, TEX_PARTICLE_PAPER, nullptr, m_pTransformCom[FIRE_SPHERE]);

	_matrix ColliderMatrix = XMMatrixIdentity();
	ColliderMatrix.r[3] = m_pTransformCom[FIRE_SPHERE]->Get_State(CTransform::STATE_POSITION);
	m_pColliderCom->Update(ColliderMatrix);

	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* FX Frame */
		m_vFrame.x += fTimeDelta;
		m_vFrame.x = fmod(m_vFrame.x, 1.f);

		/* Move To Target */
		m_pTransformCom[FIRE_SPHERE]->Move_To_Parabola_Position(fTimeDelta, 0.1f);

		/* Caculate Look Direction */
		_vector	vLookDir = XMLoadFloat4(&m_vPrevPosition) - m_pTransformCom[FIRE_SPHERE]->Get_State(CTransform::STATE_POSITION);

		/* Store Curr Position */
		XMStoreFloat4(&m_vPrevPosition, m_pTransformCom[FIRE_SPHERE]->Get_State(CTransform::STATE_POSITION));

		/* Invalidate Outer */
		m_pTransformCom[FIRE_CYLINDER]->Set_WorldMatrix(m_pTransformCom[FIRE_SPHERE]->Get_WorldMatrix());

		/* Invalidate Look Direction */
		m_pTransformCom[FIRE_CYLINDER]->Look_At_Direction(vLookDir);

		_matrix ColliderMatrix = XMMatrixIdentity();
		ColliderMatrix.r[3] = m_pTransformCom[FIRE_SPHERE]->Get_State(CTransform::STATE_POSITION);
		m_pColliderCom->Update(ColliderMatrix);

		if (m_bCollision == FALSE)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			vector<CGameObject*> HitPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_SKILL, m_iColliderID, COLL_PLAYER_BODY);
			for (auto PlayerBody : HitPlayer)
			{
				m_bCollision = TRUE;
				PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(PlayerBody)->Get_CreatureId());
				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(943, 2123), ELEMENT_FIRE, FSM_HIT_THROW, 1.f, 2.f, {});
			}
		}

		/* Check Explode Condition */
		if (m_bCollision == TRUE || m_pTransformCom[FIRE_SPHERE]->Get_State(CTransform::STATE_POSITION).m128_f32[1] < m_vTargetPosition.y + 1.f)
		{
			m_bDuration = TRUE;
			m_fDuration = -1.f;

			for (auto& iter : m_strParticleTag)
				m_pGameInstance->Stop_Particle(iter);

			CFX_EffigyFireExplode::EXPLODE_DESC tExplodeDesc = { };
			tExplodeDesc.fExplodeExpand = 2.f;
			tExplodeDesc.ePosType = CAssetFX::ASFX_DESC::POS_FIX;
			tExplodeDesc.vInitPosition = m_pTransformCom[FIRE_SPHERE]->Get_State(CTransform::STATE_POSITION);

			m_pGameInstance->PlaySoundW(L"EffigyFire_Chopping.mp3");

			m_pGameInstance->Play_AssetFX(L"EffigyFireExplode", Get_ParticleTag(L"EffigyFireExplode"), &tExplodeDesc);
		}
	}

	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CFX_EffigyFireProjectile::Render()
{
	if (FAILED(CFX_EffigyFireProjectile::Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = { };

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_SPHERE]->Get_WorldFloat4x4())))
		return E_FAIL;

	iNumMeshes = m_pModelCom[FIRE_SPHERE]->Get_NumMeshes();
	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_INPROJ)))
			return E_FAIL;

		if (FAILED(m_pModelCom[FIRE_SPHERE]->Render(siMesh)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom[FIRE_CYLINDER]->Get_WorldFloat4x4())))
		return E_FAIL;

	iNumMeshes = m_pModelCom[FIRE_CYLINDER]->Get_NumMeshes();
	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pShaderCom->Begin(MESHFX_EFFIGYFIRE_OUTPROJ)))
			return E_FAIL;

		if (FAILED(m_pModelCom[FIRE_CYLINDER]->Render(siMesh)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

CFX_EffigyFireProjectile* CFX_EffigyFireProjectile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFireProjectile* pInstance = new CFX_EffigyFireProjectile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFireProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFireProjectile::Clone(void* pArg)
{
	CFX_EffigyFireProjectile* pInstance = new CFX_EffigyFireProjectile(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFireProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFireProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom[FIRE_SPHERE]);
	Safe_Release(m_pModelCom[FIRE_CYLINDER]);

	Safe_Release(m_pTransformCom[FIRE_SPHERE]);
	Safe_Release(m_pTransformCom[FIRE_CYLINDER]);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
}
