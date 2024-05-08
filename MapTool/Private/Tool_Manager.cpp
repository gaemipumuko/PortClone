#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Tool_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PipeLine.h"
#include "VIBuffer_Terrain.h"
#include "Ray.h"
#include "NaviMesh.h"
#include "Edit_Object.h"
#include "Terrain.h"
#include "RenderTarget.h"
#include "Instancing.h"
#include "Edit_Water.h"

static ExampleAppLog Debug;

CTool_Manager::CTool_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTool_Manager::Initialize(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuizmo::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.f, NULL, io.Fonts->GetGlyphRangesKorean());

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (float)g_iWinSizeX,	(float)g_iWinSizeY);

	if (FAILED(Create_Staging()))
		return E_FAIL;

	m_hWnd = hWnd;

	vector<wstring> strPathData;
	Loading_Path(TEXT("../../Reference/Resources/Model/Map/*.*"), &strPathData);

	m_ppItem_Model = new const _char*[strPathData.size()];

	for (size_t i = 0; i < strPathData.size(); i++)
	{
		wstring strProtoTag = TEXT("Prototype_Component_Model_");

		_tchar szName[MAX_PATH];
		_wsplitpath_s(strPathData[i].c_str(), NULL, NULL, NULL, NULL, szName, MAX_PATH, NULL, NULL);

		strProtoTag.append(szName);

		m_ProtoTypeTag.push_back(strProtoTag);

		string strName = ConvertWCToC(szName);
		const _char* pName = strName.c_str();

		m_ppItem_Model[i] = new _char[strlen(pName) + 1];
		strcpy_s(const_cast<char*>(m_ppItem_Model[i]), strlen(pName) + 1, pName);
	}

	m_CurrentObjectItem = m_ppItem_Model[0];

	return S_OK;
}

void CTool_Manager::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(VK_TAB))
		m_bActiveSnap = !m_bActiveSnap;

	if (m_pGameInstance->Key_Down(VK_ESCAPE))
		m_pSelectObject = nullptr;

	CTerrain* pTerrain = (CTerrain*)m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);

	if (nullptr != pTerrain) 
	{
		_float3 vPickPos = _float3(m_fPickingPoint[0], m_fPickingPoint[1], m_fPickingPoint[2]);
		_float2 vTerrainSize;

		if (FAILED(pTerrain->Get_TerrainSize(&vTerrainSize)))
			return;

		if ((vPickPos.x >= 0.0f && vPickPos.z >= 0.0f) &&
			(vTerrainSize.x >= vPickPos.x && vTerrainSize.y >= vPickPos.z)) 
		{
			_float4 vBrushPos = _float4(vPickPos.x, vPickPos.y, vPickPos.z, 1.0f);
			pTerrain->Set_BrushPos(vBrushPos);
		}
			
	}
}

HRESULT CTool_Manager::Render_Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	Draw_Common();
	Draw_WorkSpace();
	Draw_Gizmo();

	ImGui::Render();

	return S_OK;
}

HRESULT CTool_Manager::Render_End()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CTool_Manager::Draw_Common()
{
	ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f));

	ImGui::Begin("Common");

	ImGui::SeparatorText("Work Level");

	if (ImGui::Checkbox("World", &m_bWorldLevel)) 
	{
		m_bTempleLevel = false;
		m_bStrongHoldLevel = false;
		m_bQuestLevel = false;
		m_bDvalinLevel = false;
	}

	if (ImGui::Checkbox("Temple", &m_bTempleLevel)) 
	{
		m_bWorldLevel = false;
		m_bStrongHoldLevel = false;
		m_bQuestLevel = false;
		m_bDvalinLevel = false;
		m_bDvalinLevel = false;
	}
	
	ImGui::SameLine();

	if (ImGui::Checkbox("StrongHold", &m_bStrongHoldLevel))
	{
		m_bWorldLevel = false;
		m_bTempleLevel = false;
		m_bQuestLevel = false;
		m_bDvalinLevel = false;
	}

	if (ImGui::Checkbox("Quest", &m_bQuestLevel))
	{
		m_bWorldLevel = false;
		m_bTempleLevel = false;
		m_bStrongHoldLevel = false;
		m_bDvalinLevel = false;
	}

	if (ImGui::Checkbox("Dvalin", &m_bDvalinLevel))
	{
		m_bWorldLevel = false;
		m_bTempleLevel = false;
		m_bStrongHoldLevel = false;
		m_bQuestLevel = false;
	}

	ImGui::SeparatorText("Screen Point");

	_float2 vScreenPoint = Compute_ScreenPoint();
	_float3 vPickPos = {};

	if (FAILED(Compute_DepthPicking(vScreenPoint, &vPickPos))) 
	{
		ImGui::End();
		return;
	}
	
	m_fScreenPoint[0] = vScreenPoint.x;
	m_fScreenPoint[1] = vScreenPoint.y;

	m_fPickingPoint[0] = vPickPos.x;
	m_fPickingPoint[1] = vPickPos.y;
	m_fPickingPoint[2] = vPickPos.z;

	ImGui::InputFloat2("##Screen Point", m_fScreenPoint, "%.0f");

	ImGui::SeparatorText("Picking Point");

	ImGui::InputFloat3("##Picking Point", m_fPickingPoint, "%.3f");

	ImGui::End();
}

void CTool_Manager::Draw_WorkSpace()
{
	ImGui::SetNextWindowSize(ImVec2(450.0f, 800.0f));

	ImGui::Begin("WorkSpace");

	if (ImGui::BeginTabBar("Editor"))
	{
		if (ImGui::BeginTabItem("Inspector"))
		{
			Show_Inspector();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Terrain")) 
		{
			Show_TerrainEditor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Instancing"))
		{
			Show_InstancingEditor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Wave"))
		{
			Show_WaveEditor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Spawn")) 
		{
			Show_DungeonEditor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Navigation"))
		{
			Show_NavigationEditor();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CTool_Manager::Draw_Gizmo()
{
	if (ImGui::IsKeyDown(ImGuiKey_W))
	{
		m_iOperationMode = 7;
	}
	if (ImGui::IsKeyDown(ImGuiKey_E))
	{
		m_iOperationMode = 120;
	}
	if (ImGui::IsKeyDown(ImGuiKey_R))
	{
		m_iOperationMode = 896;
	}

	_float4x4 vGizmoWorldMatrix;
	_float4x4 vViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 vProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (nullptr != m_pSelectObject)
	{
		CTransform* pTransform = (CTransform*)m_pSelectObject->Find_Component(TEXT("Com_Transform"));

		if (nullptr == pTransform)
			return;

		XMStoreFloat4x4(&vGizmoWorldMatrix, pTransform->Get_WorldMatrix());

		if (false == m_bActiveSnap)
		{
			ImGuizmo::Manipulate(
				&vViewMatrix.m[0][0],
				&vProjMatrix.m[0][0],
				(ImGuizmo::OPERATION)m_iOperationMode,
				(ImGuizmo::MODE)m_iGizmoMode,
				&vGizmoWorldMatrix.m[0][0]
			);
		}
		else
		{
			ImGuizmo::Manipulate(
				&vViewMatrix.m[0][0],
				&vProjMatrix.m[0][0],
				(ImGuizmo::OPERATION)m_iOperationMode,
				(ImGuizmo::MODE)m_iGizmoMode,
				&vGizmoWorldMatrix.m[0][0],
				nullptr,
				m_fSnapValue);
		}

		if (ImGuizmo::IsUsing())
		{
			m_bIsUsingGizmo = true;

			switch ((ImGuizmo::OPERATION)m_iOperationMode)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&vGizmoWorldMatrix).r[CTransform::STATE_POSITION]);
				break;
			case ImGuizmo::OPERATION::ROTATE:
			{
				_vector Scale;
				_vector Rotate;
				_vector Translation;

				XMMatrixDecompose(&Scale, &Rotate, &Translation, XMLoadFloat4x4(&vGizmoWorldMatrix));

				_float4x4 vWorldMatrix;
				XMStoreFloat4x4(&vWorldMatrix, XMMatrixAffineTransformation(Scale, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), Rotate, Translation));
				
				pTransform->Set_WorldFloat4x4(vWorldMatrix);
			}
			break;
			case ImGuizmo::OPERATION::SCALE:
			{
				_float fScaleX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&vGizmoWorldMatrix).r[CTransform::STATE_RIGHT]));
				_float fScaleY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&vGizmoWorldMatrix).r[CTransform::STATE_UP]));
				_float fScaleZ = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&vGizmoWorldMatrix).r[CTransform::STATE_LOOK]));

				_float4x4 vWorld;
				XMStoreFloat4x4(&vWorld, pTransform->Get_WorldMatrix());

				_matrix SelectedMatrix = XMLoadFloat4x4(&vWorld);

				pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(SelectedMatrix.r[CTransform::STATE_RIGHT]) * fScaleX);
				pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(SelectedMatrix.r[CTransform::STATE_UP]) * fScaleY);
				pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(SelectedMatrix.r[CTransform::STATE_LOOK]) * fScaleZ);
			}
			break;
			default:
				break;
			}
		}
		else
			m_bIsUsingGizmo = false;
	}
}

void CTool_Manager::Draw_Grid()
{
	_float4x4 vViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 vProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_float4x4 vWorldMatrix;
	XMStoreFloat4x4(&vWorldMatrix, XMMatrixIdentity());

	//ID3D11DepthStencilState* pPrevDepthState = nullptr;
	//m_pContext->OMGetDepthStencilState(&pPrevDepthState, 0);

	//if (nullptr != pPrevDepthState)
	//{
	//	D3D11_DEPTH_STENCIL_DESC Test;
	//	pPrevDepthState->GetDesc(&Test);
	//	int a = 0;
	//}

	//m_pContext->OMSetDepthStencilState(m_pGridDepthState, 0);

	//ImGuizmo::DrawGrid(&vViewMatrix.m[0][0], &vProjMatrix.m[0][0], &vWorldMatrix.m[0][0], 100.0f);

	//m_pContext->OMSetDepthStencilState(pPrevDepthState, 0);

	//Safe_Release(pPrevDepthState);
}

