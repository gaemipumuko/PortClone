#include "stdafx.h"
#include "Level_StrongHold_AbyssMage.h"

#include "Ortho.h"
#include "Effect.h"
#include "SkyBox.h"
#include "Dungeon.h"
#include "Updraft.h"
#include "SpawnBox.h"
#include "Ground_Item.h"
#include "Camera_Main.h"

#include "DungeonExit.h"
#include "DungeonChest.h"
#include "GearOperator_Manager.h"
#include "ElementalReactor_Manager.h"
#include "LineTrap.h"

#include "GameInstance.h"

#define CURRENT_LEVEL LEVEL_STRONGHOLD_ABYSSMAGE

CLevel_StrongHold_AbyssMage::CLevel_StrongHold_AbyssMage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Level()
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

	if (FAILED(Ready_Layer_StrongHold(TEXT("Layer_StrongHold"))))
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

	if (FAILED(Ready_Layer_Dungeon(TEXT("Layer_Dungeon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trap(TEXT("Layer_Trap"))))
		return E_FAIL;

	/* Init RenderMode */
	CDeciderObject::Set_RenderMode(CDeciderObject::RMODE_GAMEPLAY);

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Lights()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	LIGHT_DESC LightDesc = {};

	/* Create Directional Light */
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-0.11f, -0.89f, 0.05f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 0.6f);

	if (FAILED(m_pGameInstance->Add_Light(CURRENT_LEVEL, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_ConstantBuffer()
{
	_float		fSaturation = { 1.f };
	_float		fExposure = { 1.f };
	_float		fContrast = { 1.f };
	_float3		vColorFilter = { 1.f, 1.f, 1.f };

	m_pGameInstance->Update_ColorGradeCB(vColorFilter, fSaturation, fExposure, fContrast);

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_ParticleManager()
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

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_Camera(const wstring& strLayerTag)
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

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_StrongHold(const wstring& strLayerTag)
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

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_Player(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Player"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_Monster(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_EventCollider(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CUpdraft::UPDRAFT_DESC tDesc = {};
	tDesc.vPosition = _float4(-198.64f, -37.38f, -32.82f, 1.f);
	tDesc.vColliderSize = _float3(2.f, 12.f, 2.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;

	tDesc.vPosition = _float4(-288.94f, -75.88f, -30.83f, 1.f);
	tDesc.vColliderSize = _float3(3.f, 24.f, 2.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;

	tDesc.vPosition = _float4(-341.47f, -64.84f, -34.04f, 1.f);
	tDesc.vColliderSize = _float3(3.f, 18.5f, 2.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;


	tDesc.vPosition = _float4(-105.5f, 8.f, -70.f, 1.f);
	tDesc.vColliderSize = _float3(3.f, 28.f, 4.f);
	tDesc.vRadians = _float3(90.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;

	tDesc.vPosition = _float4(-105.5f, -8.f, -9.1f, 1.f);
	tDesc.vColliderSize = _float3(3.f, 28.f, 4.f);
	tDesc.vRadians = _float3(-90.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;


	tDesc.vPosition = _float4(-231.f, -46.f, -64.6f, 1.f);
	tDesc.vColliderSize = _float3(3.f, 28.f, 5.5f);
	tDesc.vRadians = _float3(90.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;

	tDesc.vPosition = _float4(-231.f, -31.f, -3.65f, 1.f);
	tDesc.vColliderSize = _float3(3.f, 28.f, 5.5f);
	tDesc.vRadians = _float3(-90.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_NPC(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_Icon(const wstring& strLayerTag)
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

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_UI(const wstring& strLayerTag)
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

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_Dungeon(const wstring& strLayerTag)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CElementalReactor_Manager::REACTORMGR_DESC tReactorMgrDesc = { };
	tReactorMgrDesc.eReactorLevel = CURRENT_LEVEL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_ElementalReactor_Manager"), strLayerTag, &tReactorMgrDesc)))
		return E_FAIL;

	CGearOperator_Manager::GEARMGR_DESC tGearMgrDesc = { };
	tGearMgrDesc.eGearLevel = CURRENT_LEVEL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_GearOperator_Manager"), strLayerTag, &tGearMgrDesc)))
		return E_FAIL;

	CDungeonExit::DUNGEON_EXIT tDungeonExit = { };
	tDungeonExit.eLevel = CURRENT_LEVEL;
	tDungeonExit.vExitPosition = _float4{ -476.2020f, -43.0840f, -16.1220f, 1.f };
	tDungeonExit.vReservePosition = _float4{ 717.4880f, 10.0000f, 190.4810f, 1.f };

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Dungeon_Exit"), strLayerTag, &tDungeonExit)))
		return E_FAIL;

	CDungeonChest::DUNGEON_CHEST tDungeonChest = { };
	tDungeonChest.eLevel = CURRENT_LEVEL;
	tDungeonChest.vRotation = _float4{ 0.f, 90.f, 0.f, 1.f };
	tDungeonChest.vPosition = _float4{ -458.018f, -43.4492f, -16.1609f, 1.f };
	
	tDungeonChest.DropList.push_back({ 124, 10000 });
	tDungeonChest.DropList.push_back({ 125, 100 });
	tDungeonChest.DropList.push_back({ 125, 100 });

	tDungeonChest.DropList.push_back({ 131, 4 });
	tDungeonChest.DropList.push_back({ 134, 1 });

	tDungeonChest.DropList.push_back({ 136, 3 });
	tDungeonChest.DropList.push_back({ 137, 2 });

	tDungeonChest.DropList.push_back({ 139, 4 });
	tDungeonChest.DropList.push_back({ 142, 1 });

	tDungeonChest.DropList.push_back({ 144, 3 });
	tDungeonChest.DropList.push_back({ 145, 2 });

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Dungeon_Chest"), strLayerTag, &tDungeonChest)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Ready_Layer_Trap(const wstring& strLayerTag)
{
	CLineTrap::LINE_TRAP_DESC tTrapDesc = {};
	tTrapDesc.vPointA = XMVectorSet(-160.f, -10.f, -58.f, 1.f);
	tTrapDesc.vPointB = XMVectorSet(-160.f, -10.f, -11.f, 1.f);
	tTrapDesc.fAccel = 2.f;
	tTrapDesc.fMoveRange = 27.f;
	tTrapDesc.fSpeed = 6.f;
	tTrapDesc.vStartDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Eff_LineTrap", L"Layer_Trap", &tTrapDesc)))
		return E_FAIL;

	tTrapDesc.vPointA = XMVectorSet(-160.f, -60.f, -58.f, 1.f);
	tTrapDesc.vPointB = XMVectorSet(-160.f, -60.f, -11.f, 1.f);
	tTrapDesc.fAccel = 2.f;
	tTrapDesc.fMoveRange = 27.f;
	tTrapDesc.fSpeed = 6.f;
	tTrapDesc.vStartDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Eff_LineTrap", L"Layer_Trap", &tTrapDesc)))
		return E_FAIL;




	tTrapDesc.vPointA = XMVectorSet(-300.f, -10.f, -58.f, 1.f);
	tTrapDesc.vPointB = XMVectorSet(-300.f, -10.f, -11.f, 1.f);
	tTrapDesc.fAccel = 1.f;
	tTrapDesc.fMoveRange = 35.f;
	tTrapDesc.fSpeed = 10.f;
	tTrapDesc.vStartDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Eff_LineTrap", L"Layer_Trap", &tTrapDesc)))
		return E_FAIL;

	tTrapDesc.vPointA = XMVectorSet(-300.f, -10.f, -58.f, 1.f);
	tTrapDesc.vPointB = XMVectorSet(-300.f, -10.f, -11.f, 1.f);
	tTrapDesc.fAccel = 2.f;
	tTrapDesc.fMoveRange = 35.f;
	tTrapDesc.fSpeed = 10.f;
	tTrapDesc.vStartDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Eff_LineTrap", L"Layer_Trap", &tTrapDesc)))
		return E_FAIL;

	tTrapDesc.vPointA = XMVectorSet(-300.f, -10.f, -58.f, 1.f);
	tTrapDesc.vPointB = XMVectorSet(-300.f, -10.f, -11.f, 1.f);
	tTrapDesc.fAccel = 4.f;
	tTrapDesc.fMoveRange = 35.f;
	tTrapDesc.fSpeed = 10.f;
	tTrapDesc.vStartDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Eff_LineTrap", L"Layer_Trap", &tTrapDesc)))
		return E_FAIL;

	tTrapDesc.vPointA = XMVectorSet(-300.f, -10.f, -58.f, 1.f);
	tTrapDesc.vPointB = XMVectorSet(-300.f, -10.f, -11.f, 1.f);
	tTrapDesc.fAccel = 8.f;
	tTrapDesc.fMoveRange = 35.f;
	tTrapDesc.fSpeed = 10.f;
	tTrapDesc.vStartDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, L"Prototype_GameObject_Eff_LineTrap", L"Layer_Trap", &tTrapDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::PreviousTick(_float fTimeDelta)
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

HRESULT CLevel_StrongHold_AbyssMage::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_StrongHold_AbyssMage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_StrongHold_AbyssMage* CLevel_StrongHold_AbyssMage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_StrongHold_AbyssMage* pLevel = new CLevel_StrongHold_AbyssMage(pDevice, pContext);

	if (FAILED(pLevel->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_StrongHold_AbyssMage"));
		Safe_Release(pLevel);
	}

	return pLevel;
}

void CLevel_StrongHold_AbyssMage::Free()
{
	__super::Free();

	m_pGameInstance->StopAll();
}
