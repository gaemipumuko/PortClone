#include "stdafx.h"
#include "Part_Weapon.h"

#include "Camera.h"
#include "Creature.h"

#include "GameInstance.h"

CPart_Weapon::CPart_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPart_Weapon::CPart_Weapon(const CPart_Weapon& rhs)
	: CPartObject(rhs)
{
}

HRESULT CPart_Weapon::Add_Component(void* pArg)
{
	PARTWEAPON_DESC* pDesc = static_cast<PARTWEAPON_DESC*>(pArg);

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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"), /* ±âº» */
		TEXT("Com_TrailTexture"), (CComponent**)&m_pTrailTextureCom)))
		return E_FAIL;

	/* For.Com_TrailBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_TrailBuffer"), (CComponent**)&m_pTrailBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Weapon::Bind_ShaderResources()
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

HRESULT CPart_Weapon::Bind_ShaderResources_Trail()
{
	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTrailTextureCom->Bind_ShaderCurrentTexture(m_pTrailShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Weapon::Initialize_Clone(void* pArg)
{
	PARTWEAPON_DESC* pDesc = static_cast<PARTWEAPON_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(&pDesc->tPartObjDesc)))
		return E_FAIL;

	if (FAILED(CPart_Weapon::Add_Component(pArg)))
		return E_FAIL;

	m_pModelCom->Set_PartRootBone("RootNode", pDesc->pBone);

	m_eTypeID = pDesc->eType;

	if (m_eTypeID == CTY_PLAYER)
	{
		m_pParentTransformCom = static_cast<CTransform*>(pDesc->pTransform);
		Safe_AddRef(m_pParentTransformCom);

		m_pParentFSMCom = static_cast<CFiniteStateMachine*>(pDesc->pFiniteStateMachineCom);
		Safe_AddRef(m_pParentFSMCom);
	}
	else
	{
		m_pParentTransformCom = static_cast<CTransform*>(pDesc->tPartObjDesc.pParent->Find_Component(L"Com_Transform"));
		Safe_AddRef(m_pParentTransformCom);

		m_pParentFSMCom = static_cast<CFiniteStateMachine*>(pDesc->tPartObjDesc.pParent->Find_Component(L"Com_FiniteStateMachine"));
		Safe_AddRef(m_pParentFSMCom);
	}

	return S_OK;
}

HRESULT CPart_Weapon::PreviousTick(_float fTimeDelta)
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
			m_fDissolve += 0.01f;
		else
		{
			m_fDissolve = 1.f;
		}
	}

	return S_OK;
}

HRESULT CPart_Weapon::Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		m_pTransformCom->Get_WorldMatrix() *
		m_pParentTransformCom->Get_WorldMatrix());

	return S_OK;
}
 
HRESULT CPart_Weapon::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3], 5.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
		//m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);
#ifdef _DEBUG
		//m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
	}

	return S_OK;
}

HRESULT CPart_Weapon::Render()
{
	if (FAILED(CPart_Weapon::Bind_ShaderResources()))
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

HRESULT CPart_Weapon::Render_Trail()
{
	if (FAILED(Bind_ShaderResources_Trail()))
		return E_FAIL;

	/* Shader */
	if (FAILED(m_pTrailShaderCom->Begin(TRAIL_DEFAULT)))
		return E_FAIL;

	/* VIBuffer */
	if (FAILED(m_pTrailBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Weapon::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
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

void CPart_Weapon::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_PLAYER_WEAPON, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Weapon COLL_PLAYER_WEAPON" << endl;
	else
		cout << "Release Weapon COLL_PLAYER_WEAPON" << endl;
}

CPart_Weapon* CPart_Weapon::Create_Weapon(WEAPONID eID, PARTWEAPON_DESC tWeaponDesc)
{
	CPart_Weapon* pWeapon = { nullptr };

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	switch (eID)
	{
	case WEAPON_WOLFMOUND:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("WeaponR");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Wolfmound";
		//tWeaponDesc.tTrailComponent.strComponentTag = L"Prototype_Component_Texture_Trail_Diluc";
		pWeapon = 
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Claymore_Wolfmound"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_KUNWU:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("WeaponR");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Kunwu";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Pole_Kunwu"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_ZEPHYRUS:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("CatalystL");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Zephyrus";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Catalyst_Zephyrus"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_NARUKAMI:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("WeaponR");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Narukami";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Pole_Narukami"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_ICECONE:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("Bone_Body_01");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Slime_IceCone";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Slime_IceCone"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_CLUB:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("WeaponR");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Hili_Club";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Hili_Club"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_SHIELD:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("WeaponL_Dummy");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Brute_Shield";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Brute_Shield"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_AXE:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("WeaponR_Dummy");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Brute_Axe";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Brute_Axe"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_STAFF_ELEC:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("Bip001 R Hand");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Staff_Elec";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Staff_Elec"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_STAFF_ICE:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("Bip001 R Hand");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Staff_Ice";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Staff_Ice"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_STAFF_WATER:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("Bip001 R Hand");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Staff_Water";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Staff_Water"))->Clone(&tWeaponDesc));
		break;
	case WEAPON_STAFF_FIRE:
		tWeaponDesc.pBone = tWeaponDesc.pBody->Get_Bone("Bip001 R Hand");
		tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tWeaponDesc.tModelComponent.strComponentTag = L"Prototype_Component_Model_Staff_Elec";
		pWeapon =
			static_cast<CPart_Weapon*>(pInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Staff_Elec"))->Clone(&tWeaponDesc));
		break;
	}

	RELEASE_INSTANCE(CGameInstance);

	return pWeapon;
}

void CPart_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTrailShaderCom);
	Safe_Release(m_pTrailTextureCom);
	Safe_Release(m_pTrailBufferCom);
	Safe_Release(m_pParentTransformCom);
	Safe_Release(m_pParentFSMCom);
}