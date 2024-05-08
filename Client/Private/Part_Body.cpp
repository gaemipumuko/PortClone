#include "stdafx.h"
#include "Part_Body.h"

#include "Camera.h"
#include "Creature.h"

#include "GameInstance.h"
#include "Elemental_Manager.h"

CPart_Body::CPart_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPart_Body::CPart_Body(const CPart_Body& rhs)
	: CPartObject(rhs)
{
}

HRESULT CPart_Body::Add_Component(void* pArg)
{
	PARTBODY_DESC* pDesc = static_cast<PARTBODY_DESC*>(pArg);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(pDesc->tModelComponent.iComponentLevel, pDesc->tModelComponent.strComponentTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_TrailShader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_TrailShader"), (CComponent**)&m_pTrailShaderCom)))
		return E_FAIL;

	/* For.Com_TrailTexture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail_Andrius"),
		TEXT("Com_TrailTexture"), (CComponent**)&m_pTrailTextureCom)))
		return E_FAIL;

	/* For.Com_TrailBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_TrailBuffer"), (CComponent**)&m_pTrailBufferCom)))
		return E_FAIL;

	///* For.Com_TrailBuffer2 */
	//if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_80"),
	//	TEXT("Com_TrailBuffer"), (CComponent**)&m_pTrailBufferCom2)))
	//	return E_FAIL;

	/* For.Com_Texture_FaceShadow */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FaceShadow"),
		TEXT("Com_Texture_FaceShadow"), (CComponent**)&m_pFaceShadowTexCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Body::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

   	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if (m_eCreatureID < NPC_PAIMON)
	{
		if (FAILED(m_pFaceShadowTexCom->Bind_ShaderResource(m_pShaderCom, "g_FaceShadowTexture", static_cast<_uint>(m_eCreatureID - 1))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPart_Body::Bind_ShaderResources_Trail()
{
	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTrailTextureCom->Bind_ShaderCurrentTexture(m_pTrailShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

void CPart_Body::Set_ControllerTranslation(_vector vCurrentTranslation)
{
	_vector vPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);

	vLook = XMVector3Normalize(vLook) * XMVector3Length(vCurrentTranslation);

	m_pParentController->Add_Direction(vLook);
}

void CPart_Body::Set_EtcTranslation(_vector vCurrentTranslation)
{
	_vector vPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook) * XMVector3Length(vCurrentTranslation);

	m_pParentTransformCom->Set_TranslationPosition(vCurrentTranslation);
}

void CPart_Body::Player_Hit(_vector vDir)
{
	_vector vPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pParentTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook) * XMVector3Length(vDir);

	CCharacterController* pPlayerController
		= static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")))->Get_Controller();
	if (pPlayerController == nullptr)
		return;

	pPlayerController->Add_Direction(vLook);
}

HRESULT CPart_Body::ElementalReaction()
{
	for (_uint i = 0; i < ELEMENT_END; ++i)
	{
		if ((m_bHostElement & (1 << i)) != 0)
		{
			if (i == ELEMENT_FIRE) // 불 원소가 호스트일때
			{
				if (m_eReceiveElement == ELEMENT_WATER)
				{
					m_eElementalReaction = REACT_VAPORIZE;
					m_bHostElement ^= (1 << ELEMENT_FIRE);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WIND)
				{
					m_eElementalReaction = REACT_SWIRL_FIRE;
					m_bHostElement ^= (1 << ELEMENT_FIRE);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_ELECTRIC)
				{
					m_eElementalReaction = REACT_OVERLOADED;
					m_bHostElement ^= (1 << ELEMENT_FIRE);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_FROST)
				{
					m_eElementalReaction = REACT_MELT_REVERSE;
					m_bHostElement ^= (1 << ELEMENT_FIRE);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_EARTH)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_FIRE;
					m_bHostElement ^= (1 << ELEMENT_FIRE);
					return S_OK;
				}
			}

			else if (i == ELEMENT_WATER)	// 물 원소가 호스트일때
			{
				if (m_eReceiveElement == ELEMENT_FIRE)
				{
					m_eElementalReaction = REACT_VAPORIZE_REVERSE;
					m_bHostElement ^= (1 << ELEMENT_WATER);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WIND)
				{
					m_eElementalReaction = REACT_SWIRL_WATER;
					m_bHostElement ^= (1 << ELEMENT_WATER);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_ELECTRIC)
				{
					m_eElementalReaction = REACT_ELECTROCHARGED;
					m_bHostElement ^= (1 << ELEMENT_WATER);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_FROST)
				{
					m_eElementalReaction = REACT_FROZEN;
					m_bHostElement ^= (1 << ELEMENT_WATER);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_EARTH)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_WATER;
					m_bHostElement ^= (1 << ELEMENT_WATER);
					return S_OK;
				}
			}

			else if (i == ELEMENT_WIND) // 바람 원소가 호스트일때
			{
				if (m_eReceiveElement == ELEMENT_FIRE)
				{
					m_eElementalReaction = REACT_SWIRL_FIRE;
					m_bHostElement ^= (1 << ELEMENT_WIND);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WATER)
				{
					m_eElementalReaction = REACT_SWIRL_WATER;
					m_bHostElement ^= (1 << ELEMENT_WIND);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_ELECTRIC)
				{
					m_eElementalReaction = REACT_SWIRL_ELECTRIC;
					m_bHostElement ^= (1 << ELEMENT_WIND);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_FROST)
				{
					m_eElementalReaction = REACT_SWIRL_FROST;
					m_bHostElement ^= (1 << ELEMENT_WIND);
					return S_OK;
				}
			}

			else if (i == ELEMENT_ELECTRIC) // 번개 원소가 호스트일때
			{
				if (m_eReceiveElement == ELEMENT_FIRE)
				{
					m_eElementalReaction = REACT_OVERLOADED;
					m_bHostElement ^= (1 << ELEMENT_ELECTRIC);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WATER)
				{
					m_eElementalReaction = REACT_ELECTROCHARGED;
					m_bHostElement ^= (1 << ELEMENT_ELECTRIC);
					return S_OK;
				}
				
				if (m_eReceiveElement == ELEMENT_WIND)
				{
					m_eElementalReaction = REACT_SWIRL_ELECTRIC;
					m_bHostElement ^= (1 << ELEMENT_ELECTRIC);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_FROST)
				{
					m_eElementalReaction = REACT_SUPERCONDUCT;
					m_bHostElement ^= (1 << ELEMENT_ELECTRIC);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_EARTH)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_ELECTIRC;
					m_bHostElement ^= (1 << ELEMENT_ELECTRIC);
					return S_OK;
				}
			}

			else if (i == ELEMENT_FROST) // 얼음 원소가 호스트일때
			{
				if (m_eReceiveElement == ELEMENT_FIRE)
				{
					m_eElementalReaction = REACT_MELT;
					m_bHostElement ^= (1 << ELEMENT_FROST);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WATER)
				{
					m_eElementalReaction = REACT_FROZEN;
					m_bHostElement ^= (1 << ELEMENT_FROST);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WIND)
				{
					m_eElementalReaction = REACT_SWIRL_FROST;
					m_bHostElement ^= (1 << ELEMENT_FROST);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_ELECTRIC)
				{
					m_eElementalReaction = REACT_SUPERCONDUCT;
					m_bHostElement ^= (1 << ELEMENT_FROST);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_EARTH)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_FROST;
					m_bHostElement ^= (1 << ELEMENT_FROST);
					return S_OK;
				}
			}

			else if (i == ELEMENT_EARTH) // 바위 원소가 호스트일때
			{
				if (m_eReceiveElement == ELEMENT_FIRE)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_FIRE;
					m_bHostElement ^= (1 << ELEMENT_EARTH);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_WATER)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_WATER;
					m_bHostElement ^= (1 << ELEMENT_EARTH);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_ELECTRIC)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_ELECTIRC;
					m_bHostElement ^= (1 << ELEMENT_EARTH);
					return S_OK;
				}

				if (m_eReceiveElement == ELEMENT_FROST)
				{
					m_eElementalReaction = REACT_CRYSTALLIZE_FROST;
					m_bHostElement ^= (1 << ELEMENT_EARTH);
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}

_bool CPart_Body::isFunctionTrue()
{
	return m_pModelCom->isFunctionTrue();
}

HRESULT CPart_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Body::Initialize_Clone(void* pArg)
{
	PARTBODY_DESC* pDesc = static_cast<PARTBODY_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	m_eTypeID = pDesc->eType;

	if (m_eTypeID == CTY_PLAYER)
	{
		m_pParentController = pDesc->pController;
		Safe_AddRef(m_pParentController);

		m_pParentTransformCom = static_cast<CTransform*>(pDesc->pTransform);
		Safe_AddRef(m_pParentTransformCom);

		m_pParentFSMCom = static_cast<CFiniteStateMachine*>(pDesc->pFiniteStateMachineCom);
		Safe_AddRef(m_pParentFSMCom);
	}
	else if (m_eTypeID == CTY_MONSTER)
	{
		m_pParentController = pDesc->pController;
		Safe_AddRef(m_pParentController);

		m_pParentTransformCom = static_cast<CTransform*>(pDesc->tPartObjDesc.pParent->Find_Component(L"Com_Transform"));
		Safe_AddRef(m_pParentTransformCom);

		m_pParentFSMCom = static_cast<CFiniteStateMachine*>(pDesc->tPartObjDesc.pParent->Find_Component(L"Com_FiniteStateMachine"));
		Safe_AddRef(m_pParentFSMCom);
	}
	else
	{
		m_pParentTransformCom = static_cast<CTransform*>(pDesc->tPartObjDesc.pParent->Find_Component(L"Com_Transform"));
		Safe_AddRef(m_pParentTransformCom);

		m_pParentFSMCom = static_cast<CFiniteStateMachine*>(pDesc->tPartObjDesc.pParent->Find_Component(L"Com_FiniteStateMachine"));
		Safe_AddRef(m_pParentFSMCom);

		m_eCreatureID = pDesc->tCreatureID;
	}

	return S_OK;
}

HRESULT CPart_Body::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	if (m_eCreatureID == MONSTER_DVALIN)
	{
		_matrix Matrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("hitbox_Seal01")) *
			XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(&m_CombinedWorldMatrix);
		XMStoreFloat4(&m_vBoneWorldPosition, Matrix.r[3]);
	}

	if (m_bPetrification == TRUE)
	{
		if (m_fPetrificationTime < 8.f)
			m_fPetrificationTime += fTimeDelta;
		else
		{
			m_bPetrification = FALSE;
			m_fPetrificationTime = 0.f;
		}
	}

	if (m_bFreezing == TRUE)
	{
		if (m_fFreezingTime < 8.f)
			m_fFreezingTime += fTimeDelta;
		else
		{
			m_bFreezing = FALSE;
			m_fFreezingTime = 0.f;
		}
	}

	return S_OK;
}

