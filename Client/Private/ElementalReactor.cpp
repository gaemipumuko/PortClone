#include "stdafx.h"
#include "ElementalReactor.h"

#include "GameInstance.h"

#define REACTOR_FOUNDATION	2
#define REACTOR_SYMBOL_OL	2
#define REACTOR_SYMBOL		3

CElementalReactor::CElementalReactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CElementalReactor::CElementalReactor(const CElementalReactor& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CElementalReactor::Invalidate_Animation(_float fTimeDelta)
{
	if (m_bReacted == FALSE)
	{
		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
	}
	else
	{
		if (m_byStep == 0)
		{
			m_pModelCom->Set_Animation(3);
			_float fRate = m_pModelCom->Play_Animation(FALSE, m_pTransformCom, fTimeDelta);
			if (fRate > m_pModelCom->Get_AnimationDuration(3))
				++m_byStep;
		}
	}

	return S_OK;
}

HRESULT CElementalReactor::Add_Component(void* pArg)
{
	REACTOR_DESC* pReactorDesc = static_cast<REACTOR_DESC*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(pReactorDesc->eReactorLevel, TEXT("Prototype_Component_Model_Elemental_Reactor"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(pReactorDesc->eReactorLevel, TEXT("Prototype_Component_Texture_Elemental_Reactor"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model_ActFX */
	if (FAILED(CGameObject::Add_Component(pReactorDesc->eReactorLevel, TEXT("Prototype_Component_Model_Activate_Reactor_FX"),
		TEXT("Com_Model_ActFX"), (CComponent**)&m_pActiveFXModelCom)))
		return E_FAIL;

	/* For.Com_Shader_ActFX */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		TEXT("Com_Shader_ActFX"), (CComponent**)&m_pActiveFXShaderCom)))
		return E_FAIL;

	/* For.Com_Texture_ActFX */
	if (FAILED(CGameObject::Add_Component(pReactorDesc->eReactorLevel, TEXT("Prototype_Component_Texture_Activate_Reactor_FX"),
		TEXT("Com_Texture_ActFX"), (CComponent**)&m_pActiveFXTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 0.5f;
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_MONSTER_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CElementalReactor::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_EffectTextures")))
		return E_FAIL;

	_float4 vUVRate = { };
	switch (m_eElement)
	{
	case ELEMENT_ELECTRIC:
		vUVRate = { 3, 1, 4, 2 };
		break;

	case ELEMENT_FIRE:
		vUVRate = { 0, 1, 4, 2 };
		break;

	case ELEMENT_FROST:
		vUVRate = { 2, 1, 4, 2 };
		break;

	case ELEMENT_EARTH:
		vUVRate = { 3, 0, 4, 2 };
		break;

	case ELEMENT_WATER:
		vUVRate = { 0, 0, 4, 2 };
		break;

	case ELEMENT_WIND:
		vUVRate = { 1, 0, 4, 2 };
		break;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVRate", &vUVRate, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CElementalReactor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElementalReactor::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	REACTOR_DESC* pReactorDesc = static_cast<REACTOR_DESC*>(pArg);

	m_eElement = pReactorDesc->eReactElement;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pReactorDesc->vReactorPosition));

	m_pModelCom->Cooking_StaticModel(m_pTransformCom->Get_WorldFloat4x4());

	return S_OK;
}

HRESULT CElementalReactor::PreviousTick(_float fTimeDelta)
{
	if ((m_bReacted == FALSE) && (m_eReceiveElement == m_eElement))
	{
		m_pGameInstance->PlaySoundW(L"RiddleSolve.mp3");

		m_bReacted = TRUE;
		m_eReceiveElement = ELEMENT_END;

#ifdef _DEBUG
		cout << "Receive" << endl;
#endif
	}

	if ((m_bActiveFX == FALSE) && (m_bReacted == TRUE))
		m_bActiveFX = TRUE;

	return S_OK;
}

HRESULT CElementalReactor::Tick(_float fTimeDelta)
{
	if (FAILED(CElementalReactor::Invalidate_Animation(fTimeDelta)))
		return E_FAIL;

	if (m_bActiveFX == TRUE)
	{
		m_fActiveFXFrame.x -= fTimeDelta;
		m_fActiveFXFrame.y += fTimeDelta;
		m_fActiveFXFrame.z += fTimeDelta;
	}

	_float4x4 ColliderPos = m_pTransformCom->Get_WorldFloat4x4();
	ColliderPos._11 *= 2.f; ColliderPos._22 *= 2.f; ColliderPos._33 *= 2.f;
	m_pColliderCom->Update(XMLoadFloat4x4(&ColliderPos));

	return S_OK;
}

HRESULT CElementalReactor::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CElementalReactor::Render()
{
	_uint		iNumMeshes = { };

	if (m_byRenderStep == 0)
	{
		m_byRenderStep = 1;

		if (FAILED(CElementalReactor::Bind_ShaderResources()))
			return E_FAIL;

		/* Reactor */
		iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (_uint siMesh = 0; siMesh < REACTOR_FOUNDATION; siMesh++)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(ANIMFX_REACTOR)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(siMesh)))
				return E_FAIL;
		}

		/* Reactor - Symbol Outline */
		if (m_bReacted == FALSE)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(REACTOR_SYMBOL_OL), m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(ANIMFX_REACTOR_SYMBOL)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(REACTOR_SYMBOL_OL)))
				return E_FAIL;

			/* Reactor - Symbol */
			if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(REACTOR_SYMBOL), m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(ANIMFX_REACTOR_SYMBOL)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(REACTOR_SYMBOL)))
				return E_FAIL;
		}
	}

	if (m_byRenderStep == 1)
	{
		m_byRenderStep = 0;
		if (m_bActiveFX == TRUE)
		{
			if (FAILED(m_pActiveFXShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;

			if (FAILED(m_pActiveFXShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
				return E_FAIL;

			if (FAILED(m_pActiveFXShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
				return E_FAIL;

			if (FAILED(m_pActiveFXTextureCom->Bind_ShaderResources(m_pActiveFXShaderCom, "g_EffectTextures")))
				return E_FAIL;

			if (FAILED(m_pActiveFXShaderCom->Bind_RawValue("g_vUVRate", &m_fActiveFXFrame, sizeof(_float4))))
				return E_FAIL;

			_float4 vAttColor = { };
			switch (m_eElement)
			{
			case ELEMENT_FIRE:      // 불
				vAttColor = { 0.6500f, 0.2758f, 0.2758f, 1.0000f };
				break;
			case ELEMENT_WATER:     // 물
				vAttColor = { 0.2471f, 0.5019f, 0.9098f, 1.0000f };
				break;
			case ELEMENT_FROST:     // 얼음
				vAttColor = { 0.4471f, 0.7373f, 0.8000f, 1.0000f };
				break;
			case ELEMENT_ELECTRIC:  // 번개
				vAttColor = { 0.5953f, 0.2674f, 0.7377f, 1.0000f };
				break;
			case ELEMENT_WIND:      // 바람
				vAttColor = { 0.0000f, 0.5992f, 0.5177f, 1.0000f };
				break;
			case ELEMENT_EARTH:     // 바위
				vAttColor = { 0.5584f, 0.4282f, 0.0025f, 1.0000f };
				break;
			case ELEMENT_END:		// 무속성
				vAttColor = { 1.0000f, 1.0000f, 1.0000f, 1.0000f };
				break;
			}

			if (FAILED(m_pActiveFXShaderCom->Bind_RawValue("g_vColor", &vAttColor, sizeof(_float4))))
				return E_FAIL;

			iNumMeshes = m_pActiveFXModelCom->Get_NumMeshes();
			for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
			{
				if (FAILED(m_pActiveFXShaderCom->Begin(MESHFX_REACTOR_ACTIVEFX)))
					return E_FAIL;

				if (FAILED(m_pActiveFXModelCom->Render(siMesh)))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

CElementalReactor* CElementalReactor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElementalReactor* pInstance = new CElementalReactor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CElementalReactor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElementalReactor::Clone(void* pArg)
{
	CElementalReactor* pInstance = new CElementalReactor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CElementalReactor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElementalReactor::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pActiveFXModelCom);
	Safe_Release(m_pActiveFXShaderCom);
	Safe_Release(m_pActiveFXTextureCom);
}
