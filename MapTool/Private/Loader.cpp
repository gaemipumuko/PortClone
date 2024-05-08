#include "Loader.h"
#include "GameInstance.h"

#include "NaviMesh.h"
#include "Edit_Model.h"

#include "Camera_Free.h"
#include "Terrain.h"
#include "Edit_Object.h"
#include "Instancing.h"
#include "Edit_Spawner.h"
#include "Edit_Water.h"

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
	case LEVEL_EDIT:
		hr = Loading_Edit_Resources();
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

HRESULT CLoader::Loading_Edit_Resources()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

#pragma region ProtoType Component

	/* Component : NaviMesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_NaviMesh"),
		CNaviMesh::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Data/Navigation/Navigation.dat")))))
		return E_FAIL;

	/* Component : Texture Grass Diffuse */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Grass_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Diffuse/Terrain_Grass_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture Grass Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Grass_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Normal/Terrain_Grass_Normal.png")))))
		return E_FAIL;

	/* Component : Texture Road Diffuse */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Road_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Diffuse/Terrain_Road_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture Road Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Road_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Normal/Terrain_Road_Normal.png")))))
		return E_FAIL;

	/* Component : Texture Stone Diffuse */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Stone_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Diffuse/Terrain_Stone_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture Stone Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Stone_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Normal/Terrain_Stone_Normal.png")))))
		return E_FAIL;

	/* Component : Texture Brush */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Common/Brush.png")))))
		return E_FAIL;

	/* Component : Texture SpawnCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_SpawnCube"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Common/SpawnCube.dds")))))
		return E_FAIL;

	/* Component : Texture Water */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Water"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Water/Water_0.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region ProtoType GameObject

	/* GameObject : Camera */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Camera"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Terrain */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Edit_Object */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_EditObject"),
		CEdit_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Instancing */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Instancing"),
		CInstancing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Spawner */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Spawner"),
		CEdit_Spawner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Edit_Water */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_EditWater"),
		CEdit_Water::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	if (FAILED(Loading_Model_Resources(TEXT("../../Reference/Resources/Model/Map/*.*"))))
		return E_FAIL;

	Set_Finished(TRUE);

	return S_OK;
}

HRESULT CLoader::Loading_Model_Resources(wstring strDataPath)
{
	if (true == strDataPath.empty())
		return E_FAIL;

	HANDLE hFinder;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = true;

	_tchar szDir[MAX_PATH] = {};
	_tchar szEXT[MAX_PATH] = {};
	wstring strNewFilePath;
	wstring strFullPath;

	hFinder = FindFirstFile(strDataPath.c_str(), &tagFindData);

	if (INVALID_HANDLE_VALUE == hFinder)
		return E_FAIL;

	_wsplitpath_s(strDataPath.c_str(), NULL, NULL, szDir, MAX_PATH, NULL, NULL, NULL, NULL);

	while (bResult)
	{
		_wsplitpath_s(tagFindData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, szEXT, MAX_PATH);

		if (tagFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(tagFindData.cFileName, L".") && lstrcmp(tagFindData.cFileName, L".."))
			{
				strNewFilePath = L"";
				strNewFilePath += szDir;
				strNewFilePath += tagFindData.cFileName;
				strNewFilePath += L"/*.*";

				wstring strFolderTag = tagFindData.cFileName;

				Loading_Model_Resources(strNewFilePath);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".dat")))
		{
			strFullPath = L"";
			strFullPath += szDir;
			strFullPath += tagFindData.cFileName;

			string strPath = ConvertWCToC(strFullPath);

			wstring strProtoTag = TEXT("Prototype_Component_Model_");

			_tchar szName[MAX_PATH];
			_wsplitpath_s(strFullPath.c_str(), NULL, NULL, NULL, NULL, szName, MAX_PATH, NULL, NULL);

			strProtoTag.append(szName);

			if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDIT, strProtoTag,
				CEdit_Model::Create(m_pDevice, m_pContext, strPath))))
				return E_FAIL;
		}

		bResult = FindNextFile(hFinder, &tagFindData);
	}

	FindClose(hFinder);

	return S_OK;
}

HRESULT CLoader::Initialize(LEVELID eLevel)
{
	if (eLevel >= LEVEL_LOADING)
		return E_FAIL;

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