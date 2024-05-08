#include "stdafx.h"
#include "Gate.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "Model.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Player.h"
#include "InteractionBox.h"
#include "Effect.h"

#include "Level_Loading.h"

CGate::CGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEventCollider(pDevice, pContext)
{
}

CGate::CGate(const CGate& rhs)
	: CEventCollider(rhs)
{
}

HRESULT CGate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGate::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	GATE_DESC* pDesc = (GATE_DESC*)pArg;
	{
		m_eGateType = pDesc->eGateType;
		m_vWorldMatrix = pDesc->vWorldMatrix;
		m_szModelTag = pDesc->szModelTag;
		m_eLevel = pDesc->eLevel;
		m_eReserveLevel = pDesc->eReserveLevel;
	}

	switch (m_eGateType)
	{
	case Client::CGate::GATE_VISION:
		m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
		m_pInteractionBox->Add_InteractionBox(TEXT("비경 진입"), CInteraction_Box::ICON_TALK);
		break;
	case Client::CGate::GATE_DVALIN:
		m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
		m_pInteractionBox->Add_InteractionBox(TEXT("드발린 토벌"), CInteraction_Box::ICON_TALK);
		break;
	}

	if (nullptr == m_pInteractionBox)
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Animation()))
		return E_FAIL;



	return S_OK;
}

HRESULT CGate::PreviousTick(_float fTimeDelta)
{
	m_pInteractionBox->PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CGate::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	switch (m_eGateType)
	{
	case Client::CGate::GATE_VISION:
		Tick_VisionGate(fTimeDelta);
		break;
	case Client::CGate::GATE_DVALIN:
		Tick_DvalinGate(fTimeDelta);
		break;
	}


	return S_OK;
}

HRESULT CGate::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	if (true == isInPlayer() && false == m_bEntry && false == m_bReserveLevel)
		m_pInteractionBox->LateTick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CGate::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	switch (m_eGateType)
	{
	case Client::CGate::GATE_VISION:
		Render_VisionGate();
		break;
	case Client::CGate::GATE_DVALIN:
		Render_DvalinGate();
		break;
	}

	return S_OK;
}

HRESULT CGate::Reserve_Level()
{
	if (LEVEL_END == m_eReserveLevel)
		return E_FAIL;

	switch (m_eReserveLevel)
	{
	case Client::LEVEL_WORLD:
	{
		CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_WORLD);

		if (nullptr == pLevel)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel)))
			return E_FAIL;
	}
		break;
	case Client::LEVEL_TEMPLE_LION:
	{
		CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TEMPLE_LION);

		if (nullptr == pLevel)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel)))
			return E_FAIL;
	}
		break;
	case Client::LEVEL_STRONGHOLD_ABYSSMAGE:
	{
		CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STRONGHOLD_ABYSSMAGE);

		if (nullptr == pLevel)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel)))
			return E_FAIL;
	}
		break;
	case Client::LEVEL_QUEST:
	{
		CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_QUEST);

		if (nullptr == pLevel)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel)))
			return E_FAIL;
	}
		break;
	case Client::LEVEL_STORMTERROR:
	{
		CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STORMTERROR);

		if (nullptr == pLevel)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOADING, pLevel)))
			return E_FAIL;
	}
		break;
	}

	return S_OK;
}

