#include "Instancing.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Model_Instancing.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Bounding_OBB.h"

CInstancing::CInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInstancing::CInstancing(const CInstancing& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInstancing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstancing::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancing::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CInstancing::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CInstancing::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CInstancing::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_int i = 0; i < INSTANCE_END; i++) 
	{
		_uint iNumInstanceMeshes = m_pVIBufferCom[i]->Get_NumMeshes();

		for (_uint j = 0; j < iNumInstanceMeshes; j++)
		{
			if (FAILED(m_pVIBufferCom[i]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", j, TEXTURETYPE::aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom[i]->Render(j)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CInstancing::Add_Instance(INSTANCE_TYPE eType, _float4x4 vInstanceWorld)
{
	if (nullptr == m_pVIBufferCom[eType])
		return E_FAIL;

	return m_pVIBufferCom[eType]->Add_Instance(vInstanceWorld);
}

HRESULT CInstancing::Load_InstanceData(INSTANCE_TYPE eType, const wstring& strInstanceDataPath)
{
	if (nullptr == m_pVIBufferCom[eType])
		return E_FAIL;

	return m_pVIBufferCom[eType]->Load_InstanceData(strInstanceDataPath);
}

HRESULT CInstancing::Save_InstanceData(INSTANCE_TYPE eType, const wstring& strInstanceDataPath)
{
	if (nullptr == m_pVIBufferCom[eType])
		return E_FAIL;

	return m_pVIBufferCom[eType]->Save_InstanceData(strInstanceDataPath);
}

HRESULT CInstancing::Clear_InstanceData(INSTANCE_TYPE eType)
{
	return m_pVIBufferCom[eType]->Clear_InstanceData();
}

HRESULT CInstancing::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Instancing"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_LUSH] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdLGrass01/Stages_MdLGrass01.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_NORMAL] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdLGrass02/Stages_MdLGrass02.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_SILVER] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdLGrass03/Stages_MdLGrass03.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_PINKTHREELEAF] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Dq_Grass_HZD_RaioCotterBud_01/Area_Dq_Grass_HZD_RaioCotterBud_01.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_BLUEONELEAF] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Dq_Grass_HZD_Bcc_01/Area_Dq_Grass_HZD_Bcc_01.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_PINKLONG] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Dq_Grass_HZD_Zc_03/Area_Dq_Grass_HZD_Zc_03.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BLUE] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Xm_Grass_Common_Sjc_01/Area_Xm_Grass_Common_Sjc_01.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_TORAN] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Xm_Grass_CWSM_Fulu_01/Area_Xm_Grass_CWSM_Fulu_01.dat"));
	//m_pVIBufferCom[INSTANCE_GRASS_LUSHLEAF] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdGrass_28/Stages_MdGrass_28.dat"));

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y1] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower01_Vo/Area_MdProps_Flower01_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower04/Area_MdProps_Flower04.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y2] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower05_Vo/Area_MdProps_Flower05_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_R] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower06/Area_MdProps_Flower06.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR_LOW] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower08/Area_MdProps_Flower08.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YB] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower09_Vo/Area_MdProps_Flower09_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BR] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower10/Area_MdProps_Flower10.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBW] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower11_Vo/Area_MdProps_Flower11_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B1] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower12_Vo/Area_MdProps_Flower12_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PUPLE] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_Exterior_Dq_Plant_Flower_Oasis_01/Homeworld_Exterior_Dq_Plant_Flower_Oasis_01.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_NAPAL_R] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_01_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_01_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O1] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_02_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_02_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O2] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_03_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_03_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PINK] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_04_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_04_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B2] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_07_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_07_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_SUN] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_10_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_10_Vo.dat"));
	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y3] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_12_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_12_Vo.dat"));

	return S_OK;
}

HRESULT CInstancing::Bind_ShaderResources()
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

CInstancing* CInstancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstancing* pInstance = new CInstancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CInstancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInstancing::Clone(void* pArg)
{
	CInstancing* pInstance = new CInstancing(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CInstancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstancing::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	for (auto& pVIBufferCom : m_pVIBufferCom)
		Safe_Release(pVIBufferCom);
}
