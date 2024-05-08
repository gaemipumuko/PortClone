#include "Edit_Object.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "Collider.h"
#include "Bounding_OBB.h"
#include "Edit_Model.h"
#include "GameInstance.h"
#include "Camera.h"

CEdit_Object::CEdit_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEdit_Object::CEdit_Object(const CEdit_Object& rhs)
	: CGameObject(rhs)
{
}

_float4x4 CEdit_Object::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldFloat4x4();
}

HRESULT CEdit_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEdit_Object::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	EDIT_DESC* pEdit_Desc = (EDIT_DESC*)pArg;

	if (true == pEdit_Desc->strModelTag.empty())
		return E_FAIL;

	m_eType = pEdit_Desc->eType;
	m_strModelTag = pEdit_Desc->strModelTag;

	if (FAILED(Add_Components(pEdit_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Object::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEdit_Object::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEdit_Object::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);

	return S_OK;
}

HRESULT CEdit_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEdit_Object::Add_Components(EDIT_DESC* pEditDesc)
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(pEditDesc->vWorldMatrix);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, m_strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC tBounding_Desc = {};
	{
		tBounding_Desc.vExtents = _float3(1.5f, 1.5f, 1.5f);
		tBounding_Desc.vCenter = _float3(0.f, tBounding_Desc.vExtents.y, 0.f);
		tBounding_Desc.vRadians = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Object::Bind_ShaderResources()
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

CEdit_Object* CEdit_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_Object* pInstance = new CEdit_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEdit_Object"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEdit_Object::Clone(void* pArg)
{
	CEdit_Object* pInstance = new CEdit_Object(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEdit_Object"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Object::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