HRESULT CGate::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(m_vWorldMatrix);

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(Add_ShaderCom()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, m_szModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_iModelType = m_pModelCom->Get_ModelType();

	if (GATE_VISION == m_eGateType)
		m_pModelCom->Cooking_StaticModel(m_pTransformCom->Get_WorldFloat4x4());

	/* For.Com_Collider */
	_float3 vMin = _float3(9999999.0f, 9999999.0f, 9999999.0f);
	_float3 vMax = _float3(-9999999.0f, -9999999.0f, -9999999.0f);

	if (FAILED(m_pModelCom->Compute_Extents(&vMin, &vMax)))
		return E_FAIL;

	_float3 vExtents = _float3((vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f);
	_float3 vCenter = _float3((vMax.x + vMin.x) * 0.5f, (vMax.y + vMin.y) * 0.5f, (vMax.z + vMin.z) * 0.5f);

	CBounding_OBB::BOUNDING_OBB_DESC tBounding_Desc = {};
	{
		tBounding_Desc.vExtents = vExtents;
		tBounding_Desc.vCenter = vCenter;
		tBounding_Desc.vRadians = _float3(0.0f, 15.0f, 0.0f);
	}

	if (GATE_DVALIN == m_eGateType)
	{
		tBounding_Desc.vExtents.x *=30.f;
		tBounding_Desc.vExtents.y *=10.f;
		tBounding_Desc.vExtents.z *=40.f;

	}

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
		return E_FAIL;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (GATE_DVALIN == m_eGateType) 
	{
		/* For.Com_Texture_Ground_Blur */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ground_Blur"),
			TEXT("Com_Texture_Ground_Blur"), (CComponent**)&m_pTextureCom[ENTRY_GROUNDBLUR])))
			return E_FAIL;

		/* For.Com_Texture_Ramp_Star */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ramp_Star"),
			TEXT("Com_Texture_Ramp_Star"), (CComponent**)&m_pTextureCom[ENTRY_RAMPSTAR])))
			return E_FAIL;

		/* For.Com_Texture_MultiChannel */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MultiChannel"),
			TEXT("Com_Texture_MultiChannel"), (CComponent**)&m_pTextureCom[ENTRY_MULTICHANNEL])))
			return E_FAIL;

		/* For.Com_Texture_NoiseMulti */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseMulti"),
			TEXT("Com_Texture_NoiseMulti"), (CComponent**)&m_pTextureCom[ENTRY_NOISEMULTI])))
			return E_FAIL;

		/* For.Com_Texture_Turbulence */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Turbulence"),
			TEXT("Com_Texture_Turbulence"), (CComponent**)&m_pTextureCom[ENTRY_TURBULENCE])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGate::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
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

HRESULT CGate::Add_ShaderCom()
{
	switch (m_eGateType)
	{
	case Client::CGate::GATE_VISION:
		/* For.Com_Shader */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case Client::CGate::GATE_DVALIN:
		/* For.Com_Shader */
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case Client::CGate::GATE_END:
		break;
	default:
		break;
	}

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CGate::Initialize_Animation()
{
	/*
	* Vision Gate Animations
	* 4 : Close
	* 5 : Closing
	* 6 : Opening
	* 7 : Open
	* 
	* else : Trash
	* 
	* Dvalin Entry Animations
	* 0 : Idle
	* 
	*/

	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (CModel::MTYPE_ANIM == m_iModelType) 
	{
		switch (m_eGateType)
		{
		case Client::CGate::GATE_VISION:
			m_pModelCom->Set_Animation(4);
			m_bLoop = false;
			break;
		case Client::CGate::GATE_DVALIN:
			//m_pModelCom->Set_Animation(0);
			m_bLoop = true;
			break;
		case Client::CGate::GATE_END:
			break;
		default:
			break;
		}
	}

	return S_OK;
}

_bool CGate::isInPlayer()
{
	_bool bisColl = false;
	CGameObject* pGameObject = m_pGameInstance->Find_Object(m_eLevel, TEXT("Layer_Player"));

	if (nullptr != pGameObject)
	{
		CPlayer* pPlayer = (CPlayer*)pGameObject;

		_int iColliderID = pPlayer->Get_BodyCollisionID();

		vector<CGameObject*> CollisionObjects = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);

		if (false == CollisionObjects.empty())
		{
			CCollider* pCollider = (CCollider*)CollisionObjects.front()->Find_Component(TEXT("Com_Collider_OBB"));

			if (nullptr != pCollider)
			{
				bisColl = m_pColliderCom->Intersect(pCollider);
				return bisColl;
			}
		}
	}

	return bisColl;
}

