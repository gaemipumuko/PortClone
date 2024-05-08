#include "stdafx.h"
#include "Skill_Abyss_Mage_Shield.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Abyss_Mage.h"
#include "Player.h"
#include "Effect.h"

CSkill_Abyss_Mage_Shield::CSkill_Abyss_Mage_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_Abyss_Mage_Shield::CSkill_Abyss_Mage_Shield(const CSkill_Abyss_Mage_Shield& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_Abyss_Mage_Shield::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_WaterNoise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Crack"),
		TEXT("Com_Texture_Crack"), (CComponent**)&m_pCrackTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fCrackDisplay, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pCrackTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_CrackIndex)))
		return E_FAIL;

	CAbyss_Mage* pAbyss_Mage = static_cast<CAbyss_Mage*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Abyss_Mage"));
	pAbyss_Mage->Get_CreatureID();

	_float4 vColor = {};
	switch (pAbyss_Mage->Get_CreatureID())
	{
	case MONSTER_ABYSS_MAGE_ELEC:
		vColor = { 1.f, 0.33f, 0.64f, 1.f };
		break;
	case MONSTER_ABYSS_MAGE_ICE:
		vColor = { 1.f, 1.f, 1.f, 1.f };
		break;
	case MONSTER_ABYSS_MAGE_WATER:
		vColor = { 0.f, 0.7255f, 0.9647f, 1.f };
		break;
	case MONSTER_ABYSS_MAGE_FIRE:
		vColor = { 0.9176f, 0.316f, 0.3059f, 1.f };
		break;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	ABYSS_MAGE_SHILED_DESC* pDesc = (ABYSS_MAGE_SHILED_DESC*)pArg;
	m_fTag = pDesc->strTag;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->tSkillDesc.vSpawnPos);
	m_pTransformCom->Look_At_Direction(pDesc->tSkillDesc.vDir);
	

	//m_pTransformCom->Set_Scale({ 0.015f, 0.03f, 0.015f, 0.f });
	m_pTransformCom->Set_Scale({ 0.02f, 0.02f, 0.02f, 0.f });


	m_pGameInstance->Add_StatusEffect(SE_ELEMENT_SHILED, m_fTag);

	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::Tick(_float fTimeDelta)
{
	CAbyss_Mage* pAbyss_Mage = static_cast<CAbyss_Mage*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Abyss_Mage"));
	if (pAbyss_Mage == nullptr)
		return S_OK;

	CTransform* pPlayerTransform
		= static_cast<CTransform*>(m_pGameInstance->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Abyss_Mage"), TEXT("Com_Transform")));

	_vector vAbyss_MagePos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vAbyss_MagePos.m128_f32[1] += 1.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vAbyss_MagePos);

	if (m_pGameInstance->IsProgress_StatusEffect(SE_ELEMENT_SHILED, m_fTag) == FALSE)
	{
		Set_Destroy(TRUE);
		return S_OK;
	}
	
	if (pAbyss_Mage->Get_ShieldHp() <= pAbyss_Mage->Get_ShieldMaxHp() / 2.f)
	{
		m_fCrackDisplay = 1.f;
		//m_pGameInstance->Play_Particle(TEXT("ShieldBreak"), Get_ParticleTag(TEXT("ShieldBreak")), PARTICLE_SHIELDBREAK, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 1.f);
	}

	if (m_pGameInstance->Key_Down('O'))
	{
		//m_fCrackDisplay = 1.f;
		m_pGameInstance->Play_Particle(TEXT("ShieldBreak"), Get_ParticleTag(TEXT("ShieldBreak")), PARTICLE_SHIELDBREAK, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 1.f);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::LateTick(_float fTimeDelta)
{
	CAbyss_Mage* pAbyss_Mage = static_cast<CAbyss_Mage*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Abyss_Mage"));
	if (pAbyss_Mage == nullptr)
		return S_OK;

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CSkill_Abyss_Mage_Shield::Render()
{
	if (FAILED(CSkill_Abyss_Mage_Shield::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			continue;*/

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(VTXMESH_EFF_ELEM_SHIELD)))
			return E_FAIL;

		CAbyss_Mage* pAbyss_Mage = static_cast<CAbyss_Mage*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Abyss_Mage"));

		if (pAbyss_Mage->Get_PartObject(CAbyss_Mage::PART_BODY)->Get_RenderEnable() == TRUE)
		{
			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

CSkill_Abyss_Mage_Shield* CSkill_Abyss_Mage_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Abyss_Mage_Shield* pInstance = new CSkill_Abyss_Mage_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Abyss_Mage_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Abyss_Mage_Shield::Clone(void* pArg)
{
	CSkill_Abyss_Mage_Shield* pInstance = new CSkill_Abyss_Mage_Shield(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Abyss_Mage_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Abyss_Mage_Shield::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCrackTextureCom);
}