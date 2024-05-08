#include "stdafx.h"
#include "Part_Glider.h"
#include "Player.h"

#include "Camera.h"
#include "Creature.h"

#include "GameInstance.h"

CPart_Glider::CPart_Glider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPart_Glider::CPart_Glider(const CPart_Glider& rhs)
	: CPartObject(rhs)
{
}

HRESULT CPart_Glider::Add_Component(void* pArg)
{
	PARTGLIDER_DESC* pDesc = static_cast<PARTGLIDER_DESC*>(pArg);

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

	return S_OK;
}

HRESULT CPart_Glider::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CPart_Glider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Glider::Initialize_Clone(void* pArg)
{
	PARTGLIDER_DESC* pDesc = static_cast<PARTGLIDER_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	if (FAILED(CPart_Glider::Add_Component(pArg)))
		return E_FAIL;

	m_pModelCom->Set_PartRootBone("RootNode", pDesc->pBone);

	m_pParentTransformCom = static_cast<CTransform*>(pDesc->pTransform);
	Safe_AddRef(m_pParentTransformCom);

	m_pParentFSMCom = static_cast<CFiniteStateMachine*>(pDesc->pFiniteStateMachineCom);
	Safe_AddRef(m_pParentFSMCom);

	return S_OK;
}

HRESULT CPart_Glider::PreviousTick(_float fTimeDelta)
{
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

	if (m_bForceDissolve == TRUE)
	{
		if (m_fDissolve < 1.f)
			m_fDissolve += 0.005f;
		else
		{
			m_fDissolve = 1.f;
		}
	}

	return S_OK;
}

HRESULT CPart_Glider::Tick(_float fTimeDelta)
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());
	FSM_STATE eFrevState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_PrevState());

	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(m_pParent)->Get_CurrentIndex());
	CModel* pModel = static_cast<CModel*>(static_cast<CPlayer*>(m_pParent)->Get_PartObject(CPlayer::PART_BODY, iIndex)->Find_Component(TEXT("Com_Model")));

	if (eState == FSM_FLY_START)
	{
		m_fMargin = -0.2f;
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
			XMMatrixTranslation(0.f, 1.55f, m_fMargin) *
			m_pParentTransformCom->Get_WorldMatrix());
	}
		
	else if (eFrevState == FSM_FLY_START && pModel->Get_ChangeAnimation() == TRUE)
	{
		m_fMargin = -0.2f;
		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
			XMMatrixTranslation(0.f, 1.55f, m_fMargin) *
			m_pParentTransformCom->Get_WorldMatrix());
	}
	else
	{
		if (m_fMargin >= 0.1f)
			m_fMargin = 0.1f;
		else
			m_fMargin += fTimeDelta * 0.1f;

		XMStoreFloat4x4(&m_CombinedWorldMatrix,
			m_pTransformCom->Get_WorldMatrix() *
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixRotationAxis({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(310.f)) *
			XMMatrixTranslation(0.f, 1.55f, m_fMargin) *
			m_pParentTransformCom->Get_WorldMatrix());
	}
		
	return S_OK;
}

HRESULT CPart_Glider::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3], 5.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);
#ifdef _DEBUG
		//m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
	}

	return S_OK;
}

HRESULT CPart_Glider::Render()
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState != FSM_FLY_START && eState != FSM_FLY_NORMAL && eState != FSM_FLY_LEFT && eState != FSM_FLY_RIGHT)
		return S_OK;

	if (FAILED(CPart_Glider::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if(m_iMeshIndex - 1 == siMesh || m_iMeshIndex - 2 == siMesh)
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

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
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
		else
		{
			if (FAILED(m_pPetrificationCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_PetrificationTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_PETRIFICATION)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPart_Glider::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	FSM_STATE eState = static_cast<FSM_STATE>(m_pParentFSMCom->Get_CurrentState());

	if (eState != FSM_FLY_START && eState != FSM_FLY_NORMAL && eState != FSM_FLY_LEFT && eState != FSM_FLY_RIGHT)
		return S_OK;

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

void CPart_Glider::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_PLAYER_WEAPON, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Glider COLL_PLAYER_WEAPON" << endl;
	else
		cout << "Release Glider COLL_PLAYER_WEAPON" << endl;
}

CPart_Glider* CPart_Glider::Create_Glider(GLIDERID eID, PARTGLIDER_DESC tWeaponDesc)
{
	CPart_Glider* pGlider = { nullptr };

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	switch (eID)
	{
	case GLIDER_ORIGIN:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("+FlycloakRootB CB A01");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Glider_Origin";
		pGlider =
			static_cast<CPart_Glider*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Glider_Origin"))->Clone(&tWeaponDesc));
		break;
	case GLIDER_LIYUE:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("+FlycloakRootB CB A01");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Glider_Liyue";
		pGlider =
			static_cast<CPart_Glider*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Glider_Liyue"))->Clone(&tWeaponDesc));
		break;
	case GLIDER_PSALMUS:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("+FlycloakRootB CB A01");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Glider_Psalmus";
		pGlider =
			static_cast<CPart_Glider*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Glider_Psalmus"))->Clone(&tWeaponDesc));
		break;
	case GLIDER_THUNDERCLAP:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("+FlycloakRootB CB A01");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Glider_Thunderclap";
		pGlider =
			static_cast<CPart_Glider*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Glider_Thunderclap"))->Clone(&tWeaponDesc));
		break;
	}

	RELEASE_INSTANCE(CGameInstance);

	return pGlider;
}

void CPart_Glider::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
	Safe_Release(m_pParentFSMCom);
}