HRESULT CGate::Tick_VisionGate(_float fTimeDelta)
{
	if (true == isInPlayer() && false == m_bEntry && false == m_bReserveLevel)
	{
		m_pInteractionBox->Tick(fTimeDelta);

		if (m_pGameInstance->Key_Down('F'))
		{
			m_bEntry = true;

			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr)
			{
				pCamera->Set_SceneMode(CAMSCENEID_GATE, SCENE_ONE, 0.f, 1.f, TRUE, m_pTransformCom);
			}

			m_pGameInstance->PlaySoundW(L"DoorOpen.mp3", 1.f, CHANNELID::SOUND_ANNOUNCE);
		}
	}

	if (true == m_bEntry)
	{
		_uint iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimIndex();

		if (6 != iCurrentAnimIndex)
		{
			m_pModelCom->Set_Animation(6);

			CEffect::EFFECT_DESC descc = {};
			_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vSelfPos.m128_f32[1] += 3.f;
			descc.vSpawnPos = vSelfPos;
			descc.vDir = XMVector3Normalize(vSelfLook);
			descc.vScale = { 6.f, 6.f, 1.f, 0.f };

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Door_Portal", L"Layer_Effect", &descc)))
				return E_FAIL;
		}

		if (m_pModelCom->Get_AnimationCurrentDuration() >= m_pModelCom->Get_AnimationDuration(6))
		{
			m_bEntry = false;
			m_bReserveLevel = true;

			m_pModelCom->Set_Animation(7);
		}
	}

	if (true == m_bReserveLevel)
	{
		m_bReserveLevel = false;

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
		{
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
		}

		if (FAILED(Reserve_Level()))
			return E_FAIL;
	}

	m_pModelCom->Play_Animation(m_bLoop, m_pTransformCom, fTimeDelta);

	return S_OK;
}

HRESULT CGate::Tick_DvalinGate(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(m_bLoop, m_pTransformCom, fTimeDelta);

	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, m_fTimeAcc);

	if (true == isInPlayer() && false == m_bEntry && false == m_bReserveLevel)
	{
		m_pInteractionBox->Tick(fTimeDelta);

		if (m_pGameInstance->Key_Down('F'))
		{
			m_bReserveLevel = true;
		}
	}

	if (true == m_bReserveLevel)
	{
		m_bReserveLevel = false;

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
		{
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
		}

		if (FAILED(Reserve_Level()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGate::Render_VisionGate()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (CModel::MTYPE_ANIM == m_iModelType)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DEFAULT)))
				return E_FAIL;
		}

		else if (CModel::MTYPE_NONANIM == m_iModelType)
		{
			if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
				return E_FAIL;
		}

		else
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGate::Render_DvalinGate()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	_bool bVertical = false;
	_float fReversed = -m_fTimeAcc * 3.f;
	_float fStale = 0.f;
	for (_uint i = 0; i < iNumMeshes; i++) 
	{
		if (i == 2)
			continue;
		switch (i)
		{
		case 0:
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fReversed, sizeof(_float))))
				return E_FAIL;
			bVertical = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bVertical", &bVertical, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
			break;
		case 1:
		{
		}
			break;
		case 2:
			break;
		case 3:
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fStale, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
			break;
		case 4:
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fStale, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bVertical", &bVertical, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
			break;
		case 5:
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER_ROT)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
			break;
		case 6:
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;


			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &fReversed, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER_ROT)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
			break;
		case 7:
			//if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			//	return E_FAIL;

			//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			//	continue;
			//if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
			//	return E_FAIL;

			//if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER_ROT)))
			//	return E_FAIL;

			//	if (FAILED(m_pModelCom->Render(i)))
			//	return E_FAIL;
			break;
		case 8:
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
				return E_FAIL;

			bVertical = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_bVertical", &bVertical, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_DVALIN_ENTER)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
			break;
		}
	}

	return S_OK;
}

CGate* CGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGate* pInstance = new CGate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGate::Clone(void* pArg)
{
	CGate* pInstance = new CGate(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGate"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGate::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);

	for (_uint i = 0; i < ENTRY_END; i++) 
	{
		if (nullptr != m_pTextureCom[i])
			Safe_Release(m_pTextureCom[i]);
	}

	Safe_Release(m_pInteractionBox);
}