void CTool_Manager::Show_Inspector()
{
	wstring szLayerTag;

	if ((false == m_bWorldLevel && false == m_bTempleLevel && false == m_bStrongHoldLevel && false == m_bQuestLevel && false == m_bDvalinLevel) ||
		(true == m_bWorldLevel && true == m_bTempleLevel && true == m_bStrongHoldLevel && true == m_bQuestLevel && true == m_bDvalinLevel))
		return;

	else if (true == m_bWorldLevel)
		szLayerTag = m_LayerTag_GameObjects;

	else if (true == m_bTempleLevel)
		szLayerTag = m_LayerTag_Temple;

	else if (true == m_bStrongHoldLevel)
		szLayerTag = m_LayerTag_StrongHold;

	else if (true == m_bQuestLevel)
		szLayerTag = m_LayerTag_Quest;

	else if (true == m_bDvalinLevel)
		szLayerTag = m_LayerTag_Dvalin;

	if (!ImGui::IsWindowHovered() &&
		!ImGui::IsAnyItemHovered() &&
		m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		if (nullptr == m_pSelectObject)
		{
			_float2 vScreenPoint = Compute_ScreenPoint();

			CRay* pRay = CRay::ScreenPointToRay(m_pDevice, m_pContext, vScreenPoint);
			CGameObject* pHitObject = nullptr;

			_float fDist = -1.0f;

			if (CRay::RayCast(LEVEL_EDIT, szLayerTag, pRay->Get_RayPos(), pRay->Get_RayDir(), &pHitObject, fDist))
			{
				if (nullptr != pHitObject) 
				{
					if (false == m_bDelete)
						m_pSelectObject = pHitObject;
					else
						m_pGameInstance->Release_Object(LEVEL_EDIT, szLayerTag, pHitObject);
				}
					
			}

			Safe_Release(pRay);
		}
	}

	if (nullptr != m_pSelectObject) 
	{
		CTransform* pTransform = (CTransform*)m_pSelectObject->Find_Component(TEXT("Com_Transform"));

		if (nullptr != pTransform) 
		{
			_float4x4 vWorldMatrix;
			XMStoreFloat4x4(&vWorldMatrix, pTransform->Get_WorldMatrix());
			
			float fTranslation[3], fRotation[3], fScale[3];
			ImGuizmo::DecomposeMatrixToComponents(&vWorldMatrix.m[0][0], fTranslation, fRotation, fScale);

			ImGui::Text("Translation");
			ImGui::InputFloat3("##Tr", fTranslation);
			ImGui::Text("Rotation");
			ImGui::InputFloat3("##Rt", fRotation);
			ImGui::Text("Scale");
			ImGui::InputFloat3("##Sc", fScale);

			ImGuizmo::RecomposeMatrixFromComponents(fTranslation, fRotation, fScale, &vWorldMatrix.m[0][0]);

			//_vector Scale;
			//_vector Rotate;
			//_vector Translation;

			//XMMatrixDecompose(&Scale, &Rotate, &Translation, XMLoadFloat4x4(&vWorldMatrix));
			//XMStoreFloat4x4(&vWorldMatrix, XMMatrixAffineTransformation(Scale, XMVectorZero(), Rotate, Translation));

			//pTransform->Set_WorldFloat4x4(vWorldMatrix);

			_float4 vRight = _float4(vWorldMatrix._11, vWorldMatrix._12, vWorldMatrix._13, vWorldMatrix._14);
			_float4 vUp = _float4(vWorldMatrix._21, vWorldMatrix._22, vWorldMatrix._23, vWorldMatrix._24);
			_float4 vLook = _float4(vWorldMatrix._31, vWorldMatrix._32, vWorldMatrix._33, vWorldMatrix._34);
			_float4  vPosition = _float4(vWorldMatrix._41, vWorldMatrix._42, vWorldMatrix._43, vWorldMatrix._44);

			pTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&vRight));
			pTransform->Set_State(CTransform::STATE_UP, XMLoadFloat4(&vUp));
			pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&vLook));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
		}
	}

	else 
	{
		_float4x4 vWorldMatrix;
		XMStoreFloat4x4(&vWorldMatrix, XMMatrixIdentity());

		float fTranslation[3], fRotation[3], fScale[3];
		ImGuizmo::DecomposeMatrixToComponents(&vWorldMatrix.m[0][0], fTranslation, fRotation, fScale);

		ImGui::Text("Translation");
		ImGui::InputFloat3("##Tr", fTranslation);
		ImGui::Text("Rotation");
		ImGui::InputFloat3("##Rt", fRotation);
		ImGui::Text("Scale");
		ImGui::InputFloat3("##Sc", fScale);
	}

	if (ImGui::Button("GameObjects", ImVec2(100, 25)))
		ImGui::OpenPopup("Create Object");

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(50, 25)))
		Load_GameObjects();

	ImGui::SameLine();

	if (ImGui::Button("Save", ImVec2(50, 25)))
		Save_GameObjects();

	ImGui::SameLine();

	ImGui::Checkbox("Delete", &m_bDelete);

	if (ImGui::BeginPopup("Create Object"))
	{
		if (nullptr == m_CurrentObjectItem) 
		{
			ImGui::EndPopup();
			return;
		}

		if (ImGui::BeginCombo("##Element Type", m_pCurrentItemType))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(m_pItemType); i++)
			{
				_bool bSelected = (m_pCurrentItemType == m_pItemType[i]);

				if (ImGui::Selectable(m_pItemType[i], bSelected))
				{
					m_pCurrentItemType = m_pItemType[i];
					m_iCurrentItemTypeIndex = i;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("##GameObjects ComboBox", m_CurrentObjectItem))
		{
			for (_int i = 0; i < m_ProtoTypeTag.size(); i++)
			{
				_bool bSelected = (m_CurrentObjectItem == m_ppItem_Model[i]);

				if (ImGui::Selectable(m_ppItem_Model[i], bSelected))
				{
					m_CurrentObjectItem = m_ppItem_Model[i];
					m_iCurrentObjectIndex = i;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (ImGui::Button("Create"))
			Create_GameObject();

		ImGui::EndPopup();
	}

	ImGui::SeparatorText("Move All Objects");

	if (ImGui::Button("+X", ImVec2(25, 25))) { Move_AllObjects(_float3(1.0f, 0.0f, 0.0f)); }
	ImGui::SameLine();
	if (ImGui::Button("+Y", ImVec2(25, 25))) { Move_AllObjects(_float3(0.0f, 1.0f, 0.0f)); }
	ImGui::SameLine();
	if (ImGui::Button("+Z", ImVec2(25, 25))) { Move_AllObjects(_float3(0.0f, 0.0f, 1.0f)); }

	if (ImGui::Button("-X", ImVec2(25, 25))) { Move_AllObjects(_float3(-1.0f, 0.0f, 0.0f)); }
	ImGui::SameLine();
	if (ImGui::Button("-Y", ImVec2(25, 25))) { Move_AllObjects(_float3(0.0f, -1.0f, 0.0f)); }
	ImGui::SameLine();
	if (ImGui::Button("-Z", ImVec2(25, 25))) { Move_AllObjects(_float3(0.0f, 0.0f, -1.0f)); }

	if (nullptr != m_pSelectObject) 
	{
		wstring szTag = ((CEdit_Object*)m_pSelectObject)->Get_Tag();

		string strTag = ConvertWCToC(szTag);

		ImGui::Text(strTag.c_str());
	}
}

void CTool_Manager::Show_TerrainEditor()
{
	ImGui::SeparatorText("Terrain Editor");

	CTerrain* pTerrain = (CTerrain*)m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);

	if (nullptr == pTerrain)
		return;

	ImGui::Text("Terrain Size");

	ImGui::InputInt2("##Scale", m_iTerrainScale);

	ImGui::SameLine();

	if (ImGui::Button("Resize")) 
	{
		pTerrain->Resize_Terrain(m_iTerrainScale[0], m_iTerrainScale[1]);
	}
	
	ImGui::SeparatorText("Brush");

	if (ImGui::Button("Load HeightMap", ImVec2(110, 25)))
		pTerrain->Apply_HeightMap(TEXT("../../Reference/Resources/Data/Map/WorldHeightMap.dat"));
	ImGui::SameLine();
	if (ImGui::Button("Load MaskMap", ImVec2(110, 25))) {}
	
	if (ImGui::Button("Save HeightMap", ImVec2(110, 25)))
		pTerrain->Save_HeightMap(TEXT("../../Reference/Resources/Data/Map/WorldHeightMap.dat"));
	ImGui::SameLine();
	if (ImGui::Button("Save MaskMap", ImVec2(110, 25)))
		pTerrain->Save_Mask(TEXT("../../Reference/Resources/Texture/Terrain/MaskMap/WorldMaskMap.dds"));

	ImGui::Checkbox("Brush Mode", &m_bBrushMode);

	if (true == m_bBrushMode)
	{
		if (ImGui::Checkbox("Paint", &m_bPainting)) 
		{
			m_bMasking = false;
		}

		if (ImGui::Checkbox("Mask", &m_bMasking)) 
		{
			m_bPainting = false;
		}

		if (true == m_bMasking)
		{
			ImGui::SameLine();

			if (ImGui::Checkbox("Grass Mask", &m_bGrassMask))
			{
				m_bRoadMask = false;
				m_bStoneMask = false;
			}
			
			ImGui::SameLine();

			if (ImGui::Checkbox("Road Mask", &m_bRoadMask))
			{
				m_bGrassMask = false;
				m_bStoneMask = false;
			}

			ImGui::SameLine();

			if (ImGui::Checkbox("Stone Mask", &m_bStoneMask))
			{
				m_bGrassMask = false;
				m_bRoadMask = false;
			}
		}

		if (!ImGui::IsWindowHovered() &&
			!ImGui::IsAnyItemHovered() &&
			m_pGameInstance->Key_Pressing(VK_LBUTTON)) 
		{
			_float3 vPickPos = _float3(m_fPickingPoint[0], m_fPickingPoint[1], m_fPickingPoint[2]);
			_float2 vTerrainSize;

			if (FAILED(pTerrain->Get_TerrainSize(&vTerrainSize)))
				return;

			if ((vPickPos.x >= 0.0f && vPickPos.z >= 0.0f) &&
				(vTerrainSize.x >= vPickPos.x && vTerrainSize.y >= vPickPos.z))
			{
				if (true == m_bPainting) 
				{
					pTerrain->Brushing_OnTerrain(m_fBrushForce, vPickPos, m_fBrushSize, m_fStrength, m_fMaxHeight);
				}
				
				else if (true == m_bMasking) 
				{
					CTerrain::MASK_TYPE eType = CTerrain::MASK_GRASS;

					if (true == m_bGrassMask)
						eType = CTerrain::MASK_GRASS;

					else if (true == m_bRoadMask)
						eType = CTerrain::MASK_ROAD;

					else if (true == m_bStoneMask)
						eType = CTerrain::MASK_STONE;

					pTerrain->Masking_OnTerrain(vPickPos, m_fBrushSize, eType);
				}
			}
		}

		pTerrain->Activate_Brush(true);

		ImGui::Text("Brush Force");
		ImGui::SameLine();
		ImGui::InputFloat("##Brush Force", &m_fBrushForce);

		ImGui::Text("Brush Size");
		ImGui::SameLine();
		ImGui::InputFloat("##Brush Size", &m_fBrushSize);

		pTerrain->Resize_Brush(m_fBrushSize);

		ImGui::Text("Brush Strength");
		ImGui::SameLine();
		ImGui::InputFloat("##Brush Strength", &m_fStrength);

		//pTerrain->Set_MaxHeight(m_fMaxHeight);
		ImGui::Text("Max Height");
		ImGui::SameLine();
		ImGui::InputFloat("##Max Height", &m_fMaxHeight);
	}
	else
		pTerrain->Activate_Brush(false);
}

void CTool_Manager::Show_InstancingEditor()
{
	ImGui::SeparatorText("Instancing Editor");

	CTerrain* pTerrain = (CTerrain*)m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);

	if (nullptr == pTerrain)
		return;

	CInstancing* pInstancing = (CInstancing*)m_pGameInstance->Find_Object(LEVEL_EDIT, TEXT("Layer_Instancing"));

	if (nullptr == pInstancing)
		return;

	if (ImGui::BeginCombo("##Instancing Type", m_pCurrentInstanceType))
	{
		for (_int i = 0; i < IM_ARRAYSIZE(m_pInstancingType); i++)
		{
			_bool bSelected = (m_pCurrentInstanceType == m_pInstancingType[i]);

			if (ImGui::Selectable(m_pInstancingType[i], bSelected))
			{
				m_pCurrentInstanceType = m_pInstancingType[i];
				m_iCurrentInstanceTypeIndex = i;
			}

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::SameLine();

	ImGui::Checkbox("Random Rotation Y", &m_bRandomRotationY);

	ImGui::Checkbox("Enable", &m_bInstancing);

	ImGui::SameLine();

	ImGui::Checkbox("Brush", &m_bInstancingBrush);

	ImGui::SameLine();

	if (ImGui::Button("Clear", ImVec2(110, 25))) 
	{
		if (0 == strcmp("Lush Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_LUSH);

		else if (0 == strcmp("Normal Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_NORMAL);

		else if (0 == strcmp("Silver Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_SILVER);

		else if (0 == strcmp("Pink Three Leaf Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_PINKTHREELEAF);

		else if (0 == strcmp("Blue One Leaf Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_BLUEONELEAF);

		else if (0 == strcmp("Pink Long Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_PINKLONG);

		else if (0 == strcmp("Blue Flower Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BLUE);

		else if (0 == strcmp("Toran Grass", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_TORAN);

		//else if (0 == strcmp("Lush Leaf Grass", m_pCurrentInstanceType))
		//	pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_LUSHLEAF);

		else if (0 == strcmp("Yellow Flower1", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y1);

		else if (0 == strcmp("Yellow Blue Red Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR);

		else if (0 == strcmp("Yellow Flower2", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y2);

		else if (0 == strcmp("Red Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_R);

		else if (0 == strcmp("Low Yellow Blue Red Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW);

		else if (0 == strcmp("Yellow Blue Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YB);

		else if (0 == strcmp("Blue Red Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BR);

		else if (0 == strcmp("Yellow Blue White Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBW);

		else if (0 == strcmp("Blue Flower1", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B1);

		else if (0 == strcmp("Puple Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE);

		else if (0 == strcmp("Red Napal Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R);

		else if (0 == strcmp("Orange Flower1", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O1);

		else if (0 == strcmp("Orange Flower2", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O2);

		else if (0 == strcmp("Pink Flower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PINK);

		else if (0 == strcmp("Blue Flower2", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B2);

		else if (0 == strcmp("SunFlower", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_SUN);

		else if (0 == strcmp("Yellow Flower3", m_pCurrentInstanceType))
			pInstancing->Clear_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y3);
	}

	if (ImGui::Button("Load", ImVec2(110, 25)))
	{
		if (true == m_bAll) 
		{
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_LUSH, TEXT("../../Reference/Resources/Data/Map/InstanceLushGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_NORMAL, TEXT("../../Reference/Resources/Data/Map/InstanceNormalGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_SILVER, TEXT("../../Reference/Resources/Data/Map/InstanceSilverGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_PINKTHREELEAF, TEXT("../../Reference/Resources/Data/Map/InstancePinkThreeLeafGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_BLUEONELEAF, TEXT("../../Reference/Resources/Data/Map/InstanceBlueOneLeafGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_PINKLONG, TEXT("../../Reference/Resources/Data/Map/InstancePinkLongGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BLUE, TEXT("../../Reference/Resources/Data/Map/InstanceBlueFlowerGrass.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_TORAN, TEXT("../../Reference/Resources/Data/Map/InstanceToranGrass.dat"));
			//pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_LUSHLEAF, TEXT("../../Reference/Resources/Data/Map/InstanceLushLeafGrass.dat"));
			
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY1.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY2.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerR.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR_LOW.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YB, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYB.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerBR.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBW.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB1.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPUPLE.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerNapal_R.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO1.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO2.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PINK, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPINK.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB2.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_SUN, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerSUN.dat"));
			pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y3, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY3.dat"));
		}

		else 
		{
			if (0 == strcmp("Lush Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_LUSH, TEXT("../../Reference/Resources/Data/Map/InstanceLushGrass.dat"));

			else if (0 == strcmp("Normal Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_NORMAL, TEXT("../../Reference/Resources/Data/Map/InstanceNormalGrass.dat"));

			else if (0 == strcmp("Silver Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_SILVER, TEXT("../../Reference/Resources/Data/Map/InstanceSilverGrass.dat"));

			else if (0 == strcmp("Pink Three Leaf Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_PINKTHREELEAF, TEXT("../../Reference/Resources/Data/Map/InstancePinkThreeLeafGrass.dat"));

			else if (0 == strcmp("Blue One Leaf Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_BLUEONELEAF, TEXT("../../Reference/Resources/Data/Map/InstanceBlueOneLeafGrass.dat"));

			else if (0 == strcmp("Pink Long Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_PINKLONG, TEXT("../../Reference/Resources/Data/Map/InstancePinkLongGrass.dat"));

			else if (0 == strcmp("Blue Flower Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BLUE, TEXT("../../Reference/Resources/Data/Map/InstanceBlueFlowerGrass.dat"));

			else if (0 == strcmp("Toran Grass", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_TORAN, TEXT("../../Reference/Resources/Data/Map/InstanceToranGrass.dat"));

			//else if (0 == strcmp("Lush Leaf Grass", m_pCurrentInstanceType))
			//	pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_LUSHLEAF, TEXT("../../Reference/Resources/Data/Map/InstanceLushLeafGrass.dat"));

			else if (0 == strcmp("Yellow Flower1", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY1.dat"));

			else if (0 == strcmp("Yellow Blue Red Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR.dat"));

			else if (0 == strcmp("Yellow Flower2", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY2.dat"));

			else if (0 == strcmp("Red Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerR.dat"));

			else if (0 == strcmp("Low Yellow Blue Red Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR_LOW.dat"));

			else if (0 == strcmp("Yellow Blue Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YB, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYB.dat"));

			else if (0 == strcmp("Blue Red Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerBR.dat"));

			else if (0 == strcmp("Yellow Blue White Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBW.dat"));

			else if (0 == strcmp("Blue Flower1", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB1.dat"));

			else if (0 == strcmp("Puple Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPUPLE.dat"));

			else if (0 == strcmp("Red Napal Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerNapal.dat"));

			else if (0 == strcmp("Orange Flower1", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO1.dat"));

			else if (0 == strcmp("Orange Flower2", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO2.dat"));

			else if (0 == strcmp("Pink Flower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PINK, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPINK.dat"));

			else if (0 == strcmp("Blue Flower2", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB2.dat"));

			else if (0 == strcmp("SunFlower", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_SUN, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerSUN.dat"));

			else if (0 == strcmp("Yellow Flower3", m_pCurrentInstanceType))
				pInstancing->Load_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y3, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY3.dat"));
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Save", ImVec2(110, 25)))
	{
		if (true == m_bAll) 
		{
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_LUSH, TEXT("../../Reference/Resources/Data/Map/InstanceLushGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_NORMAL, TEXT("../../Reference/Resources/Data/Map/InstanceNormalGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_SILVER, TEXT("../../Reference/Resources/Data/Map/InstanceSilverGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_PINKTHREELEAF, TEXT("../../Reference/Resources/Data/Map/InstancePinkThreeLeafGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_BLUEONELEAF, TEXT("../../Reference/Resources/Data/Map/InstanceBlueOneLeafGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_PINKLONG, TEXT("../../Reference/Resources/Data/Map/InstancePinkLongGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BLUE, TEXT("../../Reference/Resources/Data/Map/InstanceBlueFlowerGrass.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_TORAN, TEXT("../../Reference/Resources/Data/Map/InstanceToranGrass.dat"));
			//pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_LUSHLEAF, TEXT("../../Reference/Resources/Data/Map/InstanceLushLeafGrass.dat"));

			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY1.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY2.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerR.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR_LOW.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YB, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYB.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerBR.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBW.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB1.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPUPLE.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerNapal_R.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO1.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO2.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PINK, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPINK.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB2.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_SUN, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerSUN.dat"));
			pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y3, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY3.dat"));
		}

		else 
		{
			if (0 == strcmp("Lush Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_LUSH, TEXT("../../Reference/Resources/Data/Map/InstanceLushGrass.dat"));

			else if (0 == strcmp("Normal Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_NORMAL, TEXT("../../Reference/Resources/Data/Map/InstanceNormalGrass.dat"));

			else if (0 == strcmp("Silver Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_SILVER, TEXT("../../Reference/Resources/Data/Map/InstanceSilverGrass.dat"));

			else if (0 == strcmp("Pink Three Leaf Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_PINKTHREELEAF, TEXT("../../Reference/Resources/Data/Map/InstancePinkThreeLeafGrass.dat"));

			else if (0 == strcmp("Blue One Leaf Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_BLUEONELEAF, TEXT("../../Reference/Resources/Data/Map/InstanceBlueOneLeafGrass.dat"));

			else if (0 == strcmp("Pink Long Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_PINKLONG, TEXT("../../Reference/Resources/Data/Map/InstancePinkLongGrass.dat"));

			else if (0 == strcmp("Blue Flower Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BLUE, TEXT("../../Reference/Resources/Data/Map/InstanceBlueFlowerGrass.dat"));

			else if (0 == strcmp("Toran Grass", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_TORAN, TEXT("../../Reference/Resources/Data/Map/InstanceToranGrass.dat"));

			//else if (0 == strcmp("Lush Leaf Grass", m_pCurrentInstanceType))
			//	pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_LUSHLEAF, TEXT("../../Reference/Resources/Data/Map/InstanceLushLeafGrass.dat"));

			else if (0 == strcmp("Yellow Flower1", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY1.dat"));

			else if (0 == strcmp("Yellow Blue Red Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR.dat"));

			else if (0 == strcmp("Yellow Flower2", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY2.dat"));

			else if (0 == strcmp("Red Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerR.dat"));

			else if (0 == strcmp("Low Yellow Blue Red Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBR_LOW.dat"));

			else if (0 == strcmp("Yellow Blue Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YB, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYB.dat"));

			else if (0 == strcmp("Blue Red Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_BR, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerBR.dat"));

			else if (0 == strcmp("Yellow Blue White Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_YBW, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerYBW.dat"));

			else if (0 == strcmp("Blue Flower1", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB1.dat"));

			else if (0 == strcmp("Puple Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPUPLE.dat"));

			else if (0 == strcmp("Red Napal Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerNapal.dat"));

			else if (0 == strcmp("Orange Flower1", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O1, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO1.dat"));

			else if (0 == strcmp("Orange Flower2", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_O2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerO2.dat"));

			else if (0 == strcmp("Pink Flower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_PINK, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerPINK.dat"));

			else if (0 == strcmp("Blue Flower2", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_B2, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerB2.dat"));

			else if (0 == strcmp("SunFlower", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_SUN, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerSUN.dat"));

			else if (0 == strcmp("Yellow Flower3", m_pCurrentInstanceType))
				pInstancing->Save_InstanceData(CInstancing::INSTANCE_GRASS_FLOWER_Y3, TEXT("../../Reference/Resources/Data/Map/InstanceFlowerY3.dat"));
		}
	}

	ImGui::SameLine();

	ImGui::Checkbox("All", &m_bAll);

	if (true == m_bInstancing) 
	{
		ImGui::Text("Brush Size");
		ImGui::SameLine();
		ImGui::InputFloat("##Brush Size", &m_fBrushSize);

		pTerrain->Resize_Brush(m_fBrushSize);

		if (true == m_bInstancingBrush)
			pTerrain->Activate_Brush(true);
		else
			pTerrain->Activate_Brush(false);

		if (!ImGui::IsWindowHovered() &&
			!ImGui::IsAnyItemHovered() &&
			m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			if (true == m_bInstancingBrush) 
			{
				for (float fX = m_fPickingPoint[0] - m_fBrushSize; fX <= m_fPickingPoint[0] + m_fBrushSize; fX += m_fInstanceTerm)
				{
					for (float fZ = m_fPickingPoint[2] - m_fBrushSize; fZ <= m_fPickingPoint[2] + m_fBrushSize; fZ += m_fInstanceTerm)
					{
						/*_float3 vBrushPos = _float3(m_fPickingPoint[0], m_fPickingPoint[1], m_fPickingPoint[2]);
						_float3 vGrassPos = _float3(fX, m_fPickingPoint[1], fZ);

						_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vGrassPos) - XMLoadFloat3(&vBrushPos)));

						if (fDistance < m_fBrushSize) 
						{
						}*/

						_float4x4 vInstanceWorld;
						XMStoreFloat4x4(&vInstanceWorld, XMMatrixIdentity());

						if (true == m_bRandomRotationY)
							XMStoreFloat4x4(&vInstanceWorld, XMMatrixRotationY(XMConvertToRadians(rand() % 360)));

						vInstanceWorld.m[3][0] = fX;
						vInstanceWorld.m[3][1] = m_fPickingPoint[1];
						vInstanceWorld.m[3][2] = fZ;

						if (0 == strcmp("Lush Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_LUSH, vInstanceWorld);
						else if (0 == strcmp("Normal Grass", m_pCurrentInstanceType)) 
						{
							_float fScale = (2.0f + rand() % 3) / 10.0f;

							XMStoreFloat4x4(&vInstanceWorld, XMMatrixIdentity());

							if (true == m_bRandomRotationY)
								XMStoreFloat4x4(&vInstanceWorld, XMMatrixScaling(fScale, fScale, fScale) * XMMatrixRotationY(XMConvertToRadians(rand() % 360)));

							vInstanceWorld.m[3][0] = fX;
							vInstanceWorld.m[3][1] = m_fPickingPoint[1];
							vInstanceWorld.m[3][2] = fZ;

							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_NORMAL, vInstanceWorld);
						}
							
						else if (0 == strcmp("Silver Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_SILVER, vInstanceWorld);
						else if (0 == strcmp("Pink Three Leaf Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_PINKTHREELEAF, vInstanceWorld);
						else if (0 == strcmp("Blue One Leaf Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_BLUEONELEAF, vInstanceWorld);
						else if (0 == strcmp("Pink Long Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_PINKLONG, vInstanceWorld);
						else if (0 == strcmp("Blue Flower Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_BLUE, vInstanceWorld);
						else if (0 == strcmp("Toran Grass", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_TORAN, vInstanceWorld);
						//else if (0 == strcmp("Lush Leaf Grass", m_pCurrentInstanceType))
						//	pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_LUSHLEAF, vInstanceWorld);

						else if (0 == strcmp("Yellow Flower1", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_Y1, vInstanceWorld);
						else if (0 == strcmp("Yellow Blue Red Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YBR, vInstanceWorld);
						else if (0 == strcmp("Yellow Flower2", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_Y2, vInstanceWorld);
						else if (0 == strcmp("Red Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_R, vInstanceWorld);
						else if (0 == strcmp("Low Yellow Blue Red Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW, vInstanceWorld);
						else if (0 == strcmp("Yellow Blue Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YB, vInstanceWorld);
						else if (0 == strcmp("Blue Red Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_BR, vInstanceWorld);
						else if (0 == strcmp("Yellow Blue White Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YBW, vInstanceWorld);
						else if (0 == strcmp("Blue Flower1", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_B1, vInstanceWorld);
						else if (0 == strcmp("Puple Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE, vInstanceWorld);
						else if (0 == strcmp("Red Napal Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R, vInstanceWorld);
						else if (0 == strcmp("Orange Flower1", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_O1, vInstanceWorld);
						else if (0 == strcmp("Orange Flower2", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_O2, vInstanceWorld);
						else if (0 == strcmp("Pink Flower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_PINK, vInstanceWorld);
						else if (0 == strcmp("Blue Flower2", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_B2, vInstanceWorld);
						else if (0 == strcmp("SunFlower", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_SUN, vInstanceWorld);
						else if (0 == strcmp("Yellow Flower3", m_pCurrentInstanceType))
							pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_Y3, vInstanceWorld);

					}
				}
			}

			else 
			{
				_float4x4 vInstanceWorld;
				XMStoreFloat4x4(&vInstanceWorld, XMMatrixIdentity());

				if (true == m_bRandomRotationY)
					XMStoreFloat4x4(&vInstanceWorld, XMMatrixRotationY(XMConvertToRadians(rand() % 360)));

				vInstanceWorld.m[3][0] = m_fPickingPoint[0];
				vInstanceWorld.m[3][1] = m_fPickingPoint[1];
				vInstanceWorld.m[3][2] = m_fPickingPoint[2];

				if (0 == strcmp("Lush Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_LUSH, vInstanceWorld);
				else if (0 == strcmp("Normal Grass", m_pCurrentInstanceType)) 
				{
					_float fScale = (4.0f + rand() % 3) / 10.0f;

					XMStoreFloat4x4(&vInstanceWorld, XMMatrixIdentity());

					if (true == m_bRandomRotationY)
						XMStoreFloat4x4(&vInstanceWorld, XMMatrixScaling(fScale, fScale, fScale) * XMMatrixRotationY(XMConvertToRadians(rand() % 360)));

					vInstanceWorld.m[3][0] = m_fPickingPoint[0];
					vInstanceWorld.m[3][1] = m_fPickingPoint[1];
					vInstanceWorld.m[3][2] = m_fPickingPoint[2];

					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_NORMAL, vInstanceWorld);
				}
				else if (0 == strcmp("Silver Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_SILVER, vInstanceWorld);
				else if (0 == strcmp("Pink Three Leaf Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_PINKTHREELEAF, vInstanceWorld);
				else if (0 == strcmp("Blue One Leaf Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_BLUEONELEAF, vInstanceWorld);
				else if (0 == strcmp("Pink Long Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_PINKLONG, vInstanceWorld);
				else if (0 == strcmp("Blue Flower Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_BLUE, vInstanceWorld);
				else if (0 == strcmp("Toran Grass", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_TORAN, vInstanceWorld);
				//else if (0 == strcmp("Lush Leaf Grass", m_pCurrentInstanceType))
				//	pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_LUSHLEAF, vInstanceWorld);

				else if (0 == strcmp("Yellow Flower1", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_Y1, vInstanceWorld);
				else if (0 == strcmp("Yellow Blue Red Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YBR, vInstanceWorld);
				else if (0 == strcmp("Yellow Flower2", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_Y2, vInstanceWorld);
				else if (0 == strcmp("Red Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_R, vInstanceWorld);
				else if (0 == strcmp("Low Yellow Blue Red Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YBR_LOW, vInstanceWorld);
				else if (0 == strcmp("Yellow Blue Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YB, vInstanceWorld);
				else if (0 == strcmp("Blue Red Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_BR, vInstanceWorld);
				else if (0 == strcmp("Yellow Blue White Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_YBW, vInstanceWorld);
				else if (0 == strcmp("Blue Flower1", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_B1, vInstanceWorld);
				else if (0 == strcmp("Puple Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_PUPLE, vInstanceWorld);
				else if (0 == strcmp("Red Napal Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_NAPAL_R, vInstanceWorld);
				else if (0 == strcmp("Orange Flower1", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_O1, vInstanceWorld);
				else if (0 == strcmp("Orange Flower2", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_O2, vInstanceWorld);
				else if (0 == strcmp("Pink Flower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_PINK, vInstanceWorld);
				else if (0 == strcmp("Blue Flower2", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_B2, vInstanceWorld);
				else if (0 == strcmp("SunFlower", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_SUN, vInstanceWorld);
				else if (0 == strcmp("Yellow Flower3", m_pCurrentInstanceType))
					pInstancing->Add_Instance(CInstancing::INSTANCE_GRASS_FLOWER_Y3, vInstanceWorld);
			}
		}

		ImGui::Text("Instance Term");
		ImGui::InputFloat("##Instance Term", &m_fInstanceTerm);
	}
}

void CTool_Manager::Show_WaveEditor()
{
	_int iLevel = -1;
	wstring szLayerTag;
	wstring szDataPath;

	if ((false == m_bWorldLevel && false == m_bTempleLevel && false == m_bStrongHoldLevel && false == m_bQuestLevel && false == m_bDvalinLevel) ||
		(true == m_bWorldLevel && true == m_bTempleLevel && true == m_bStrongHoldLevel && true == m_bQuestLevel && true == m_bDvalinLevel))
		return;

	else if (true == m_bWorldLevel) 
	{
		iLevel = 2;
		szLayerTag = m_LayerTag_WaterInWorld;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/WorldWater.dat");
	}
		

	else if (true == m_bTempleLevel)
	{
		iLevel = 3;
		szLayerTag = m_LayerTag_WaterInTemple;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/TempleWater.dat");
	}

	else if (true == m_bStrongHoldLevel)
	{
		iLevel = 4;
		szLayerTag = m_LayerTag_WaterInStrongHold;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/StrongHoldWater.dat");
	}

	else if (true == m_bQuestLevel)
	{
		iLevel = 5;
		szLayerTag = m_LayerTag_WaterInQuest;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/QuestWater.dat");
	}

	else if (true == m_bDvalinLevel)
	{
		iLevel = 6;
		szLayerTag = m_LayerTag_WaterInDvalin;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/DvalinWater.dat");
	}
		

	if (!ImGui::IsWindowHovered() &&
		!ImGui::IsAnyItemHovered() &&
		m_pGameInstance->Key_Pressing(VK_LBUTTON)) 
	{
		if (nullptr == m_pSelectObject)
		{
			_float2 vScreenPoint = Compute_ScreenPoint();

			CRay* pRay = CRay::ScreenPointToRay(m_pDevice, m_pContext, vScreenPoint);
			CGameObject* pHitObject = nullptr;

			_float fDist = -1.0f;

			if (CRay::RayCast(LEVEL_EDIT, szLayerTag, pRay->Get_RayPos(), pRay->Get_RayDir(), &pHitObject, fDist))
			{
				if (nullptr != pHitObject)
					m_pSelectObject = pHitObject;
			}

			Safe_Release(pRay);
		}
	}

	ImGui::SeparatorText("Water Desc");

	ImGui::Text("Water Size");
	ImGui::InputInt2("##Water Size", m_iWaterSize);

	ImGui::Text("Grid Spacing");
	ImGui::InputFloat("##Grid Spacing", &m_fGridSpacing);

	ImGui::Text("Water Spawn Position");
	ImGui::InputFloat3("##Water Spawn Position", m_fWaterSpawnPos);

	ImGui::Text("Water UV Scale");
	ImGui::InputFloat3("##Water UV Scale", m_fWaterUVScale);

	ImGui::Text("Water Scale");
	ImGui::InputFloat("##Water Scale", &m_fWaterScale);

	ImGui::Text("Water Noise Value");
	ImGui::InputFloat3("##Water Noise Value", m_fNoise);

	ImGui::Text("Water Speed");
	ImGui::InputFloat2("##Water Speed", m_fWaterSpeed);

	if (ImGui::Button("Create", ImVec2(110, 25))) 
	{
		CEdit_Water::WATER_DESC tWaterDesc = {};
		{
			tWaterDesc.iSizeX = m_iWaterSize[0];
			tWaterDesc.iSizeZ = m_iWaterSize[1];
			tWaterDesc.fGridSpacing = m_fGridSpacing;

			_float4x4 vWorldMatrix;
			XMStoreFloat4x4(&vWorldMatrix, XMMatrixIdentity());
			{
				vWorldMatrix.m[3][0] = m_fWaterSpawnPos[0];
				vWorldMatrix.m[3][1] = m_fWaterSpawnPos[1];
				vWorldMatrix.m[3][2] = m_fWaterSpawnPos[2];
				vWorldMatrix.m[3][3] = 1.0f;
			}

			tWaterDesc.vWorldMatrix = vWorldMatrix;

			tWaterDesc.vUVScale = _float3(m_fWaterUVScale[0], m_fWaterUVScale[1], m_fWaterUVScale[2]);
			tWaterDesc.fWaveScale = m_fWaterScale;
			tWaterDesc.vNoise = _float3(m_fNoise[0], m_fNoise[1], m_fNoise[2]);
			tWaterDesc.vWaveSpeed = _float2(m_fWaterSpeed[0], m_fWaterSpeed[1]);
		}

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_EditWater"), szLayerTag, &tWaterDesc)))
			return;
	}

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(110, 25))) 
	{
		ifstream InFile(szDataPath, ios::binary);

		if (InFile.is_open())
		{
			_uint iNumWater;
			InFile.read(reinterpret_cast<_char*>(&iNumWater), sizeof(_uint));

			_uint iSizeX;
			_uint iSizeZ;
			_float4x4 vWorldMatrix;
			_float fGridSpacing;
			_float fWaveScale;
			_float3 vUVScale;
			_float3 vNoise;
			_float2 vWaveSpeed;

			for (_int i = 0; i < iNumWater; i++)
			{
				InFile.read(reinterpret_cast<_char*>(&iSizeX), sizeof(_uint));
				InFile.read(reinterpret_cast<_char*>(&iSizeZ), sizeof(_uint));
				InFile.read(reinterpret_cast<_char*>(&vWorldMatrix), sizeof(_float4x4));
				InFile.read(reinterpret_cast<_char*>(&fGridSpacing), sizeof(_float));
				InFile.read(reinterpret_cast<_char*>(&fWaveScale), sizeof(_float));
				InFile.read(reinterpret_cast<_char*>(&vUVScale), sizeof(_float3));
				InFile.read(reinterpret_cast<_char*>(&vNoise), sizeof(_float3));
				InFile.read(reinterpret_cast<_char*>(&vWaveSpeed), sizeof(_float2));

				CEdit_Water::WATER_DESC tWaterDesc = {};
				{
					tWaterDesc.iSizeX = iSizeX;
					tWaterDesc.iSizeZ = iSizeZ;
					tWaterDesc.vWorldMatrix = vWorldMatrix;
					tWaterDesc.fGridSpacing = fGridSpacing;
					tWaterDesc.fWaveScale = fWaveScale;
					tWaterDesc.vUVScale = vUVScale;
					tWaterDesc.vNoise = vNoise;
					tWaterDesc.vWaveSpeed = vWaveSpeed;
				}

				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_EditWater"), szLayerTag, &tWaterDesc)))
					return;
			}
		}

		InFile.close();
	}

	ImGui::SameLine();

	if (ImGui::Button("Save", ImVec2(110, 25))) 
	{
		list<CGameObject*>* pGameObjects = m_pGameInstance->Find_Objects(LEVEL_EDIT, szLayerTag);

		if (nullptr != pGameObjects)
		{
			ofstream OutFile(szDataPath, ios::binary);

			_uint iNumWater = pGameObjects->size();

			OutFile.write(reinterpret_cast<_char*>(&iNumWater), sizeof(_uint));

			for (auto& pGameObject : *pGameObjects)
			{
				CEdit_Water* pWater = (CEdit_Water*)pGameObject;

				_uint iSizeX = pWater->Get_SizeX();
				_uint iSizeZ = pWater->Get_SizeZ();
				_float4x4 vWorldMatrix = pWater->Get_WorldMatrix();
				_float fGridSpacing = pWater->Get_GridSpacing();
				_float fWaveScale = *pWater->Get_WaveScale();
				_float3 vUVScale = *pWater->Get_UVScale();
				_float3 vNoise = *pWater->Get_Noise();
				_float2 vWaveSpeed = *pWater->Get_WaveSpeed();

				OutFile.write(reinterpret_cast<_char*>(&iSizeX), sizeof(_uint));
				OutFile.write(reinterpret_cast<_char*>(&iSizeZ), sizeof(_uint));
				OutFile.write(reinterpret_cast<_char*>(&vWorldMatrix), sizeof(_float4x4));
				OutFile.write(reinterpret_cast<_char*>(&fGridSpacing), sizeof(_float));
				OutFile.write(reinterpret_cast<_char*>(&fWaveScale), sizeof(_float));
				OutFile.write(reinterpret_cast<_char*>(&vUVScale), sizeof(_float3));
				OutFile.write(reinterpret_cast<_char*>(&vNoise), sizeof(_float3));
				OutFile.write(reinterpret_cast<_char*>(&vWaveSpeed), sizeof(_float2));
			}

			OutFile.close();
		}
	}

	ImGui::SeparatorText("Water List");

	list<CGameObject*>* pWaters = m_pGameInstance->Find_Objects(LEVEL_EDIT, szLayerTag);

	if (nullptr != pWaters)
	{
		_int iIndex = 0;

		for (auto iter = pWaters->begin(); iter != pWaters->end(); iter++)
		{
			CTransform* pTransform = (CTransform*)((*iter)->Find_Component(TEXT("Com_Transform")));

			_float4 vWaterPos;
			XMStoreFloat4(&vWaterPos, pTransform->Get_State(CTransform::STATE_POSITION));

			string szName = "Water_";
			{
				szName.append(to_string(vWaterPos.x));
				szName.append("_");
				szName.append(to_string(vWaterPos.y));
				szName.append("_");
				szName.append(to_string(vWaterPos.z));
			}

			if (ImGui::Selectable(szName.c_str()))
			{
				m_pSelectObject = *iter;
				m_iSpawnerIndex = iIndex;
			}

			iIndex++;
		}
	}

	if (nullptr != m_pSelectObject)
	{
		if (ImGui::Button("Delete", ImVec2(110, 25)))
		{
			m_pSelectObject->Set_Destroy(true);
			m_pSelectObject = nullptr;
			m_iSpawnerIndex = 0;
		}
	}

	ImGui::SeparatorText("Water Info");
	if (nullptr != m_pSelectObject) 
	{
		CEdit_Water* pWater = (CEdit_Water*)m_pSelectObject;

		_float* pWaveScale = pWater->Get_WaveScale();
		
		_float fUVScale[3] =
		{
			pWater->Get_UVScale()->x,
			pWater->Get_UVScale()->y,
			pWater->Get_UVScale()->z
		};

		_float fNoise[3] = 
		{
			pWater->Get_Noise()->x,
			pWater->Get_Noise()->y,
			pWater->Get_Noise()->z
		};

		_float fWaveSpeed[2] = 
		{
			pWater->Get_WaveSpeed()->x,
			pWater->Get_WaveSpeed()->y
		};

		ImGui::Text("Wave Scale");
		ImGui::InputFloat("##Wave Scale", pWaveScale);

		ImGui::Text("Wave UV Scale");
		ImGui::InputFloat3("##Wave UV Scale", fUVScale);

		ImGui::Text("Wave Noise");
		ImGui::InputFloat3("##Wave Noise", fNoise);

		ImGui::Text("Wave Speed");
		ImGui::InputFloat2("##Wave Speed", fWaveSpeed);

		pWater->Set_WaveScale(pWaveScale);
		pWater->Set_UVScale(fUVScale);
		pWater->Set_Noise(fNoise);
		pWater->Set_WaveSpeed(fWaveSpeed);
	}
}

void CTool_Manager::Show_DungeonEditor()
{
	_int iLevel = -1;
	wstring szLayerTag;
	wstring szDataPath;

	if ((false == m_bWorldLevel && false == m_bTempleLevel && false == m_bStrongHoldLevel && false == m_bQuestLevel && false == m_bDvalinLevel) ||
		(true == m_bWorldLevel && true == m_bTempleLevel && true == m_bStrongHoldLevel && true == m_bQuestLevel && true == m_bDvalinLevel))
		return;

	else if (true == m_bWorldLevel)
	{
		iLevel = 2;
		szLayerTag = m_LayerTag_WorldSpawner;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/Spawner_World.dat");
	}


	else if (true == m_bTempleLevel)
	{
		iLevel = 3;
		szLayerTag = m_LayerTag_TempleSpawner;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/Spawner_Temple.dat");
	}

	else if (true == m_bStrongHoldLevel)
	{
		iLevel = 4;
		szLayerTag = m_LayerTag_StrongHoldSpawner;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/Spawner_StrongHold.dat");
	}

	else if (true == m_bQuestLevel)
	{
		iLevel = 5;
		szLayerTag = m_LayerTag_QuestSpawner;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/Spawner_Quest.dat");
	}

	else if (true == m_bDvalinLevel)
	{
		iLevel = 6;
		szLayerTag = m_LayerTag_DvalinSpawner;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/Spawner_Dvalin.dat");
	}

	if (!ImGui::IsWindowHovered() &&
		!ImGui::IsAnyItemHovered() &&
		m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		if (nullptr == m_pSelectObject)
		{
			_float2 vScreenPoint = Compute_ScreenPoint();

			CRay* pRay = CRay::ScreenPointToRay(m_pDevice, m_pContext, vScreenPoint);
			CGameObject* pHitObject = nullptr;

			_float fDist = -1.0f;

			if (CRay::RayCast(LEVEL_EDIT, szLayerTag, pRay->Get_RayPos(), pRay->Get_RayDir(), &pHitObject, fDist))
			{
				if (nullptr != pHitObject)
					m_pSelectObject = pHitObject;
			}

			Safe_Release(pRay);
		}
	}

	if (!ImGui::IsWindowHovered() &&
		!ImGui::IsAnyItemHovered() &&
		m_pGameInstance->Key_Down('P')) 
	{
		m_fSpawnerPosition[0] = m_fPickingPoint[0];
		m_fSpawnerPosition[1] = m_fPickingPoint[1];
		m_fSpawnerPosition[2] = m_fPickingPoint[2];
	}

	ImGui::SeparatorText("Spawner Desc");

	ImGui::Text("Spawner Position");
	ImGui::InputFloat3("##Spawner Pos", m_fSpawnerPosition);

	ImGui::Text("Spawner Range");
	ImGui::InputFloat("##Spanwer Range", &m_fSpawnerRange);

	if (ImGui::Button("Create", ImVec2(110, 25))) 
	{
		CEdit_Spawner::SPAWNER_DESC tSpawnerDesc = {};
		{
			tSpawnerDesc.iLevel;
			tSpawnerDesc.fSpawnRange = m_fSpawnerRange;
			tSpawnerDesc.vSpawnerPos = _float4(m_fSpawnerPosition[0], m_fSpawnerPosition[1], m_fSpawnerPosition[2], 1.0f);
		}

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_Spawner"), szLayerTag, &tSpawnerDesc)))
			return;

		m_fSpawnerPosition[0] = 0.0f;
		m_fSpawnerPosition[1] = 0.0f;
		m_fSpawnerPosition[2] = 0.0f;

		m_fSpawnerRange = 0.0f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(110, 25))) 
	{
		ifstream InFile(szDataPath, ios::binary);

		if (InFile.is_open()) 
		{
			_uint iNumSpawner;
			InFile.read(reinterpret_cast<_char*>(&iNumSpawner), sizeof(_uint));

			for (_int i = 0; i < iNumSpawner; i++) 
			{
				_float4 vSpawnPos;
				_float fSpawnerRange;
				_uint iNumMonster;

				InFile.read(reinterpret_cast<_char*>(&vSpawnPos), sizeof(_float4));
				InFile.read(reinterpret_cast<_char*>(&fSpawnerRange), sizeof(_float));
				InFile.read(reinterpret_cast<_char*>(&iNumMonster), sizeof(_uint));

				vector<CEdit_Spawner::MONSTER_INFO> MonsterInfos;

				for (_int j = 0; j < iNumMonster; j++) 
				{
					CEdit_Spawner::MONSTER_INFO tMonsterInfo = {};
					{
						InFile.read(reinterpret_cast<_char*>(&tMonsterInfo.eCreatureType), sizeof(CEdit_Spawner::CREATURE_TYPE));
						InFile.read(reinterpret_cast<_char*>(&tMonsterInfo.vSpawnPosition), sizeof(_float4));
					}

					MonsterInfos.push_back(tMonsterInfo);
				}

				CEdit_Spawner::SPAWNER_DESC tSpawnerDesc = {};
				{
					tSpawnerDesc.iLevel = iLevel;
					tSpawnerDesc.vSpawnerPos = vSpawnPos;
					tSpawnerDesc.fSpawnRange = fSpawnerRange;
					tSpawnerDesc.pMonsterInfo = &MonsterInfos;
				}

				if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_Spawner"), szLayerTag, &tSpawnerDesc)))
					return;
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Save", ImVec2(110, 25))) 
	{
		list<CGameObject*>* pGameObjects = m_pGameInstance->Find_Objects(LEVEL_EDIT, szLayerTag);

		if (nullptr != pGameObjects)
		{
			ofstream OutFile(szDataPath, ios::binary);

			_uint iNumSpawner = pGameObjects->size();

			OutFile.write(reinterpret_cast<_char*>(&iNumSpawner), sizeof(_uint));

			for (auto& pGameObject : *pGameObjects) 
			{
				CEdit_Spawner* pSpawner = (CEdit_Spawner*)pGameObject;

				_float4 vSpawnPos = pSpawner->Get_SpawnerPosition();
				_float fSpawnerRange = pSpawner->Get_SpawnerRange();

				vector<CEdit_Spawner::MONSTER_INFO>* pMonsterInfo = pSpawner->Get_MonsterInfo();
				_uint iNumMonster = pMonsterInfo->size();

				OutFile.write(reinterpret_cast<_char*>(&vSpawnPos), sizeof(_float4));
				OutFile.write(reinterpret_cast<_char*>(&fSpawnerRange), sizeof(_float));
				OutFile.write(reinterpret_cast<_char*>(&iNumMonster), sizeof(_uint));

				for (_int i = 0; i < iNumMonster; i++) 
				{
					CEdit_Spawner::MONSTER_INFO tMonsterInfo = {};
					{
						tMonsterInfo.eCreatureType = (*pMonsterInfo)[i].eCreatureType;
						tMonsterInfo.vSpawnPosition = (*pMonsterInfo)[i].vSpawnPosition;
					}

					OutFile.write(reinterpret_cast<_char*>(&tMonsterInfo.eCreatureType), sizeof(CEdit_Spawner::CREATURE_TYPE));
					OutFile.write(reinterpret_cast<_char*>(&tMonsterInfo.vSpawnPosition), sizeof(_float4));
				}
			}

			OutFile.close();
		}
	}

	list<CGameObject*>* pWorldSpawners = m_pGameInstance->Find_Objects(LEVEL_EDIT, szLayerTag);
	
	if (nullptr != pWorldSpawners)
	{
		switch (iLevel)
		{
		case 2:
			ImGui::SeparatorText("World Spawner List");
			break;
		case 3:
			ImGui::SeparatorText("Temple Spawner List");
			break;
		case 4:
			ImGui::SeparatorText("StrongHold Spawner List");
			break;
		case 5:
			ImGui::SeparatorText("Quest Spawner List");
			break;
		case 6:
			ImGui::SeparatorText("Dvalin Spawner List");
			break;
		}

		_int iIndex = 0;

		for (auto iter = pWorldSpawners->begin(); iter != pWorldSpawners->end(); iter++)
		{
			CTransform* pTransform = (CTransform*)((*iter)->Find_Component(TEXT("Com_Transform")));
			
			_float4 vSpawnerPos;
			XMStoreFloat4(&vSpawnerPos, pTransform->Get_State(CTransform::STATE_POSITION));

			string szName = "Spawner_";
			{
				szName.append(to_string(vSpawnerPos.x));
				szName.append("_");
				szName.append(to_string(vSpawnerPos.y));
				szName.append("_");
				szName.append(to_string(vSpawnerPos.z));
			}

			if (ImGui::Selectable(szName.c_str())) 
			{
				m_pSelectObject = *iter;
				m_iSpawnerIndex = iIndex;
			}

			iIndex++;
		}
	}

	if (nullptr != m_pSelectObject)
	{
		if (ImGui::Button("Delete", ImVec2(110, 25)))
		{
			m_pSelectObject->Set_Destroy(true);
			m_pSelectObject = nullptr;
			m_iSpawnerIndex = 0;
		}
	}

	if (nullptr != m_pSelectObject)
	{
		if (!ImGui::IsWindowHovered() &&
			!ImGui::IsAnyItemHovered() &&
			m_pGameInstance->Key_Down('O'))
		{
			m_fMonsterSpawnPos[0] = m_fPickingPoint[0];
			m_fMonsterSpawnPos[1] = m_fPickingPoint[1];
			m_fMonsterSpawnPos[2] = m_fPickingPoint[2];
		}

		CEdit_Spawner* pSpawner = (CEdit_Spawner*)m_pSelectObject;

		ImGui::SeparatorText("Monster Desc");

		ImGui::Text("Monster Type");
		if (ImGui::BeginCombo("##Monster Type", m_pCurrentMonsterType))
		{
			for (_int i = 0; i < IM_ARRAYSIZE(m_pMonsterType); i++)
			{
				_bool bSelected = (m_pCurrentMonsterType == m_pMonsterType[i]);

				if (ImGui::Selectable(m_pMonsterType[i], bSelected))
				{
					m_pCurrentMonsterType = m_pMonsterType[i];
					m_iCurrentMonsterTypeIndex = i;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Monster Spawn Position");
		ImGui::InputFloat3("##Spawner Pos", m_fMonsterSpawnPos);

		if (ImGui::Button("Add", ImVec2(110, 25)))
		{
			if (m_iCurrentMonsterTypeIndex >= 9) 
			{
				_float4 vMonsterSpawnPos = _float4(m_fMonsterSpawnPos[0], m_fMonsterSpawnPos[1], m_fMonsterSpawnPos[2], 1.0f);

				CEdit_Spawner::MONSTER_INFO tMonsterInfo = {};
				{
					tMonsterInfo.eCreatureType = (CEdit_Spawner::CREATURE_TYPE)m_iCurrentMonsterTypeIndex;
					tMonsterInfo.vSpawnPosition = vMonsterSpawnPos;
				}

				pSpawner->Add_MonsterInfo(&tMonsterInfo);

				m_fMonsterSpawnPos[0] = 0.0f;
				m_fMonsterSpawnPos[1] = 0.0f;
				m_fMonsterSpawnPos[2] = 0.0f;
			}
		}

		ImGui::SeparatorText("Monster List");

		vector<CEdit_Spawner::MONSTER_INFO>* pMonsters = pSpawner->Get_MonsterInfo();

		if (nullptr == pMonsters)
			return;

		for (_int i = 0; i < pMonsters->size(); i++)
		{
			string szName;

			switch ((*pMonsters)[i].eCreatureType)
			{
			case CEdit_Spawner::MONSTER_CHUCHU:
				szName = "ChuChu";
				break;
			case CEdit_Spawner::MONSTER_SLIME_WATER:
				szName = "Slime Water";
				break;
			case CEdit_Spawner::MONSTER_SLIME_ICE:
				szName = "Slime Ice";
				break;
			case CEdit_Spawner::MONSTER_HILI_NORMAL:
				szName = "Hili Normal";
				break;
			case CEdit_Spawner::MONSTER_BRUTE_SHIELD:
				szName = "Brute Shield";
				break;
			case CEdit_Spawner::MONSTER_BRUTE_AXE:
				szName = "Brute Axe";
				break;
			default:
				break;
			}

			if (ImGui::Selectable(szName.c_str()))
			{
				m_pSelectMonsterInfo = &(*pMonsters)[i];
				m_iMonsterIndex = i;
			}
		}

		if (nullptr != m_pSelectMonsterInfo)
		{
			if (ImGui::Button("Erase", ImVec2(110, 25))) 
			{
				pMonsters->erase(pMonsters->begin() + m_iMonsterIndex);
				m_pSelectMonsterInfo = nullptr;
				m_iMonsterIndex = -1;
			}
		}
	}
}

void CTool_Manager::Show_NavigationEditor()
{
	/* 터레인 점 찍기 */
	if (m_iPointIndex < 3 && !ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		_float2 vScreenPoint = Compute_ScreenPoint();
		_float3 vPosition_InTerrain;

		CGameObject* pTerrain = m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);

		if (nullptr == pTerrain)
			return;

		CNaviMesh* pNaviCom = (CNaviMesh*)pTerrain->Find_Component(TEXT("Com_NaviMesh"));

		if (nullptr == pNaviCom)
			return;

		CRay* pRay = CRay::ScreenPointToRay(m_pDevice, m_pContext, vScreenPoint);

		if (CRay::RayCast(pRay->Get_RayPos(), pRay->Get_RayDir(), pTerrain, &vPosition_InTerrain))
		{
			pNaviCom->Search_Point(&vPosition_InTerrain);
			XMStoreFloat3(&m_vPoints[m_iPointIndex], XMLoadFloat3(&vPosition_InTerrain));

			m_iPointIndex++;
		}

		Safe_Release(pRay);
	}
	
	/* 터레인 셀 삭제 */
	if (m_iPointIndex < 3 && !ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && m_pGameInstance->Key_Down(VK_RBUTTON)) 
	{
		_float2 vScreenPoint = Compute_ScreenPoint();
		_float3 vPosition_InTerrain;

		CGameObject* pTerrain = m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);
		CNaviMesh* pNaviCom = (CNaviMesh*)pTerrain->Find_Component(TEXT("Com_NaviMesh"));

		if (nullptr == pTerrain)
			return;

		CRay* pRay = CRay::ScreenPointToRay(m_pDevice, m_pContext, vScreenPoint);

		if (CRay::RayCast(pRay->Get_RayPos(), pRay->Get_RayDir(), pTerrain, &vPosition_InTerrain))
			pNaviCom->Delete_Triangle(vPosition_InTerrain);

		Safe_Release(pRay);
	}

	ImGui::SeparatorText("Navigation Editor");

	ImGui::Text("Point A");
	ImGui::InputFloat3("##Point A", &m_vPoints[0].x);

	ImGui::Text("Point B");
	ImGui::InputFloat3("##Point B", &m_vPoints[1].x);

	ImGui::Text("Point C");
	ImGui::InputFloat3("##Point C", &m_vPoints[2].x);

	if (ImGui::Button("Create"))
	{
		_float3 PointA, PointB, PointC;

		PointA = m_vPoints[0];
		PointB = m_vPoints[1];
		PointC = m_vPoints[2];

		if (m_vPoints[0].x < m_vPoints[1].x)
		{
			if (m_vPoints[0].x < m_vPoints[2].x)
			{
				PointA = m_vPoints[0];

				if (m_vPoints[1].z > m_vPoints[2].z)
				{
					PointB = m_vPoints[1];
					PointC = m_vPoints[2];
				}
				else
				{
					PointB = m_vPoints[2];
					PointC = m_vPoints[1];
				}
			}
		}
		else
		{
			if (m_vPoints[1].x < m_vPoints[2].x)
			{
				PointA = m_vPoints[1];

				if (m_vPoints[0].z > m_vPoints[2].z)
				{
					PointB = m_vPoints[0];
					PointC = m_vPoints[2];
				}
				else
				{
					PointB = m_vPoints[2];
					PointC = m_vPoints[0];
				}
			}
			else
			{
				PointA = m_vPoints[2];

				if (m_vPoints[0].z > m_vPoints[1].z)
				{
					PointB = m_vPoints[0];
					PointC = m_vPoints[1];
				}
				else
				{
					PointB = m_vPoints[1];
					PointC = m_vPoints[0];
				}
			}
		}

		_float3 vSortPoints[3] = {};

		_vector vFirst = XMLoadFloat3(&PointB) - XMLoadFloat3(&PointA);
		_vector vSecond = XMLoadFloat3(&PointC) - XMLoadFloat3(&PointA);

		_float3 vResult;
		XMStoreFloat3(&vResult, XMVector3Cross(vFirst, vSecond));

		if (vResult.y < 0.0f)
		{
			vSortPoints[0] = PointA;
			vSortPoints[1] = PointC;
			vSortPoints[2] = PointB;
		}
		else
		{
			vSortPoints[0] = PointA;
			vSortPoints[1] = PointB;
			vSortPoints[2] = PointC;
		}

		CGameObject* pTerrain = m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);

		if (nullptr == pTerrain)
			return;

		CNaviMesh* pNaviCom = (CNaviMesh*)pTerrain->Find_Component(TEXT("Com_NaviMesh"));

		if (nullptr == pNaviCom)
			return;

		pNaviCom->Add_Triangle(vSortPoints);

		m_iPointIndex = 0;
		ZeroMemory(m_vPoints, sizeof(_float3) * 3);

	}

	ImGui::SameLine();

	if (ImGui::Button("Clear")) 
	{
		m_iPointIndex = 0;
		ZeroMemory(m_vPoints, sizeof(_float3) * 3);
	}

	ImGui::SameLine();

	if (ImGui::Button("Save")) 
	{
		CGameObject* pTerrain = m_pGameInstance->Find_Object(LEVEL_EDIT, m_LayerTag_Terrain);

		if (nullptr == pTerrain)
			return;

		CNaviMesh* pNaviCom = (CNaviMesh*)pTerrain->Find_Component(TEXT("Com_NaviMesh"));

		if (nullptr == pNaviCom)
			return;

		pNaviCom->Save_Triangles();
	}
		
}

_float2 CTool_Manager::Compute_ScreenPoint()
{
	POINT ptMouse;

	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	return _float2((_float)ptMouse.x, (_float)ptMouse.y);
}

HRESULT CTool_Manager::Create_GameObject()
{
	wstring szLayerTag;

	if ((false == m_bWorldLevel && false == m_bTempleLevel && false == m_bStrongHoldLevel && false == m_bQuestLevel && false == m_bDvalinLevel) ||
		(true == m_bWorldLevel && true == m_bTempleLevel && true == m_bStrongHoldLevel && true == m_bQuestLevel && true == m_bDvalinLevel))
		return E_FAIL;

	else if (true == m_bWorldLevel)
		szLayerTag = m_LayerTag_GameObjects;


	else if (true == m_bTempleLevel)
		szLayerTag = m_LayerTag_Temple;

	else if (true == m_bStrongHoldLevel)
		szLayerTag = m_LayerTag_StrongHold;

	else if (true == m_bQuestLevel)
		szLayerTag = m_LayerTag_Quest;

	else if (true == m_bDvalinLevel)
		szLayerTag = m_LayerTag_Dvalin;

	if (0 > m_iCurrentObjectIndex)
		return E_FAIL;

	CEdit_Object::EDIT_DESC EditDesc = {};
	{
		WORLD_ELEMENT_TYPE eType = (WORLD_ELEMENT_TYPE)m_iCurrentItemTypeIndex;
		_float4x4 vWorldMatrix;
		{
			XMStoreFloat4x4(&vWorldMatrix, XMMatrixIdentity());

			_float3 vSpawnPos = _float3(0.0f, 0.0f, 0.0f);
			Compute_DepthPicking(_float2(800.0f, 450.f), &vSpawnPos);

			_float4 vPosition = _float4(vSpawnPos.x, vSpawnPos.y, vSpawnPos.z, 1.0f);
			{
				vPosition.x = floor(vPosition.x);
				vPosition.y = floor(vPosition.y);
				vPosition.z = floor(vPosition.z);
			}

			memcpy(&vWorldMatrix.m[3][0], &vPosition, sizeof(_float4));
		}
		wstring strModelTag = m_ProtoTypeTag[m_iCurrentObjectIndex];

		EditDesc.eType = eType;
		EditDesc.vWorldMatrix = vWorldMatrix;
		EditDesc.strModelTag = m_ProtoTypeTag[m_iCurrentObjectIndex];
	}

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_EditObject"), szLayerTag, &EditDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Manager::Load_GameObjects()
{
	wstring szLayerTag;
	wstring szDataPath;

	if ((false == m_bWorldLevel && false == m_bTempleLevel && false == m_bStrongHoldLevel && false == m_bQuestLevel && false == m_bDvalinLevel) ||
		(true == m_bWorldLevel && true == m_bTempleLevel && true == m_bStrongHoldLevel && true == m_bQuestLevel && true == m_bDvalinLevel))
		return E_FAIL;

	else if (true == m_bWorldLevel) 
	{
		szLayerTag = m_LayerTag_GameObjects;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/WorldData.dat");
	}

	else if (true == m_bTempleLevel) 
	{
		szLayerTag = m_LayerTag_Temple;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/TempleData.dat");
	}
		

	else if (true == m_bStrongHoldLevel) 
	{
		szLayerTag = m_LayerTag_StrongHold;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/StrongHoldData.dat");
	}
		

	else if (true == m_bQuestLevel) 
	{
		szLayerTag = m_LayerTag_Quest;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/QuestData.dat");
	}

	else if (true == m_bDvalinLevel)
	{
		szLayerTag = m_LayerTag_Dvalin;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/DvalinData.dat");
	}

	if (LEVEL_EDIT != m_pGameInstance->Get_CurrentLevel())
		return E_FAIL;

	ifstream InFile(szDataPath, ios::binary);

	if (InFile.is_open()) 
	{
		_int iNumGameObject = -1;

		InFile.read(reinterpret_cast<_char*>(&iNumGameObject), sizeof(_int));

		if (iNumGameObject < 0)
			return E_FAIL;

		for (_int i = 0; i < iNumGameObject; i++)
		{
			WORLD_ELEMENT_TYPE eType = WORLD_ELEMENT_END;
			InFile.read(reinterpret_cast<_char*>(&eType), sizeof(WORLD_ELEMENT_TYPE));

			if (WORLD_ELEMENT_END == eType)
				return E_FAIL;

			_float4x4 vWorldMatrix;
			InFile.read(reinterpret_cast<_char*>(&vWorldMatrix), sizeof(_float4x4));

			_char ModelTag[MAX_PATH];
			InFile.read(reinterpret_cast<_char*>(&ModelTag), MAX_PATH);

			wstring strModelTag = ConvertCToWC(ModelTag);

			CEdit_Object::EDIT_DESC EditDesc = {};
			{
				EditDesc.eType = eType;
				EditDesc.vWorldMatrix = vWorldMatrix;
				EditDesc.strModelTag = strModelTag;
			}

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_EDIT, TEXT("Prototype_GameObject_EditObject"), szLayerTag, &EditDesc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CTool_Manager::Save_GameObjects()
{
	wstring szLayerTag;
	wstring szDataPath;

	if ((false == m_bWorldLevel && false == m_bTempleLevel && false == m_bStrongHoldLevel && false == m_bQuestLevel && false == m_bDvalinLevel) ||
		(true == m_bWorldLevel && true == m_bTempleLevel && true == m_bStrongHoldLevel && true == m_bQuestLevel && true == m_bDvalinLevel))
		return E_FAIL;

	else if (true == m_bWorldLevel)
	{
		szLayerTag = m_LayerTag_GameObjects;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/WorldData.dat");
	}

	else if (true == m_bTempleLevel)
	{
		szLayerTag = m_LayerTag_Temple;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/TempleData.dat");
	}


	else if (true == m_bStrongHoldLevel)
	{
		szLayerTag = m_LayerTag_StrongHold;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/StrongHoldData.dat");
	}


	else if (true == m_bQuestLevel)
	{
		szLayerTag = m_LayerTag_Quest;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/QuestData.dat");
	}

	else if (true == m_bDvalinLevel)
	{
		szLayerTag = m_LayerTag_Dvalin;
		szDataPath = TEXT("../../Reference/Resources/Data/Map/DvalinData.dat");
	}

	list<CGameObject*>* pGameObjects = m_pGameInstance->Find_Objects(LEVEL_EDIT, szLayerTag);

	if (nullptr == pGameObjects)
		return E_FAIL;

	ofstream OutFile(szDataPath, ios::binary);

	if (OutFile.is_open()) 
	{
		_int iNumGameObject = (_int)pGameObjects->size();
		OutFile.write(reinterpret_cast<_char*>(&iNumGameObject), sizeof(_int));

		for (auto& pGameObject : *pGameObjects)
		{
			CEdit_Object* pEditObject = (CEdit_Object*)pGameObject;

			WORLD_ELEMENT_TYPE eType = pEditObject->Get_Type();
			_float4x4 vWorldMatrix = pEditObject->Get_WorldMatrix();
			wstring strModelTag = pEditObject->Get_Tag();

			string szModelTag = ConvertWCToC(strModelTag);
			_char ModelTag[MAX_PATH];

			strcpy_s(ModelTag, MAX_PATH, szModelTag.c_str());

			OutFile.write(reinterpret_cast<_char*>(&eType), sizeof(WORLD_ELEMENT_TYPE));
			OutFile.write(reinterpret_cast<_char*>(&vWorldMatrix), sizeof(_float4x4));
			OutFile.write(reinterpret_cast<_char*>(&ModelTag), MAX_PATH);
		}
	}

	return S_OK;
}

void CTool_Manager::Move_AllObjects(_float3 vDir)
{
	list<CGameObject*>* pGameObjects = m_pGameInstance->Find_Objects(LEVEL_EDIT, m_LayerTag_GameObjects);

	if (nullptr == pGameObjects)
		return;

	for (auto& pGameObject : *pGameObjects) 
	{
		CTransform* pTransform = (CTransform*)pGameObject->Find_Component(TEXT("Com_Transform"));

		if (nullptr == pTransform)
			return;

		_vector Position = pTransform->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vDir);
		
		pTransform->Set_State(CTransform::STATE_POSITION, Position);
	}
}

HRESULT CTool_Manager::Loading_Path(const wstring& strDataPath, vector<wstring>* pPath)
{
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

				Loading_Path(strNewFilePath, pPath);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".dat")))
		{
			strFullPath = L"";
			strFullPath += szDir;
			strFullPath += tagFindData.cFileName;

			wstring strProtoTag = TEXT("Prototype_Component_Model_");

			_tchar szName[MAX_PATH];
			_wsplitpath_s(strFullPath.c_str(), NULL, NULL, NULL, NULL, szName, MAX_PATH, NULL, NULL);

			strProtoTag.append(szName);

			pPath->push_back(strFullPath);
		}

		bResult = FindNextFile(hFinder, &tagFindData);
	}

	FindClose(hFinder);

	return S_OK;
}

HRESULT CTool_Manager::Compute_DepthPicking(_float2 vScreenPoint, _float3* pPickPos)
{
	if (nullptr == pPickPos)
		return E_FAIL;

	if ((vScreenPoint.x >= 0 && vScreenPoint.x <= g_iWinSizeX) &&
		(vScreenPoint.y >= 0 && vScreenPoint.y <= g_iWinSizeY))
	{
		CRenderTarget* pSceneWorld = m_pGameInstance->Find_RenderTarget(TEXT("Target_SceneWorld"));
		ID3D11RenderTargetView* pRTV = pSceneWorld->Get_RenderTargetView();
		ID3D11Resource* pWorldResource = nullptr;

		pRTV->GetResource(&pWorldResource);

		ID3D11Texture2D* pWorldTexture = static_cast<ID3D11Texture2D*>(pWorldResource);

		D3D11_BOX srcBox = {};
		{
			srcBox.left = (int)vScreenPoint.x;
			srcBox.top = (int)vScreenPoint.y;
			srcBox.right = (int)vScreenPoint.x + 1;
			srcBox.bottom = (int)vScreenPoint.y + 1;
			srcBox.front = 0;
			srcBox.back = 1;
		}

		m_pContext->CopySubresourceRegion(
			m_pDepthCopyTexture,
			0, 0, 0, 0,
			pWorldTexture, 0, &srcBox);

		D3D11_MAPPED_SUBRESOURCE msr;
		ZeroMemory(&msr, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(m_pDepthCopyTexture, 0, D3D11_MAP_READ, 0, &msr);

		_float4 vPosition = *reinterpret_cast<_float4*>(msr.pData);

		m_pContext->Unmap(m_pDepthCopyTexture, 0);

		XMStoreFloat3(pPickPos, XMLoadFloat4(&vPosition));

		Safe_Release(pWorldResource);
	}

	return S_OK;
}

HRESULT CTool_Manager::Create_Staging()
{
	D3D11_TEXTURE2D_DESC DepthCopyTextureDesc;
	ZeroMemory(&DepthCopyTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	{
		DepthCopyTextureDesc.Width = 1;
		DepthCopyTextureDesc.Height = 1;
		DepthCopyTextureDesc.MipLevels = 1;
		DepthCopyTextureDesc.ArraySize = 1;
		DepthCopyTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		DepthCopyTextureDesc.SampleDesc.Quality = 0;
		DepthCopyTextureDesc.SampleDesc.Count = 1;

		DepthCopyTextureDesc.Usage = D3D11_USAGE_STAGING;
		DepthCopyTextureDesc.BindFlags = 0;
		DepthCopyTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		DepthCopyTextureDesc.MiscFlags = 0;
	}

	if (FAILED(m_pDevice->CreateTexture2D(&DepthCopyTextureDesc, nullptr, &m_pDepthCopyTexture)))
		return E_FAIL;

	return S_OK;
}

CTool_Manager* CTool_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
	CTool_Manager* pInstance = new CTool_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CTool_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDepthCopyTexture);

	for (size_t i = 0; i < m_ProtoTypeTag.size(); i++)
		Safe_Delete_Array(m_ppItem_Model[i]);

	Safe_Delete_Array(m_ppItem_Model);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
