#include "../Default/framework.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Free.h"

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
	case LEVEL_EDITOR:
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

	/* Component : Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_EDITOR, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Particle_%d.png"), 28))))
		return E_FAIL;

	/* GameObject : Camera */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Camera"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Set_Finished(TRUE);

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