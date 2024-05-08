#include "stdafx.h"
#include "Level_StormTerror.h"

#include "Ortho.h"
#include "Camera_Main.h"
#include "Camera_Char.h"

#include "GameInstance.h"

#include "Effect.h"

#include "Ground_Item.h"

#include "SpawnBox.h"
#include "Updraft.h"

#include "SkyBox.h"
#include "Dungeon.h"

#define CURRENT_LEVEL LEVEL_STORMTERROR

CLevel_StormTerror::CLevel_StormTerror(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_StormTerror::Ready_Level()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Init_Scene()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_ConstantBuffer()))
		return E_FAIL;

	if (FAILED(Ready_ParticleManager()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_StormTerror(TEXT("Layer_StormTerror"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Icon(TEXT("Layer_Icon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_EventCollider(TEXT("Layer_EventCollider"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	/* Init RenderMode */
	CDeciderObject::Set_RenderMode(CDeciderObject::RMODE_GAMEPLAY);

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Lights()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	LIGHT_DESC LightDesc = {};

	/* Create Directional Light */
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-0.45f, -0.89f, 0.23f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 0.6f);

	if (FAILED(m_pGameInstance->Add_Light(CURRENT_LEVEL, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_ConstantBuffer()
{
	_float		fSaturation = { 1.f };
	_float		fExposure = { 1.f };
	_float		fContrast = { 1.f };
	_float3		vColorFilter = { 1.f, 1.f, 1.f };

	m_pGameInstance->Update_ColorGradeCB(vColorFilter, fSaturation, fExposure, fContrast);

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_ParticleManager()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Prepare_ParticleManager(
		static_cast<CTexture*>(m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"))),
		static_cast<CRenderer*>(m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"))))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_Particles(L"../../Reference/Resources/Data/Effect/Particle.dat")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_Camera(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CCamera_Main::CAMERAMAIN_DESC tCameraMainDesc = {  };

	tCameraMainDesc.tCameraInfo.fMouseSensor = 0.025f;

	tCameraMainDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
	tCameraMainDesc.tCameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
	tCameraMainDesc.tCameraDesc.fNear = 0.2f;
	tCameraMainDesc.tCameraDesc.fFar = 400.f;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Camera"), strLayerTag, &tCameraMainDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_StormTerror(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CSkyBox::SKYBOX_DESC tSkyBoxDesc = {};
	{
		tSkyBoxDesc.eLevel = CURRENT_LEVEL;
	}

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_SkyBox"), strLayerTag, &tSkyBoxDesc)))
		return E_FAIL;

	CDungeon::DUNGEON_DESC tDungeonDesc = {};
	{
		tDungeonDesc.eLevel = CURRENT_LEVEL;
	}

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Dungeon"), strLayerTag, &tDungeonDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_Player(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Player"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_Monster(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_EventCollider(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_NPC(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_Icon(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_UIManager") == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Manager"), TEXT("Layer_UIManager"))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_QuestManager") == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Quest_Manager"), TEXT("Layer_QuestManager"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_StormTerror::Ready_Layer_UI(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	COrtho::ORTHO_DESC tOrthoDesc = { };
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = L"";

	tOrthoDesc.fPosition = { 0.f, 0.f };
	tOrthoDesc.fSize = { static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Inventory"), L"Layer_Inventory", &tOrthoDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	/* Cursor */
	if (m_pGameInstance->Key_Down(VK_MENU))
	{
		m_pGameInstance->Clip_Mouse();
		m_pGameInstance->FreeFix_Mouse();
	}
	if (m_pGameInstance->Key_Up(VK_MENU))
	{
		m_pGameInstance->Fix_Mouse();
		m_pGameInstance->FreeClip_Mouse();
	}

	return S_OK;
}

HRESULT CLevel_StormTerror::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StormTerror::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_StormTerror* CLevel_StormTerror::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_StormTerror* pLevel = new CLevel_StormTerror(pDevice, pContext);

	if (FAILED(pLevel->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_StormTerror"));
		Safe_Release(pLevel);
	}

	return pLevel;
}

void CLevel_StormTerror::Free()
{
	__super::Free();

	m_pGameInstance->StopAll();
}
