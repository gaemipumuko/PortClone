#include "stdafx.h"
#include "Shougun_Trail_TEST.h"

#include "GameInstance.h"

#include "Camera.h"

CShougun_Trail_TEST::CShougun_Trail_TEST(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CShougun_Trail_TEST::CShougun_Trail_TEST(const CShougun_Trail_TEST& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CShougun_Trail_TEST::Add_Component(void* pArg)
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	switch (m_iTrail)
	{
	case 0:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_0"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_1"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_2"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_3"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_4"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 5:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_5"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 6:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_6"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 7:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_7"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 8:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_8"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 9:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_9"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}


	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Universe"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShougun_Trail_TEST::Bind_ShaderResources()
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

HRESULT CShougun_Trail_TEST::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShougun_Trail_TEST::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SHOGUN_TRAIL_DESC* pDesc = (SHOGUN_TRAIL_DESC*)pArg;
		_vector vSpawnPos	= pDesc->vTrailPos;
		m_iTrail			= pDesc->iTrailIndex;
		m_pParentTransform	= pDesc->pParentTransform;

		if (m_iTrail > 9)
			m_iTrail = 0;

		if (FAILED(Add_Component(pArg)))
			return E_FAIL;

		_vector vParentRight = m_pParentTransform->Get_State(CTransform::STATE_RIGHT);
		_vector vParentUp = m_pParentTransform->Get_State(CTransform::STATE_UP);
		_vector vParentLook = m_pParentTransform->Get_State(CTransform::STATE_LOOK);

		m_pTransformCom->RotationFromCurrentState(vParentRight,XMConvertToRadians(m_pGameInstance->Get_Int_Random(0,359)));
		m_pTransformCom->RotationFromCurrentState(vParentUp,XMConvertToRadians(m_pGameInstance->Get_Int_Random(0,359)));
		m_pTransformCom->RotationFromCurrentState(vParentLook,XMConvertToRadians(m_pGameInstance->Get_Int_Random(0,359)));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSpawnPos);
	}
	else
	{
		if (FAILED(Add_Component(pArg)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	m_pTransformCom->Set_Scale({ 0.02f, 0.02f, 0.02f, 0.f });



	m_fLifeTime = 1.f;


	return S_OK;
}

HRESULT CShougun_Trail_TEST::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CShougun_Trail_TEST::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta*6.f;


	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CShougun_Trail_TEST::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CShougun_Trail_TEST::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 2)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc,sizeof(_float))))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(VTXMESH_TRAIL_TEST)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CShougun_Trail_TEST* CShougun_Trail_TEST::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShougun_Trail_TEST* pInstance = new CShougun_Trail_TEST(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CShougun_Trail_TEST"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShougun_Trail_TEST::Clone(void* pArg)
{
	CShougun_Trail_TEST* pInstance = new CShougun_Trail_TEST(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CShougun_Trail_TEST"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShougun_Trail_TEST::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}