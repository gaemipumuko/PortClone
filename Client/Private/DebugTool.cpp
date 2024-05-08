#include "stdafx.h"

#ifndef _DEBUGNUI

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "DebugTool.h"

#include "Player.h"
#include "Part_Weapon.h"
#include "Camera_Main.h"

#include "GameInstance.h"

#define BEGIN_TAB(strTag)	if(ImGui::BeginTabItem(strTag)) {
#define END_TAB				ImGui::EndTabItem(); }

struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;
	bool                AutoScroll;

	ExampleAppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have random access to the result of our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of
				// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
				// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
				// within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
				// on your side is recommended. Using ImGuiListClipper requires
				// - A) random access into your data
				// - B) items all being the  same height,
				// both of which we can handle since we have an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display
				// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
				// it possible (and would be recommended if you want to search through tens of thousands of entries).
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
			// Using a scrollbar or mouse-wheel will take away from the bottom edge.
			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}
};

static ExampleAppLog			g_ConsoleLog;
static ImGuiWindowFlags			g_ImGuiWindowFlags = 0;

CDebugTool::CDebugTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

void CDebugTool::Main_Window()
{
	ImGui::Begin("Main", 0, g_ImGuiWindowFlags);

	if (ImGui::BeginTabBar("Tab"))
	{
		BEGIN_TAB("Player")
			CDebugTool::MainWin_Player();
		END_TAB
		BEGIN_TAB("Debug")
			CDebugTool::MainWin_Debug();
		END_TAB
		BEGIN_TAB("Render")
			CDebugTool::MainWin_Render();
		END_TAB

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CDebugTool::ConsoleLog_Window()
{
	ImGui::Begin("Console Log", 0, g_ImGuiWindowFlags);
	ImGui::End();

	g_ConsoleLog.Draw("Console Log");
}

void CDebugTool::MainWin_Player()
{
	if (ImGui::BeginTabBar("PlayerTabBar", 0))
	{
		if (ImGui::BeginTabItem("Pivot"))
		{
			ImGui::SeparatorText("Select Model");

			ImGui::NewLine();
			ImGui::SeparatorText("Edit Pivot");
			ImGui::Text("Scale       : "); ImGui::SameLine();
			ImGui::InputFloat4("##PivotScale", (_float*)(&m_vPivotScale));
			ImGui::Text("Rotation    : "); ImGui::SameLine();
			ImGui::InputFloat4("##PivotRotation", (_float*)(&m_vPivotRotation));
			ImGui::Text("Translation : "); ImGui::SameLine();
			ImGui::InputFloat4("##PivotTranslation", (_float*)(&m_vPivotTranslation));

			ImGui::NewLine();
			if (ImGui::Button("Invalidate Pivot", { -1, 0 }))
			{
				_matrix PivotMatrix = XMMatrixIdentity();
				_vector vRotation =
					XMQuaternionRotationRollPitchYaw(
						XMConvertToRadians(m_vPivotRotation.x),
						XMConvertToRadians(m_vPivotRotation.y),
						XMConvertToRadians(m_vPivotRotation.z));

				PivotMatrix = XMMatrixAffineTransformation(XMLoadFloat4(&m_vPivotScale), { 0.f, 0.f, 0.f, 1.f }, vRotation, XMLoadFloat4(&m_vPivotTranslation));

				static_cast<CModel*>(
					static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"))
					->Get_PartObject(CPlayer::PART_WEAPON, 0)->Find_Component(L"Com_Model"))->Set_PivotMatrix(PivotMatrix);
			}

			ImGui::SeparatorText("Player Position");

			CGameObject* pPlayer = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));

			if (nullptr != pPlayer) 
			{
				CTransform* pTransform = (CTransform*)pPlayer->Find_Component(TEXT("Com_Transform"));

				if (nullptr != pTransform)
				{
					_float4 vPosition;
					XMStoreFloat4(&vPosition, pTransform->Get_State(CTransform::STATE_POSITION));

					_float fPosition[3];
					{
						fPosition[0] = vPosition.x;
						fPosition[1] = vPosition.y;
						fPosition[2] = vPosition.z;
					}

					ImGui::InputFloat3("Position", fPosition);
				}
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void CDebugTool::MainWin_Debug()
{
	if (ImGui::BeginTabBar("DebugBar", 0))
	{
		CCamera_Main* pCamera =  static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
		{
			if (ImGui::BeginTabItem("Camera"))
			{
				m_fSpeedPerSec = static_cast<CTransform*>(pCamera->Find_Component((pCamera->Get_IsFreeCamera() == TRUE) ? L"Com_FreeTransform" : L"Com_Transform"))->Get_SpeedPerSec();
				m_fRotationPerSec = static_cast<CTransform*>(pCamera->Find_Component((pCamera->Get_IsFreeCamera() == TRUE) ? L"Com_FreeTransform" : L"Com_Transform"))->Get_RotationPerSec();

				ImGui::SeparatorText("Camera Mode");
				if (ImGui::BeginTable("SeparateType", 2))
				{
					ImGui::TableNextColumn();
					if (ImGui::RadioButton("Free Camera", pCamera->Get_IsFreeCamera() == TRUE))
						pCamera->Set_FreeCamera(TRUE);

					ImGui::TableNextColumn();
					if (ImGui::RadioButton("Main Camera", pCamera->Get_IsFreeCamera() == FALSE))
						pCamera->Set_FreeCamera(FALSE);

					ImGui::EndTable();
				}

				ImGui::NewLine();
				ImGui::SeparatorText("Camera Desc");
				ImGui::Text("Spd Per Sec : "); ImGui::SameLine();
				if (ImGui::InputFloat("##InputFreeCameraSPS", &m_fSpeedPerSec, 8))
				{
					static_cast<CTransform*>(pCamera->Find_Component((pCamera->Get_IsFreeCamera() == TRUE) ? L"Com_FreeTransform" : L"Com_Transform"))->Set_Desc({ m_fSpeedPerSec, XMConvertToRadians(m_fRotationPerSec) });
				}
				
				ImGui::Text("Alert) Input : Degree, Output : Radian");
				ImGui::Text("Rot Per Sec : "); ImGui::SameLine();
				if (ImGui::InputFloat("##InputFreeCameraRPS", &m_fRotationPerSec, 8))
				{
					static_cast<CTransform*>(pCamera->Find_Component((pCamera->Get_IsFreeCamera() == TRUE) ? L"Com_FreeTransform" : L"Com_Transform"))->Set_Desc({ m_fSpeedPerSec, XMConvertToRadians(m_fRotationPerSec) });
				}

				ImGui::EndTabItem();
			}
		}

		if (ImGui::BeginTabItem("Cursor"))
		{
			ImGui::SeparatorText("Fix Cursor");
			if (ImGui::BeginTable("FixCursor", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("TRUE", m_bFixCursor == TRUE))
				{
					m_bFixCursor = TRUE;
					m_pGameInstance->Fix_Mouse();
				}

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("FALSE", m_bFixCursor == FALSE))
				{
					m_bFixCursor = FALSE;
					m_pGameInstance->FreeFix_Mouse();
				}

				ImGui::EndTable();
			}

			ImGui::NewLine();
			ImGui::SeparatorText("Clip Cursor");
			if (ImGui::BeginTable("ClipCursor", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("TRUE", m_bClipCursor == TRUE))
				{
					m_bClipCursor = TRUE;
					m_pGameInstance->Clip_Mouse();
				}

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("FALSE", m_bClipCursor == FALSE))
				{
					m_bClipCursor = FALSE;
					m_pGameInstance->FreeClip_Mouse();
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Render"))
		{
			ImGui::SeparatorText("Show RTV");
			if (ImGui::BeginTable("SeparateType", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("TRUE ", m_pRenderer->Get_RenderRTV() == TRUE))
					m_pRenderer->Set_RenderRTV(TRUE);

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("FALSE", m_pRenderer->Get_RenderRTV() == FALSE))
					m_pRenderer->Set_RenderRTV(FALSE);

				ImGui::EndTable();
			}

			ImGui::NewLine();
			ImGui::SeparatorText("Show DevCom");
			if (ImGui::BeginTable("ShowDevCom", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("TRUE ", m_pRenderer->Set_RenderDebugCom() == TRUE))
					m_pRenderer->Set_RenderDebugCom(TRUE);

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("FALSE", m_pRenderer->Set_RenderDebugCom() == FALSE))
					m_pRenderer->Set_RenderDebugCom(FALSE);

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void CDebugTool::MainWin_Render()
{
	if (ImGui::BeginTabBar("RenderTabBar", 0))
	{
		if (ImGui::BeginTabItem("ColorGrade"))
		{
			ImGui::SeparatorText("Edit ColorGrade Constant Buffer");
			ImGui::Text("ColorFilter : "); ImGui::SameLine();
			if (ImGui::SliderFloat3("##SliderCGColorFilter", (_float*)&m_vColorFilter, 0.f, 2.f))
			{
				m_pGameInstance->Update_ColorGradeCB(m_vColorFilter, m_fSaturation, m_fExposure, m_fContrast);
			}

			ImGui::Text("Saturation  : "); ImGui::SameLine();
			if (ImGui::SliderFloat("##SliderCGSaturation", &m_fSaturation, -5.f, 5.f))
			{
				m_pGameInstance->Update_ColorGradeCB(m_vColorFilter, m_fSaturation, m_fExposure, m_fContrast);
			}

			ImGui::Text("Exposure    : "); ImGui::SameLine();
			if (ImGui::SliderFloat("##SliderCGExposure", &m_fExposure, -5.f, 5.f))
			{
				m_pGameInstance->Update_ColorGradeCB(m_vColorFilter, m_fSaturation, m_fExposure, m_fContrast);
			}

			ImGui::Text("Contrast    : "); ImGui::SameLine();
			if (ImGui::SliderFloat("##SliderCGContrast", &m_fContrast, 0.99f, 1.01f, "%.8f"))
			{
				m_pGameInstance->Update_ColorGradeCB(m_vColorFilter, m_fSaturation, m_fExposure, m_fContrast);
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Bloom"))
		{
			ImGui::SeparatorText("Edit Bloom Constant Buffer");
			ImGui::Text("BloomScale     : "); ImGui::SameLine();
			if (ImGui::SliderFloat("##SliderBloomScale", &m_fBloomScale, 0.f, 10.f))
			{
				m_pGameInstance->Update_BloomCB(m_fBloomScale, m_fBloomThreshold);
			}

			ImGui::Text("BloomThreshold : "); ImGui::SameLine();
			if (ImGui::SliderFloat("##SliderBloomThreshold", &m_fBloomThreshold, 0.f, 100.f))
			{
				m_pGameInstance->Update_BloomCB(m_fBloomScale, m_fBloomThreshold);
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

HRESULT CDebugTool::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	g_ImGuiWindowFlags |= ImGuiWindowFlags_NoMove;
	g_ImGuiWindowFlags |= ImGuiWindowFlags_NoResize;

	m_pRenderer = static_cast<CRenderer*>(m_pGameInstance->Find_Component_Prototype(LEVEL_STATIC, L"Prototype_Component_Renderer")->Clone(nullptr));
	if (m_pRenderer == nullptr)
		return E_FAIL;

    return S_OK;
}

HRESULT CDebugTool::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	CDebugTool::Main_Window();
	CDebugTool::ConsoleLog_Window();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

CDebugTool* CDebugTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugTool* pInstance = new CDebugTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CDebugTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebugTool::Free()
{
    __super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#endif