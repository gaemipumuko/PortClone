#include "framework.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "MainApp.h"

#include "Level_Loading.h"

#include "GameInstance.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Open_Level(LEVELID eLevel)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (eLevel >= LEVEL_LOADING)
		return E_FAIL;

	CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eLevel);
	if (pLevel == nullptr)
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Loading"));
		return E_FAIL;
	}

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);
}

HRESULT CMainApp::Ready_Prototype()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(CMainApp::Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(CMainApp::Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	/* Component : Shader_Vtx */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Vtx"),
		CShader::Create(m_pDevice, m_pContext, VTXPOS::VertexElements, VTXPOS::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_Vtx.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxTex.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, VTXNORTEX::VertexElements, VTXNORTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxNorTex.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, VTXMESH::VertexElements, VTXMESH::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxMesh.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, VTXANIMMESH::VertexElements, VTXANIMMESH::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxAnimMesh.hlsl")))))
		return E_FAIL;

	/* Component : Renderer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Component : Transform */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Component : VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC	eGraphicDesc;
	ZeroMemory(&eGraphicDesc, sizeof GRAPHIC_DESC);

	eGraphicDesc.iWinSizeX = g_iWinSizeX;
	eGraphicDesc.iWinSizeY = g_iWinSizeY;
	eGraphicDesc.isWindowed = GRAPHIC_DESC::MODE_WIN;
	eGraphicDesc.hWnd = g_hWnd;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, 1, 1, g_hInstance, eGraphicDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX(TEXT("Failed to Initialize : Engine"));
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Ready Prototype : CMainApp"));
		return E_FAIL;
	}

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	if (FAILED(Open_Level(LEVEL_EDITOR)))
	{
		MSG_BOX(TEXT("Failed to Open Level : CMainApp"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::PreviousTick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	return m_pGameInstance->PreviousTick_Engine(fTimeDelta);
}

HRESULT CMainApp::Tick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	return m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	return m_pGameInstance->LateTick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();
	m_pGameInstance->Render_Level();

	m_pGameInstance->Present();

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp();

	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CMainApp"));
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CGameInstance::Release_Engine();
}