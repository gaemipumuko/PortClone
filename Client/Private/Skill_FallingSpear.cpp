#include "stdafx.h"
#include "Camera.h"
#include "FX_ShockWave.h"
#include "FallingSpearTrail.h"
#include "Skill_FallingSpear.h"

#include "GameInstance.h"

CSkill_FallingSpear::CSkill_FallingSpear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_FallingSpear::CSkill_FallingSpear(const CSkill_FallingSpear& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_FallingSpear::Add_Component(void* pArg)
{
	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kunwu"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
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

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		TEXT("Com_Texture"), (CComponent**)&m_pDissolveCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_FallingSpear::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fDissolve, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveSize", &m_fDissolveSize, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveColor", &m_vDissolveColor, sizeof _float3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vAddColor", &m_vAddColor, sizeof _float3)))
		return E_FAIL;

	if (FAILED(m_pDissolveCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_DissolveTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_FallingSpear::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_FallingSpear::Initialize_Clone(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CSkill_FallingSpear::Add_Component(pArg)))
		return E_FAIL;

	SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
	XMStoreFloat4(&m_vDropPoint, pDesc->vSpawnPos);
	m_fCreateDelay = pDesc->fCreateDelay;
	m_fHeight = 10.f;
	m_fLifeTime = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos + XMVectorSet(0.f, m_fHeight, 0.f, 0.f));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-90.f));

	m_vAddColor = _float3(m_fAddColor, m_fAddColor, m_fAddColor * 0.5f);

	return S_OK;
}

HRESULT CSkill_FallingSpear::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_FallingSpear::Tick(_float fTimeDelta)
{
	m_fCreateDelay -= fTimeDelta;
	if (m_fCreateDelay > 0.f)
		return S_OK;

	if (m_bCreate == FALSE)
	{
		m_bCreate = TRUE;
		CFallingSpearTrail::FALLINGSPEARTRAIL_DESC tTrailDesc = {};
		tTrailDesc.pParentTransformCom = m_pTransformCom;
		tTrailDesc.fHeightCorrection = -1.f;
		tTrailDesc.fTrailSpeed = 1.5f;
		tTrailDesc.vScale = _float3(1.2f, 6.f, 1.f);

		tTrailDesc.bRotation = FALSE;
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_FallingSpearTrail", L"Layer_Effect", &tTrailDesc)))
			return E_FAIL;

		tTrailDesc.bRotation = TRUE;
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_FallingSpearTrail", L"Layer_Effect", &tTrailDesc)))
			return E_FAIL;
	}

	if (m_fHeight > 1.f)
	{
		m_fHeight -= fTimeDelta * 30.f;
		if (m_fHeight < 1.f)
			m_fHeight = 1.f;

		_vector vPos = XMLoadFloat4(&m_vDropPoint) + XMVectorSet(0.f, m_fHeight, 0.f, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else
	{
		if (m_bEffect == FALSE)
		{
			m_bEffect = TRUE;

			CFX_ShockWave::SW_DESC tSWDesc = { };
			tSWDesc.fScale			= 0.2f;
			tSWDesc.ePosType		= CAssetFX::ASFX_DESC::POS_FIX;
			tSWDesc.vInitPosition	= XMLoadFloat4(&m_vDropPoint);
			tSWDesc.fDuration		= 0.5f;

			m_pGameInstance->Play_AssetFX(L"ShockWave", Get_ParticleTag(L"ShockWave"), &tSWDesc);
		}
		m_fLifeTime -= fTimeDelta;
		m_fAddColor = m_fLifeTime * 0.5f;
		if (m_fLifeTime <= 0.f)
		{
			if (m_fDissolve < 1.f)
			{
				m_fDissolve += 0.08f;
				m_fAddColor = m_fDissolve * 0.5f;
			}
			else
			{
				m_fDissolve = 1.f;
				Set_Destroy(TRUE);
			}
		}

		m_vAddColor = _float3(m_fAddColor, m_fAddColor, m_fAddColor * 0.5f);
	}
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());


	return S_OK;
}

HRESULT CSkill_FallingSpear::LateTick(_float fTimeDelta)
{
	if (m_fCreateDelay > 0.f)
		return S_OK;

	if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&m_WorldMatrix).r[3], 5.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_FallingSpear::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_LightMapTexture", i, TEXTURETYPE::aiTextureType_LIGHTMAP)))
			return E_FAIL;

		/*if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(VTXMESH_DISSOLVE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_FallingSpear* CSkill_FallingSpear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FallingSpear* pInstance = new CSkill_FallingSpear(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_FallingSpear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FallingSpear::Clone(void* pArg)
{
	CSkill_FallingSpear* pInstance = new CSkill_FallingSpear(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_FallingSpear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FallingSpear::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDissolveCom);
}