HRESULT CPart_Body::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Invalidate_Animation(fTimeDelta)))
		return E_FAIL;

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	XMStoreFloat4x4(&m_CombinedWorldMatrix, 
		XMMatrixTranslation(m_vRootTranslation.m128_f32[0], 0.f, m_vRootTranslation.m128_f32[2]) * XMLoadFloat4x4(&m_CombinedWorldMatrix));

	return S_OK;
}

HRESULT CPart_Body::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	if (m_eCreatureID == MONSTER_ANDRIUS || m_eCreatureID == MONSTER_EFFIGY_FIRE || m_eCreatureID == MONSTER_DVALIN)
	{
		if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3], 90.f))
		{
			if (m_bRenderEnable == TRUE)
			{
				m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
				m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);
			}
#ifdef _DEBUG
			//m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
		}
	}
	else
	{
		if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3], 5.f))
		{
			if (m_bRenderEnable == TRUE)
			{
				m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
				m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);
			}
#ifdef _DEBUG
			//m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
		}
	}

	return S_OK;
}

HRESULT CPart_Body::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		/* Exception Effect Eye */
		if (m_eTypeID == CTY_PLAYER && (m_iFaceMeshIndex - 1 == siMesh || m_iFaceMeshIndex - 2 == siMesh))
			continue;

		if (m_eTypeID == CTY_MONSTER && m_iFaceMeshIndex - 1 == siMesh)
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_LightMapTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_LIGHTMAP)))
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = FALSE), sizeof _bool)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bLightMapTexture", &(bLightMap = TRUE), sizeof _bool)))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bFaceMesh", &(bFaceMesh = (m_iFaceMeshIndex == siMesh)), sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bFreezing == TRUE) // 빙결
		{
			if (FAILED(m_pFreezingCom->Bind_ShaderResource(m_pShaderCom, "g_FreezingDiffuseTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pFreezingCom->Bind_ShaderResource(m_pShaderCom, "g_FreezingNoiseTexture", 1)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_FREEZING)))
				return E_FAIL;
		}
		else if(m_bPetrification == TRUE) // 석화
		{
			if (FAILED(m_pPetrificationCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_PetrificationTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PETRIFICATION)))
				return E_FAIL;
		}
		else
		{
			if (m_fDissolve == 0.f)
			{
				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fDissolve, sizeof _float)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveSize", &m_fDissolveSize, sizeof _float)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveColor", &m_vDissolveColor, sizeof _float3)))
					return E_FAIL;

				if (FAILED(m_pDissolveCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_DissolveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DISSOLVE)))
					return E_FAIL;
			}
		}

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPart_Body::Render_Trail()
{
	if (FAILED(Bind_ShaderResources_Trail()))
		return E_FAIL;

	/* Shader */
	if (FAILED(m_pTrailShaderCom->Begin(TRAIL_ANDRIUS)))
		return E_FAIL;

	/* VIBuffer */
	if (FAILED(m_pTrailBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Body::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;
	
		if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_SHADOW)))
			return E_FAIL;
	
		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

