#include "Level_Edit.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Terrain.h"
#include "Edit_Object.h"
#include "Edit_Spawner.h"
#include "Edit_Water.h"

#define CURRENT_LEVEL LEVEL_EDIT

CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Edit::Ready_Level()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameObjects(TEXT("Layer_GameObjects"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Instancing(TEXT("Layer_Instancing"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Water(TEXT("Layer_Water"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Spawner(TEXT("Layer_Spawner"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Light()
{
	LIGHT_DESC LightDesc = {};

	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_EDIT, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CCamera_Free::CAMERA_FREE_DESC tCameraFreeDesc = {};
	{
		tCameraFreeDesc.fMouseSensor = 0.1f;

		tCameraFreeDesc.tCameraDesc.vEye = _float4(0.0f, 10.0f, -15.0f, 1.0f);
		tCameraFreeDesc.tCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		tCameraFreeDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
		tCameraFreeDesc.tCameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
		tCameraFreeDesc.tCameraDesc.fNear = 0.2f;
		tCameraFreeDesc.tCameraDesc.fFar = 1000.f;
	}

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Camera"), strLayerTag, &tCameraFreeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Terrain(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Terrain"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_GameObjects(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Instancing(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Instancing"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Water(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	//CEdit_Water::WATER_DESC tWaterDesc = {};
	//{
	//	tWaterDesc.iSizeX = 200;
	//	tWaterDesc.iSizeZ = 200;
	//	tWaterDesc.vPosition = _float4(0, 0, 0, 1.0f);
	//}

	//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_EditWater"), strLayerTag, &tWaterDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Spawner(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	//CEdit_Spawner::SPAWNER_DESC tSpawnerDesc = {};
	//{
	//	tSpawnerDesc.fSpawnRange = 3.0f;
	//	tSpawnerDesc.vSpawnerPos = _float4(-5.0f, 0.0f, 0.0f, 1.0f);
	//}

	//if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_Spawner"), strLayerTag, &tSpawnerDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Edit* pLevel = new CLevel_Edit(pDevice, pContext);

	if (FAILED(pLevel->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Edit"));
		Safe_Release(pLevel);
	}

	return pLevel;
}

void CLevel_Edit::Free()
{
	__super::Free();
}
