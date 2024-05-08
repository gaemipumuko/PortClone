#include "stdafx.h"
#include "Eff_Barbara_Normal_Star.h"
#include "GameInstance.h"
#include "Camera.h"


CEff_Barbara_Normal_Star::CEff_Barbara_Normal_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_Barbara_Normal_Star::CEff_Barbara_Normal_Star(const CEff_Barbara_Normal_Star& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_Barbara_Normal_Star::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Barbara_Star"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Barbara_Color_Ramp"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	/* For.Com_Collider_SPHERE */

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Star::Bind_ShaderResources()
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

HRESULT CEff_Barbara_Normal_Star::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_Barbara_Normal_Star::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = (EFFECT_DESC*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Look_At_Direction(pDesc->vDir);
		
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({ 0.02f, 0.02f, 0.02f, 0.f });

	m_fLifeTime = 1.f;

	m_fColorRand = m_pGameInstance->Get_Float_Random(0.f,1.f);

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Star::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEff_Barbara_Normal_Star::Tick(_float fTimeDelta)
{

	m_fLifeTime -= fTimeDelta * 1.5f;
	if (m_fLifeTime <= 0.f)
	{
		CEffect::EFFECT_DESC descc = {};
		descc.eEffectID = EFFECT_HITTING_STAR;
		descc.fLifeTime = 1.f;
		_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		descc.vSpawnPos = vSelfPos;
		descc.vScale = { 1.5f, 1.8f, 1.f, 0.f };

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Barbara_Normal_Atk", L"Layer_Effect", &descc)))
			return E_FAIL;

		m_pGameInstance->PlaySoundW(L"Barbara_B_Attack4.mp3");

		Set_Destroy(TRUE);
	}

	m_fTimeAcc += fTimeDelta * 1.5f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Star::LateTick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CEff_Barbara_Normal_Star::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fColorRand, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXMESH_BARBARA_STAR)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;

	}

	return S_OK;
}

CEff_Barbara_Normal_Star* CEff_Barbara_Normal_Star::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_Barbara_Normal_Star* pInstance = new CEff_Barbara_Normal_Star(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_Barbara_Normal_Star"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_Barbara_Normal_Star::Clone(void* pArg)
{
	CEff_Barbara_Normal_Star* pInstance = new CEff_Barbara_Normal_Star(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_Barbara_Normal_Star"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_Barbara_Normal_Star::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}