_float CPart_Body::Get_Damage()
{
	_float fDamage = m_fReceiveDamage;
	m_fReceiveDamage = 0.f;

	return fDamage;
}

FSM_OBJECT_STATE CPart_Body::Get_ReceiveState()
{
	FSM_OBJECT_STATE eState = m_eReceiveState;
	m_eReceiveState = FSM_OBJECT_END;

	return eState;
}

ELEMENT CPart_Body::Get_ReceiveElement()
{
	return m_eReceiveElement;
}

ELEMENTAL_REACTION CPart_Body::Get_ElementalReaction()
{
	ELEMENTAL_REACTION eReaction = m_eElementalReaction;
	m_eElementalReaction = REACT_END;

	return eReaction;
}

_bool CPart_Body::isHostElement(ELEMENT eElement)
{
	if ((m_bHostElement & (1 << eElement)) != 0)
		return TRUE;

	return FALSE;
}

void CPart_Body::Set_Damage(_float fDamage, ELEMENT eElement, FSM_OBJECT_STATE eForceState, _bool isPetrification)
{
	m_fReceiveDamage = fDamage;
	m_eReceiveState = eForceState;
	m_eReceiveElement = eElement;
	
	if (isPetrification == TRUE)
	{
		m_fPetrificationTime = 0.f;
		m_bPetrification = isPetrification;
	}

	if (eElement != ELEMENT_END && m_pParentTransformCom != nullptr)
	{
		_vector vPosition = {};
		if ((m_eCreatureID == MONSTER_DVALIN))
			vPosition = XMLoadFloat4(&m_vBoneWorldPosition);
		else
			vPosition = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);

		CElemental_Manager::Get_Instance()->Create_Elemental(vPosition, 1, eElement, CElemental_Manager::ELEMENTAL_PARTICLE);
	}

	if (!FAILED(CPart_Body::ElementalReaction()))
		return;

	if (eElement != ELEMENT_END && (m_bHostElement & (1 << eElement)) == 0)
		m_bHostElement |= (1 << eElement);
}

