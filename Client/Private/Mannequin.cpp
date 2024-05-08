#include "stdafx.h"
#include "Mannequin.h"

#include "Camera.h"

#include "GameInstance.h"

#include "Effect.h"

CMannequin::CMannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CMannequin::CMannequin(const CMannequin& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CMannequin::Bind_Animation(_uint iAnimIndex)
{
	_uint iMaxAnimIndex = m_pModelCom->Get_AnimationCount();
	if (iAnimIndex > iMaxAnimIndex)
		return E_FAIL;

	m_iAnimationIndex = iAnimIndex;

	return S_OK;
}

HRESULT CMannequin::Bind_SpecificModel(LEVELID eID, wstring strPrototypeTag)
{
	if (m_pModelCom != nullptr)
	{
		if (FAILED(CGameObject::Release_Component(TEXT("Com_Model"))))
			return E_FAIL;
		Safe_Release(m_pModelCom);
	}

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(eID, strPrototypeTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (strPrototypeTag == L"Prototype_Component_Model_Traveler")
		m_eFaceID = FACE_TRAVELER;
	else if (strPrototypeTag == L"Prototype_Component_Model_Babara")
		m_eFaceID = FACE_BARBARA;
	else if (strPrototypeTag == L"Prototype_Component_Model_Diluc")
		m_eFaceID = FACE_DILUC;
	else if (strPrototypeTag == L"Prototype_Component_Model_Ganyu")
		m_eFaceID = FACE_RAIDEN;
	else if (strPrototypeTag == L"Prototype_Component_Model_Zhongli")
		m_eFaceID = FACE_ZHONGLI;
	else
		m_eFaceID = FACE_END;

	return S_OK;
}

HRESULT CMannequin::Change_Effects()
{
	CEffect::EFFECT_DESC descc = {};
	descc.eEffectID = EFFECT_HITTING_STAR;
	descc.fLifeTime = 0.5f;
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	/*vSelfPos.m128_f32[2] -= 1.f;*/
	vSelfPos.m128_f32[1] += 0.95f;
	//vSelfPos += vSelfLook * 1.f;

	descc.vSpawnPos = vSelfPos;
	descc.vScale = { 0.8f, 0.8f, 1.f, 0.f };

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Eff_Transition", L"Layer_Effect", &descc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CMannequin::Add_Component(void* pArg)
{
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

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		TEXT("Com_Texture"), (CComponent**)&m_pDissolveCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMannequin::Bind_ShaderResources()
{
	_float4x4 CombinedMatrix = m_pTransformCom->Get_WorldFloat4x4();

	CombinedMatrix._41 += m_vRootTranslation.x;
	CombinedMatrix._43 += m_vRootTranslation.z;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
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

HRESULT CMannequin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMannequin::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMannequin::PreviousTick(_float fTimeDelta)
{


	return S_OK;
}

HRESULT CMannequin::Tick(_float fTimeDelta)
{
	if (m_pModelCom != nullptr)
	{
		_vector vTranslation = { };

		m_pModelCom->Set_Animation(m_iAnimationIndex);
		m_pModelCom->Play_RootAnimation(TRUE, m_pTransformCom, fTimeDelta * (m_bPlayAnimation == TRUE ? 1.f : 0.f) * m_fAnimationSpeed, vTranslation);
		//m_pModelCom->Play_Animation_Half(TRUE, m_pTransformCom, fTimeDelta);

		XMStoreFloat4(&m_vRootTranslation, vTranslation);
	}

	//if (m_bForceDissolve == TRUE)
	//{
	//	if (m_fDissolve > 0.f)
	//	{
	//		m_fDissolve -= 0.05f;
	//	}
	//	else
	//	{
	//		m_fDissolve = 0.f;
	//	//	m_bForceDissolve = FALSE;  // 디졸브 효과가 완전히 사라지면 m_bForceDissolve를 FALSE로 설정하여 사라지도록 처리
	//	}
	//}
	//else
	//{
	//	if (m_fDissolve < 1.f)
	//	{
	//		m_fDissolve += 0.05f;
	//	}
	//	else
	//	{
	//		m_fDissolve = 1.f;
	//	}
	//}

	

	return S_OK;
}

HRESULT CMannequin::LateTick(_float fTimeDelta)
{
	if (m_pModelCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CMannequin::Render()
{
	if (FAILED(CMannequin::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool bLightMap = { };
	_bool bFaceMesh = { };
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		/* Exception Effect Eye */
		if (m_eFaceID - 1 == siMesh || m_eFaceID - 2 == siMesh)
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bFaceMesh", &(bFaceMesh = (m_eFaceID == siMesh)), sizeof _bool)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bChangeModel == FALSE)
		{
			if (m_fDissolve == 0.f)
			{
				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP)))
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

				if (FAILED(m_pShaderCom->Begin(VTXANIMMESH_RAMP_DISSOLVE)))
					return E_FAIL;
			}
		}


		if (FAILED(m_pModelCom->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	return S_OK;
}

CMannequin* CMannequin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMannequin* pInstance = new CMannequin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CMannequin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMannequin::Clone(void* pArg)
{
    CMannequin* pInstance = new CMannequin(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CMannequin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMannequin::Free()
{
    __super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDissolveCom);
}
