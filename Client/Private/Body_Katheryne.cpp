#include "stdafx.h"
#include "Body_Katheryne.h"

#include "Bone.h"
#include "GameInstance.h"

#include "Camera_Main.h"

CBody_Katheryne::CBody_Katheryne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_Body(pDevice, pContext)
{
}

CBody_Katheryne::CBody_Katheryne(const CBody_Katheryne& rhs)
	: CPart_Body(rhs)
{
}

HRESULT CBody_Katheryne::Invalidate_Animation(_float fTimeDelta)
{
	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pParentFSMCom->Get_CurrentState());
	
	_float fRate = 0.f;

	switch (eState)
	{
	case FSM_NPC_IDLE:
		m_pModelCom->Set_Animation(4);
		m_pModelCom->Play_PlayerRootAnimation(TRUE, m_pParentTransformCom, fTimeDelta, m_vRootTranslation);
	break;
	}
	return S_OK;
}

HRESULT CBody_Katheryne::Add_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.vExtents = _float3(0.2f, 0.2f, 0.2f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_Object(COLL_NPC_BODY, &m_iColliderID, m_pColliderCom, this);

	return S_OK;
}

HRESULT CBody_Katheryne::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Katheryne::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Katheryne::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Katheryne::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Katheryne::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Katheryne::Render()
{
	if (FAILED(CPart_Body::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool	bLightMap = { };
	_bool	bFaceMesh = { };
	_float	fRampIndex = { -1.f };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRampIndex", &(fRampIndex = -2.f), sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bPetrification == FALSE)
		{
			if (m_fDissolve == 0.f)
			{
				if (siMesh == 3)
				{
					if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_BLACKHAIR)))
						return E_FAIL;
				}
				else
				{
					if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP)))
						return E_FAIL;
				}
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

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP_DISSOLVE)))
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

void CBody_Katheryne::Release_ColliderID()
{
	HRESULT hr = m_pGameInstance->Release_Collider_Object(COLL_NPC_BODY, m_iColliderID);

	if (FAILED(hr))
		cout << "Failed Katheryne Body COLL_NPC_BODY" << endl;
	else
		cout << "Release Katheryne Body COLL_NPC_BODY" << endl;
}

CBody_Katheryne* CBody_Katheryne::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Katheryne* pInstance = new CBody_Katheryne(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBody_Katheryne"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Katheryne::Clone(void* pArg)
{
	CBody_Katheryne* pInstance = new CBody_Katheryne(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBody_Katheryne"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Katheryne::Free()
{
	__super::Free();
}