_vector CPart_Body::Get_Position()
{ 
	return m_pParentTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CPart_Body::Set_CreatureID(CREATUREID eID)
{
	m_eCreatureID = eID;

#pragma region Face Mesh Index
	switch (m_eCreatureID)
	{
	case Client::CHARACTER_TRAVELER:
		m_iFaceMeshIndex	= FACE_TRAVELER;
		m_iFaceEyeMeshIndex = FACEEYE_TRAVELER;
		break;
	case Client::CHARACTER_BARBARA:
		m_iFaceMeshIndex	= FACE_BARBARA;
		m_iFaceEyeMeshIndex = FACEEYE_BARBARA;
		break;
	case Client::CHARACTER_DILUC:
		m_iFaceMeshIndex	= FACE_DILUC;
		m_iFaceEyeMeshIndex = FACEEYE_DILUC;
		break;
	case Client::CHARACTER_RAIDEN:
		m_iFaceMeshIndex	= FACE_RAIDEN;
		m_iFaceEyeMeshIndex = FACEEYE_RAIDEN;
		break;
	case Client::CHARACTER_ZHONGLI:
		m_iFaceMeshIndex	= FACE_ZHONGLI;
		m_iFaceEyeMeshIndex = FACEEYE_ZHONGLI;
		break;
	case Client::NPC_PAIMON:
		m_iFaceMeshIndex	= FACE_PAIMON;
		m_iFaceEyeMeshIndex = FACEEYE_PAIMON;
		break;
	default:
		m_iFaceMeshIndex	= -1;
		m_iFaceEyeMeshIndex = -1;
		break;
	}
}

void CPart_Body::Set_HostElement(ELEMENT eElement, _bool bSet)
{
	if (bSet == FALSE) // 호스트 해제
	{
		_int iTemp = static_cast<_int>(pow(2, (_int)ELEMENT_END)) - 1;
		m_bHostElement &= (iTemp - (1 << eElement));
		return;
	}

	m_eReceiveElement = eElement;

	if (!FAILED(CPart_Body::ElementalReaction()))
		return;

	if (eElement != ELEMENT_END && (m_bHostElement & (1 << eElement)) == 0)
		m_bHostElement |= (1 << eElement);
}

_vector CPart_Body::Get_BodyWorldPosition()
{
	_matrix Matrix = XMLoadFloat4x4(&m_CombinedWorldMatrix);

	return Matrix.r[3];
}

CPart_Body* CPart_Body::Create_Body(CREATUREID eID, PARTBODY_DESC tBodyDesc)
{
	CPart_Body* pBody = { nullptr };

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	switch (eID)
	{
	case CREATUREID::CHARACTER_TRAVELER:
		tBodyDesc.tModelComponent.strComponentTag = L"";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Traveler"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::CHARACTER_BARBARA:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Barbara";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Barbara"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::CHARACTER_DILUC:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Diluc";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Diluc"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::CHARACTER_RAIDEN:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Raiden";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Raiden"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::CHARACTER_ZHONGLI:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Zhongli";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Zhongli"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_ANDRIUS:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Andrius";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Andrius"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_SLIME_WATER:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Slime_Water";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Slime_Water"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_SLIME_ICE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Slime_Ice";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Slime_Ice"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_HILI_NORMAL:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Hili_Normal";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Hili_Normal"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_PAIMON:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Paimon";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Paimon"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_KATHERYNE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Katheryne";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Katheryne"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_AMBOR:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Ambor";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Ambor"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_BRUTE_SHIELD:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Brute";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Brute"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_BRUTE_AXE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Brute";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Brute"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_EFFIGY_FIRE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Effigy_Fire";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Effigy_Fire"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_DVALIN:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Dvalin";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Dvalin"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_ABYSS_MAGE_ELEC:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Abyss_Mage";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Abyss_Mage"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_ABYSS_MAGE_ICE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Abyss_Mage";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Abyss_Mage"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_ABYSS_MAGE_WATER:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Abyss_Mage";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Abyss_Mage"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::MONSTER_ABYSS_MAGE_FIRE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Abyss_Mage";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Abyss_Mage"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_SWAN:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Swan";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_Swan"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_LAWRENCE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Lawrence";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_GUY:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Guy";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_MILES:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Miles";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_RAYMOND:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Raymond";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_JILLIANA:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Jilliana";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_JACK:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Jack";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_LYNN:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Lynn";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_PALLAD:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Pallad";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_STANLEY:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Stanley";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_MARJORIE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Marjorie";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_MARGARET:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Margaret";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_SARA:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Sara";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_WAGNER:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Wagner";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_QUINN:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Quinn";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_BLANCHE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Blanche";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_BEATRICE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Beatrice";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_EURY:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Eury";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_DONNA:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Donna";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_NORMA:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Norma";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_ANTHONY:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Anthony";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_CHARLES:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Charles";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_ANNA:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Anna";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_GLORY:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Glory";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_SUSIE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Susie";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_ALBERT:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Albert";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_ARAMIS:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Aramis";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_SCHULZ:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Schulz";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_TIMAEUS:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Timaeus";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::NPC_MICHELLE:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Michelle";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"))->Clone(&tBodyDesc));
		break;
	case CREATUREID::TELEPORT:
		tBodyDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_TelePort";
		pBody = static_cast<CPart_Body*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Body_TelePort"))->Clone(&tBodyDesc));
		break;
	}

	RELEASE_INSTANCE(CGameInstance);

	return pBody;
}

void CPart_Body::Free()
{
	__super::Free();

	Safe_Release(m_pParentController);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTrailShaderCom);
	Safe_Release(m_pTrailTextureCom);
	Safe_Release(m_pTrailBufferCom);
	Safe_Release(m_pTrailBufferCom2);
	Safe_Release(m_pFaceShadowTexCom);
	Safe_Release(m_pParentTransformCom);
	Safe_Release(m_pParentFSMCom);
}