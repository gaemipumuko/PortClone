#include "../Default/framework.h"
#include "ToolWindow.h"

#include "EffectMainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"



CEffectMainApp::CEffectMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffectMainApp::Open_Level(LEVELID eLevel)
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

HRESULT CEffectMainApp::Ready_Prototype()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectMainApp::Ready_Prototype_GameObject()
{
	return S_OK;
}

HRESULT CEffectMainApp::Ready_Prototype_Component_For_Static()
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

	m_pRenderer->Set_RenderRTV(FALSE);
	m_pRenderer->Set_RenderDebugCom(FALSE);

	/* Component : Transform */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Component : VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Component : VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectMainApp::Ready_Prototype_For_Loading()
{
	return S_OK;
}

HRESULT CEffectMainApp::Initialize()
{
	GRAPHIC_DESC	eGraphicDesc;
	ZeroMemory(&eGraphicDesc, sizeof GRAPHIC_DESC);

	eGraphicDesc.iWinSizeX = g_iWinSizeX;
	eGraphicDesc.iWinSizeY = g_iWinSizeY;
	eGraphicDesc.isWindowed = GRAPHIC_DESC::MODE_WIN;
	eGraphicDesc.hWnd = g_hWnd;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, COLL_END, 0, g_hInstance, eGraphicDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX(TEXT("Failed to Initialize : Engine"));
		return E_FAIL;
	}

	/*m_pTool_Manager = CTool_Manager::Create(m_pDevice, m_pContext, g_hWnd);

	if (nullptr == m_pTool_Manager)
		return E_FAIL;*/

	if (FAILED(Ready_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Ready Prototype : CEffectMainApp"));
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_For_Loading()))
	{
		MSG_BOX(TEXT("Failed to Ready Prototype : CEffectMainApp"));
		return E_FAIL;
	}

	if (FAILED(Open_Level(LEVEL_EDITOR)))
	{
		MSG_BOX(TEXT("Failed to Open Level : CEffectMainApp"));
		return E_FAIL;
	}

	m_pTool = CToolWindow::Create(m_pDevice, m_pContext);

	return S_OK;
}

HRESULT CEffectMainApp::PreviousTick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
	++m_iFPS;

	if (m_fTimeAcc >= 1.f)
	{
		wstring strFPS = TEXT("FPS ") + to_wstring(m_iFPS);

		SetWindowText(g_hWnd, strFPS.c_str());

		m_iFPS = 0;
		m_fTimeAcc = 0.f;
	}
#endif

	return m_pGameInstance->PreviousTick_Engine(fTimeDelta);
}

HRESULT CEffectMainApp::Tick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	HRESULT hr = m_pGameInstance->Tick_Engine(fTimeDelta);

	//m_pTool_Manager->Tick(fTimeDelta);

	return hr;
}

HRESULT CEffectMainApp::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	return m_pGameInstance->LateTick_Engine(fTimeDelta);
}

HRESULT CEffectMainApp::Render()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	m_pTool->Render_Begin();

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	m_pTool->Render_End();

	m_pGameInstance->Present();

	return S_OK;
}

CEffectMainApp* CEffectMainApp::Create()
{
	CEffectMainApp* pInstance = new CEffectMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEffectMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pRenderer);

	Safe_Release(m_pTool);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
