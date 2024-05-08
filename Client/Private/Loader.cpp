#include "stdafx.h"
#include "Loader.h"

#include "BossChest.h"
#include "DungeonExit.h"
#include "DungeonChest.h"
#include "GearOperator_Manager.h"
#include "ElementalReactor_Manager.h"

#include "GameInstance.h"

_uint APIENTRY ThreadEntry(void* pArg)
{
	/* 새로운 Thread가 생성되었을 때 Device용 Com객체를 초기화 */
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Begin_Thread()))
		return FALSE;

	if (FAILED(pLoader->Loading_Resources()))
		return FALSE;

	if (FAILED(pLoader->End_Thread()))
		return FALSE;

	return TRUE;
}

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLoader::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);
	 
	return S_OK;
}

HRESULT CLoader::Loading_Resources()
{
	HRESULT hr = { };

	switch (m_eNextLevelID)
	{
	case LEVEL_WORLD:
		hr = Loading_World_Resources();
		break;
	case LEVEL_TEMPLE_LION:
		hr = Loading_Temple_Lion_Resources();
		break;
	case LEVEL_STRONGHOLD_ABYSSMAGE:
		hr = Loading_StrongHold_AbyssMage_Resources();
		break;
	case LEVEL_QUEST:
		hr = Loading_Quest_Resources();
		break;
	case LEVEL_STORMTERROR:
		hr = Loading_StormTerror_Resources();
		break;
	}

	if (FAILED(hr))
	{
		MSG_BOX(TEXT("Failed to Loading Resources : CLoader"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Loading_Entry_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	Set_LoadRatio(100.f);

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Loading_World_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;
	
	/* Component : Texture_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_WORLD, TEXT("Prototype_Component_Texture_Dungeon_Exit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit_%d.png"), 5), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_WORLD, TEXT("Prototype_Component_Model_Dungeon_Exit"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon_Exit"),
		CDungeonExit::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	Set_LoadRatio(100.f);

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Loading_Temple_Lion_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	/* Component : Texture_Elemental_Reactor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Texture_Elemental_Reactor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Elemental_Reactor/Elemental_Reactor_%d.png"), 5), TRUE)))
		return E_FAIL;

	/* Component : Model_Elemental_Reactor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Elemental_Reactor"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Elemental_Reactor/Elemental_Reactor.dat"), TRUE)))
		return E_FAIL;

	/* Component : Texture_Activate_Reactor_FX */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Texture_Activate_Reactor_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Activate_Reactor_FX/Activate_Reactor_FX_%d.png"), 2), TRUE)))
		return E_FAIL;

	/* Component : Model_Activate_Reactor_FX */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Activate_Reactor_FX"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Activate_Reactor_FX/Activate_Reactor_FX.dat"), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Door */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Dungeon_Door"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Door/Dungeon_Door.dat"), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Platform */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Dungeon_Platform"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Platform/Dungeon_Platform.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : ElementalReactor_Manager */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_ElementalReactor_Manager"),
		CElementalReactor_Manager::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	/* Component : Model_Gear_Operator */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Gear_Operator"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Gear_Operator/Gear_Operator/Gear_Operator.dat"), TRUE)))
		return E_FAIL;

	/* Component : Model_Gear_RuneCylinder */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Gear_RuneCylinder"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Gear_Operator/Gear_RuneCylinder/Gear_RuneCylinder.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : GearOperator_Manager */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_GearOperator_Manager"),
		CGearOperator_Manager::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	/* Component : Texture_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Texture_Dungeon_Exit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit_%d.png"), 5), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_TEMPLE_LION, TEXT("Prototype_Component_Model_Dungeon_Exit"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon_Exit"),
		CDungeonExit::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	/* GameObject : Dungeon_Chest */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon_Chest"),
		CDungeonChest::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	Set_LoadRatio(100.f);

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Loading_StrongHold_AbyssMage_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	/* Component : Texture_Elemental_Reactor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Texture_Elemental_Reactor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Elemental_Reactor/Elemental_Reactor_%d.png"), 5), TRUE)))
		return E_FAIL;

	/* Component : Model_Elemental_Reactor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Elemental_Reactor"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Elemental_Reactor/Elemental_Reactor.dat"), TRUE)))
		return E_FAIL;

	/* Component : Texture_Activate_Reactor_FX */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Texture_Activate_Reactor_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Activate_Reactor_FX/Activate_Reactor_FX_%d.png"), 2), TRUE)))
		return E_FAIL;

	/* Component : Model_Activate_Reactor_FX */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Activate_Reactor_FX"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Elemental_Reactor/Activate_Reactor_FX/Activate_Reactor_FX.dat"), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Door */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Dungeon_Door"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Door/Dungeon_Door.dat"), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Platform */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Dungeon_Platform"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Platform/Dungeon_Platform.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : ElementalReactor_Manager */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_ElementalReactor_Manager"),
		CElementalReactor_Manager::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	/* Component : Model_Gear_Operator */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Gear_Operator"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Gear_Operator/Gear_Operator/Gear_Operator.dat"), TRUE)))
		return E_FAIL;

	/* Component : Model_Gear_RuneCylinder */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Gear_RuneCylinder"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Gear_Operator/Gear_RuneCylinder/Gear_RuneCylinder.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : GearOperator_Manager */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_GearOperator_Manager"),
		CGearOperator_Manager::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	/* Component : Texture_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Texture_Dungeon_Exit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit_%d.png"), 5), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Dungeon_Exit"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon_Exit"),
		CDungeonExit::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Chest */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STRONGHOLD_ABYSSMAGE, TEXT("Prototype_Component_Model_Dungeon_Chest"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Chest/Dungeon_Chest.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : Dungeon_Chest */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon_Chest"),
		CDungeonChest::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	Set_LoadRatio(100.f);

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Loading_Quest_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	Set_LoadRatio(100.f);

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Loading_StormTerror_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	/* Component : Texture_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STORMTERROR, TEXT("Prototype_Component_Texture_Dungeon_Exit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit_%d.png"), 5), TRUE)))
		return E_FAIL;

	/* Component : Model_Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STORMTERROR, TEXT("Prototype_Component_Model_Dungeon_Exit"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Dungeon_Exit/Dungeon_Exit.dat"), TRUE)))
		return E_FAIL;

	/* GameObject : Dungeon_Exit */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon_Exit"),
		CDungeonExit::Create(m_pDevice, m_pContext), TRUE)))
		return E_FAIL;

	Set_LoadRatio(100.f);

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Initialize(LEVELID eLevel)
{
	if (eLevel >= LEVEL_LOADING)
		return E_FAIL;

	m_fLoadRatio = 0.f;
	m_eNextLevelID = eLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr);

	if (!m_hThread)
		return E_FAIL;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevel)
{
	CLoader* pLoader = new CLoader(pDevice, pContext);

	if (FAILED(pLoader->Initialize(eLevel)))
	{
		MSG_BOX(TEXT("Failed to Create : CLoader"));
		Safe_Release(pLoader);
	}

	return pLoader;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}