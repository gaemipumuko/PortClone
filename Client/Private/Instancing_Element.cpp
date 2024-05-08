#include "stdafx.h"
#include "Instancing_Element.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Shader.h"
#include "VIBuffer_Model_Instancing.h"
#include "Camera.h"

CInstancing_Element::CInstancing_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CInstancing_Element::Initialize(LEVELID eLevel)
{
	m_eLevel = eLevel;

	if (FAILED(Initialize_Instancing()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancing_Element::Update(_float4* pPosition)
{
	if (nullptr == pPosition)
		return E_FAIL;

	for (_uint i = 0; i < INSTANCE_END; i++) 
	{
		m_pVIBufferCom[i]->Update_InstanceData(pPosition);
	}

	return S_OK;
}

HRESULT CInstancing_Element::Render()
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

HRESULT CInstancing_Element::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewProj", pLightViewProj, iNumArray)))
		return E_FAIL;

	for (_int i = 0; i < INSTANCE_END; i++)
	{
		_uint iNumInstanceMeshes = m_pVIBufferCom[i]->Get_NumMeshes();

		for (_uint j = 0; j < iNumInstanceMeshes; j++)
		{
			if (FAILED(m_pVIBufferCom[i]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", j, TEXTURETYPE::aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom[i]->Render(j)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CInstancing_Element::Initialize_Instancing()
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_LUSH]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceLushGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_NORMAL]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceNormalGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_SILVER]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceSilverGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_PINKTHREELEAF]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstancePinkThreeLeafGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_BLUEONELEAF]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceBlueOneLeafGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_PINKLONG]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstancePinkLongGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BLUE]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceBlueFlowerGrass.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_TORAN]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceToranGrass.dat"))))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y1]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY1.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y2]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY2.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_R]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerR.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR_LOW]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR_LOW.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YB]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYB.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BR]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerBR.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBW]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBW.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B1]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB1.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PUPLE]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPUPLE.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_NAPAL_R]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerNapal_R.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O1]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO1.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O2]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO2.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PINK]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPINK.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B2]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB2.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_SUN]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerSUN.dat"))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y3]->Load_InstanceData(TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY3.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancing_Element::Add_Components()
{
	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Instancing"));

	if (nullptr == m_pShaderCom)
		return E_FAIL;



	m_pVIBufferCom[INSTANCE_GRASS_LUSH] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdLGrass01/Stages_MdLGrass01.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_LUSH])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_NORMAL] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdLGrass02/Stages_MdLGrass02.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_NORMAL])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_SILVER] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Stages_MdLGrass03/Stages_MdLGrass03.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_SILVER])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_PINKTHREELEAF] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Dq_Grass_HZD_RaioCotterBud_01/Area_Dq_Grass_HZD_RaioCotterBud_01.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_PINKTHREELEAF])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_BLUEONELEAF] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Dq_Grass_HZD_Bcc_01/Area_Dq_Grass_HZD_Bcc_01.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_BLUEONELEAF])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_PINKLONG] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Dq_Grass_HZD_Zc_03/Area_Dq_Grass_HZD_Zc_03.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_PINKLONG])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BLUE] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Xm_Grass_Common_Sjc_01/Area_Xm_Grass_Common_Sjc_01.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BLUE])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_TORAN] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_Xm_Grass_CWSM_Fulu_01/Area_Xm_Grass_CWSM_Fulu_01.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_TORAN])
		return E_FAIL;




	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y1] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower01_Vo/Area_MdProps_Flower01_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y1])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower04/Area_MdProps_Flower04.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y2] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower05_Vo/Area_MdProps_Flower05_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y2])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_R] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower06/Area_MdProps_Flower06.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_R])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR_LOW] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower08/Area_MdProps_Flower08.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBR_LOW])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YB] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower09_Vo/Area_MdProps_Flower09_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YB])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BR] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower10/Area_MdProps_Flower10.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_BR])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBW] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower11_Vo/Area_MdProps_Flower11_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_YBW])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B1] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Area_MdProps_Flower12_Vo/Area_MdProps_Flower12_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B1])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PUPLE] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_Exterior_Dq_Plant_Flower_Oasis_01/Homeworld_Exterior_Dq_Plant_Flower_Oasis_01.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PUPLE])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_NAPAL_R] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_01_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_01_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_NAPAL_R])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O1] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_02_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_02_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O1])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O2] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_03_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_03_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_O2])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PINK] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_04_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_04_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_PINK])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B2] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_07_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_07_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_B2])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_SUN] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_10_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_10_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_SUN])
		return E_FAIL;

	m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y3] = CVIBuffer_Model_Instancing::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Map/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_12_Vo/Homeworld_General_Common_Thing_Prop_Irodori_MainFlower_12_Vo.dat"));

	if (nullptr == m_pVIBufferCom[INSTANCE_GRASS_FLOWER_Y3])
		return E_FAIL;

	return S_OK;
}

HRESULT CInstancing_Element::Bind_ShaderResources()
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

CInstancing_Element* CInstancing_Element::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevel)
{
	CInstancing_Element* pInstance = new CInstancing_Element(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eLevel)))
	{
		MSG_BOX(TEXT("Failed to Created : CInstancing_Element"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstancing_Element::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);

	for (auto& pVIBufferCom : m_pVIBufferCom)
		Safe_Release(pVIBufferCom);
}
