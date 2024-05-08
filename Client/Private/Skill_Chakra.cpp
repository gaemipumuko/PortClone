#include "stdafx.h"
#include "Skill_Chakra.h"
#include "GameInstance.h"
#include "Camera.h"
#include "PartObject.h"
#include "Player.h"
#include "Effect.h"

CSkill_Chakra::CSkill_Chakra(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_Chakra::CSkill_Chakra(const CSkill_Chakra& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_Chakra::Add_Component(void* pArg)
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

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chakra"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chakra"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Chakra::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CSkill_Chakra::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Chakra::Initialize_Clone(void* pArg)
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
	m_pTransformCom->Set_Scale({ 0.1f, 0.1f, 0.1f, 0.f });
	m_fLifeTime = 3.f;

	return S_OK;
}

HRESULT CSkill_Chakra::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_Chakra::Tick(_float fTimeDelta)
{
	if (m_fLifeTime > 0.f)
		m_fLifeTime -= fTimeDelta;
	else
		m_bDestroy = TRUE;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
	vPlayerPos -= vPlayerLook * 0.3f;
	vPlayerPos.m128_f32[1] += 1.3f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Look_At_Direction(vPlayerLook);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_Chakra::LateTick(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
	vPlayerPos -= vPlayerLook * 0.3f;
	vPlayerPos.m128_f32[1] += 1.3f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Look_At_Direction(vPlayerLook);

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_Chakra::Render()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CFiniteStateMachine* pPlayerFSM = static_cast<CFiniteStateMachine*>((pPlayer->Get_PlayerInfo(iIndex).pFiniteStateMachineCom));
	FSM_STATE eState = static_cast<FSM_STATE>(pPlayerFSM->Get_CurrentState());

	if (FAILED(CSkill_Chakra::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_CHAKRA)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_Chakra* CSkill_Chakra::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Chakra* pInstance = new CSkill_Chakra(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_Chakra"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Chakra::Clone(void* pArg)
{
	CSkill_Chakra* pInstance = new CSkill_Chakra(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_Chakra"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Chakra::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}