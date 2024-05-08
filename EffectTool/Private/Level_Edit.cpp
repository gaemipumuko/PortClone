#include "../Default/framework.h"
#include "Level_Edit.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Free.h"

#define CURRENT_LEVEL LEVEL_EDITOR

CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Edit::Ready_Level()
{
	if (FAILED(Ready_ParticleManager()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	// if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
	// 	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_ParticleManager()
{
	m_pGameInstance->Prepare_ParticleManager(
		static_cast<CTexture*>(m_pGameInstance->Clone_Component_Prototype(CURRENT_LEVEL, TEXT("Prototype_Component_Texture_Particle"))),
		static_cast<CRenderer*>(m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"))));

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC tCameraFreeDesc = {};
	{
		tCameraFreeDesc.fMouseSensor = 0.1f;

		tCameraFreeDesc.tCameraDesc.vEye = _float4(0.0f, 10.0f, -15.0f, 1.0f);
		tCameraFreeDesc.tCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		tCameraFreeDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
		tCameraFreeDesc.tCameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
		tCameraFreeDesc.tCameraDesc.fNear = 0.2f;
		tCameraFreeDesc.tCameraDesc.fFar = 100.f;
	}

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Camera"), strLayerTag, &tCameraFreeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Terrain(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Terrain"), strLayerTag)))
		return E_FAIL;

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
