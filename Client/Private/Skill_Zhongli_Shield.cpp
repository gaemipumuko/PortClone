#include "stdafx.h"
#include "Skill_Zhongli_Shield.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"

CSkill_Zhongli_Shield::CSkill_Zhongli_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_Zhongli_Shield::CSkill_Zhongli_Shield(const CSkill_Zhongli_Shield& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_Zhongli_Shield::Add_Component(void* pArg)
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

	///* For.Com_Model */
	//if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zhongli_Shield"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;

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

HRESULT CSkill_Zhongli_Shield::Bind_ShaderResources()
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

	_float4 vColor = { 1.f, 0.5f, 0.f, 1.f };
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

HRESULT CSkill_Zhongli_Shield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Zhongli_Shield::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Look_At_Direction(pDesc->vDir);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	//m_pTransformCom->Set_Scale({ 0.015f, 0.03f, 0.015f, 0.f });
	m_pTransformCom->Set_Scale({ 0.02f, 0.02f, 0.02f, 0.f });
	m_fLifeTime = 20.f;

	m_pGameInstance->Add_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli", 7000);

	return S_OK;
}

HRESULT CSkill_Zhongli_Shield::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_Zhongli_Shield::Tick(_float fTimeDelta)
{
	if (m_fCrackDisplay > 0.f)
		m_fCrackDisplay -= fTimeDelta;

	if (m_pGameInstance->Get_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli") <= 0)
		m_pGameInstance->Release_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli");

	if (m_pGameInstance->IsProgress_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli") == FALSE)
	{
		Set_Destroy(TRUE);
		return S_OK;
	}

	if (m_bDead == FALSE)
	{
		m_fLifeTime -= fTimeDelta;
		if (m_fLifeTime <= 0.f)
		{
			m_fLifeTime = 0.f;
			m_bDead = TRUE;
		}
	}
	else
	{
		m_fLifeTime += fTimeDelta;
		if (m_fLifeTime >= 1.f)
		{
			m_pGameInstance->Release_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli");
			Set_Destroy(TRUE);
		}
	}

	if (m_pGameInstance->Key_Down('O'))
	{
		m_fCrackDisplay = 1.f;
		m_pGameInstance->Play_Particle(TEXT("ShieldBreak"), Get_ParticleTag(TEXT("ShieldBreak")), PARTICLE_SHIELDBREAK, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 1.f);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_Zhongli_Shield::LateTick(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[1] += 0.8f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CSkill_Zhongli_Shield::Render()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CFiniteStateMachine* pPlayerFSM = static_cast<CFiniteStateMachine*>((pPlayer->Get_PlayerInfo(iIndex).pFiniteStateMachineCom));
	FSM_STATE eState = static_cast<FSM_STATE>(pPlayerFSM->Get_CurrentState());

	if (pPlayer->Get_CurrentIndex() == PLAYER_TWO && 
		(eState == FSM_ATTACK_CHARGED1 || eState == FSM_ATTACK_CHARGED2))
		return S_OK;

	if (eState == FSM_ELEMENTAL_BURST)
		return S_OK;

	if (FAILED(CSkill_Zhongli_Shield::Bind_ShaderResources()))
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

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_Zhongli_Shield* CSkill_Zhongli_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Zhongli_Shield* pInstance = new CSkill_Zhongli_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Zhongli_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Zhongli_Shield::Clone(void* pArg)
{
	CSkill_Zhongli_Shield* pInstance = new CSkill_Zhongli_Shield(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Zhongli_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Zhongli_Shield::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCrackTextureCom);
}