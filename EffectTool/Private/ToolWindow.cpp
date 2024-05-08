#include "../Default/framework.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "ToolWindow.h"
#include "GameInstance.h"

#pragma region IMGUI_COLOR

static bool alpha_preview = true;
static bool alpha_half_preview = false;
static bool drag_and_drop = true;
static bool options_menu = true;
static bool hdr = false;
static bool no_border = false;
ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
ImVec4 g_vColorBox;

#pragma endregion

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

CToolWindow::CToolWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	g_ImGuiWindowFlags |= ImGuiWindowFlags_NoMove;
	g_ImGuiWindowFlags |= ImGuiWindowFlags_NoResize;
}

HRESULT CToolWindow::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CToolWindow::Render_Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

#pragma region Show Window

	ImGui::Begin("Main", 0, g_ImGuiWindowFlags);

	if (ImGui::BeginTabBar("Tab"))
	{
		BEGIN_TAB("Particle")
			CToolWindow::Particle_Window();
		END_TAB

		BEGIN_TAB("Effect")
			CToolWindow::Effect_Window();
		END_TAB;

		ImGui::EndTabBar();
	}

	ImGui::End();

#pragma endregion

	ImGui::Render();
}

void CToolWindow::Render_End()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CToolWindow::Effect_Window()
{
}

void CToolWindow::Particle_Window()
{
	if (ImGui::BeginTabBar("Particle", 0))
	{
		if (ImGui::BeginTabItem("Particle"))
		{
			m_ParticleTags.clear();
			for (auto& iter : m_pGameInstance->Get_ParticleTags())
			{
				m_ParticleTags.push_back(ConvertWCToC(iter));
			}

			ImGui::SeparatorText("Select Particle");
			ImGui::Text("Particle : "); ImGui::SameLine();
			if (ImGui::ListBox("##SelectParticle", &m_iSelectParticleIndex,
				[](void* data, int index, const char** out_text) {
					vector<string>* Particles = static_cast<vector<string>*>(data);

					auto strFindTag = next(Particles->begin(), index);

					if (strFindTag != Particles->end())
					{
						*out_text = strFindTag->c_str();
						return true;
					}

					return false;
				},
				&m_ParticleTags, static_cast<int>(m_ParticleTags.size()), 4
			))
			{
				if (m_iSelectParticleIndex < m_ParticleTags.size())
					memcpy(m_szParticleTag, m_ParticleTags[m_iSelectParticleIndex].c_str(), sizeof(char[MAX_PATH]));

				m_tParticleInfo = m_pGameInstance->Get_ParticleInfo(ConvertCToWC(m_ParticleTags[m_iSelectParticleIndex]));
			}

			ImGui::NewLine();
			ImGui::SeparatorText("Modify Type");
			ImGui::Text("Name     : "); ImGui::SameLine();
			ImGui::InputText("##CreateParticleTag", m_szParticleTag, sizeof(m_szParticleTag), 8);

			ImGui::NewLine();
			if (ImGui::BeginTable("ModifyParticleData", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::Button("Create", { -1, 0 }))
				{
					if (FAILED(m_pGameInstance->Create_ParticlePrototype(ConvertCToWC(m_szParticleTag), m_tParticleInfo)))
						CONSOLE_ERROR("Failed to Create : Particle");
					else
						CONSOLE_INFO("Successed to Create : Particle");
				}

				ImGui::TableNextColumn();
				if (ImGui::Button("Delete", { -1, 0 }))
				{
					if (FAILED(m_pGameInstance->Delete_ParticlePrototype(ConvertCToWC(m_szParticleTag))))
						CONSOLE_ERROR("Failed to Delete : Particle");
					else
						CONSOLE_INFO("Successed to Delete : Particle");
				}

				ImGui::EndTable();
			}

			ImGui::NewLine();
			ImGui::SeparatorText("Action Type");
			if (ImGui::BeginTable("ActionParticleData", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::Button("Save", { -1, 0 }))
				{
					if (FAILED(m_pGameInstance->Save_Particles(L"../../Reference/Resources/Data/Effect/Particle.dat")))
						CONSOLE_ERROR("Failed to Save : Particle");
					else
						CONSOLE_INFO("Successed to Save : Particle");
				}

				ImGui::TableNextColumn();
				if (ImGui::Button("Load", { -1, 0 }))
				{
					if (FAILED(m_pGameInstance->Load_Particles(L"../../Reference/Resources/Data/Effect/Particle.dat")))
						CONSOLE_ERROR("Failed to Load : Particle");
					else
						CONSOLE_INFO("Successed to Load : Particle");
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Value"))
		{
			static string strMaxParticleCountText = "Max Particle Count [Limit : " + to_string(MAX_PARTICLE) + "]";
			ImGui::SeparatorText(strMaxParticleCountText.c_str());
			ImGui::Text("Count : "); ImGui::SameLine();
			ImGui::InputInt("##InputMaxParticleCount", &m_tParticleInfo.iMaxParticle, 1, 100, 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Is Frame Animation?");
			ImGui::Text("IsAnim: "); ImGui::SameLine();
			if (ImGui::BeginTable("IsFrameAnim", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("TRUE", m_tParticleInfo.bFrameAnimation == TRUE))
					m_tParticleInfo.bFrameAnimation = TRUE;

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("FALSE", m_tParticleInfo.bFrameAnimation == FALSE))
					m_tParticleInfo.bFrameAnimation = FALSE;

				ImGui::EndTable();
			}
			ImGui::NewLine();
			ImGui::SeparatorText("Does Particle Fade?");
			ImGui::Text("IsFade: "); ImGui::SameLine();
			if (ImGui::BeginTable("IsFrameAnim", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("TRUE", m_tParticleInfo.bFade == TRUE))
					m_tParticleInfo.bFade = TRUE;

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("FALSE", m_tParticleInfo.bFade == FALSE))
					m_tParticleInfo.bFade = FALSE;

				ImGui::EndTable();
			}


			ImGui::NewLine();
			ImGui::SeparatorText("Frame Anim Cycle Time");
			ImGui::Text("Time  : "); ImGui::SameLine();
			ImGui::InputFloat("##InputFrameAnimCc", (_float*)(&m_tParticleInfo.fFrameAnimCycle), 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Texture Max [Row, Col]");
			ImGui::Text("Max   : "); ImGui::SameLine();
			ImGui::InputInt2("##InputParticleMaxRowCol", (_int*)(&m_tParticleInfo.iNumParticleRowCol), 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Texture Curr [Row, Col]");
			ImGui::Text("Curr  : "); ImGui::SameLine();
			ImGui::InputInt2("##InputParticleCurrRowCol", (_int*)(&m_tParticleInfo.iCurrParticleRowCol), 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Type");
			if (ImGui::BeginTable("ParticleType", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("SPHERE_DIV", m_tParticleInfo.eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_DIV))
					m_tParticleInfo.eType = PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_DIV;

				ImGui::TableNextColumn();
				if (ImGui::RadioButton("SPHERE_CONV", m_tParticleInfo.eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_CONV))
					m_tParticleInfo.eType = PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_CONV;

				if (ImGui::RadioButton("Combined Transform", m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_COMBINED_TARGET)))
					m_tParticleInfo.iOption ^= 0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_COMBINED_TARGET;

				ImGui::EndTable();
			}

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Option");
			if (ImGui::BeginTable("ParticleType", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::RadioButton("Gravity", m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_GRAVITY)))
					m_tParticleInfo.iOption ^= 0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_GRAVITY;
				if (false != (m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_GRAVITY)))
				{
					ImGui::SameLine(); ImGui::Text(" Acc   : "); ImGui::SameLine();
					ImGui::InputFloat("##InputGarvityAcc", &m_tParticleInfo.fGravAcc, 0.f, 0.f, "%.3f", 8);
				}

				if (ImGui::RadioButton("Rotaion", m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ROTATION)))
					m_tParticleInfo.iOption ^= 0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ROTATION;
				if (false != (m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ROTATION)))
				{
					ImGui::SameLine();
					ImGui::InputFloat3("##InputRotation", (_float*)(& m_tParticleInfo.vRotationXYZ), "%.3f", 8);
				}

				if (ImGui::RadioButton("Accel", m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ACCEL)))
					m_tParticleInfo.iOption ^= 0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ACCEL;
				if (false != (m_tParticleInfo.iOption & (0x1 << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ACCEL)))
				{
					ImGui::SameLine();
					ImGui::InputFloat("##InputAccel", (&m_tParticleInfo.fAccel), 0.f, 0.f, "%.3f", 8);
				}


				ImGui::EndTable();
			}

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Texture Rotation In Degrees");
			ImGui::Text("Degree  : "); ImGui::SameLine();
			ImGui::InputFloat("##InputImageRotation", (&m_tParticleInfo.fAngle), 0.f, 0.f,"%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Position [x y z r]");
			ImGui::Text(""); ImGui::SameLine();
			ImGui::InputFloat4("##InputParticlePosition", (_float*)(&m_tParticleInfo.vParticlePosition), "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Direction [Min.x Min.y Min.z]");
			ImGui::Text("Min   : "); ImGui::SameLine();
			ImGui::InputFloat3("##InputParticleDirectionRectMin", (_float*)(&m_tParticleInfo.vDirectionMin), "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Direction [Max.x Max.y Max.z]");
			ImGui::Text("Max   : "); ImGui::SameLine();
			ImGui::InputFloat3("##InputParticleDirectionRectMax", (_float*)(&m_tParticleInfo.vDirectionMax), "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Size [Min - Max]");
			ImGui::Text("Size  : "); ImGui::SameLine();
			ImGui::InputFloat2("##InputParticleSize", (_float*)(&m_tParticleInfo.vParticleSize), "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Speed [Min - Max]");
			ImGui::Text("Speed : "); ImGui::SameLine();
			ImGui::InputFloat2("##InputParticleSpeed", (_float*)(&m_tParticleInfo.vParticleSpeed), "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle LifeTime [Min - Max]");
			ImGui::Text("Life  : "); ImGui::SameLine();
			ImGui::InputFloat2("##InputParticleLifeTime", (_float*)(&m_tParticleInfo.vParticleLifeTime), "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle EmitRate");
			ImGui::Text("Emit  : "); ImGui::SameLine();
			ImGui::InputInt("##InputParticleEmitRate", &m_tParticleInfo.iParticleEmitRate, 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle CreateTime");
			ImGui::Text("CTime : "); ImGui::SameLine();
			ImGui::InputFloat("##InputParticleCreateTime", &m_tParticleInfo.fParticleCreateTime, 0.f, 0.f, "%.3f", 8);

			ImGui::NewLine();
			ImGui::SeparatorText("Particle Correction");
			ImGui::Text("Vec   : "); ImGui::SameLine();
			ImGui::InputFloat4("##InputParticleCorrectionVector", (_float*)(&m_tParticleInfo.vCorrectionVector), "%.3f", 8);

			ImGui::NewLine();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Color"))
		{
			ImGui::SeparatorText("Random Color?"); ImGui::SameLine();
			ImGui::Checkbox("##IsRandomColor", &m_bRandColor);
			if (m_bRandColor == FALSE)
			{
				g_vColorBox = ImVec4(
					m_tParticleInfo.vParticleColor.m128_f32[0],
					m_tParticleInfo.vParticleColor.m128_f32[1],
					m_tParticleInfo.vParticleColor.m128_f32[2],
					m_tParticleInfo.vParticleColor.m128_f32[3]);
				ImGui::ColorButton("MyColor##3c", *(ImVec4*)&g_vColorBox, misc_flags | (no_border ? ImGuiColorEditFlags_NoBorder : 0), ImVec2(20, 20));

				ImGui::NewLine();
				ImGui::SeparatorText("Particle Color [RGB]");
				ImGui::Text("Color : "); ImGui::SameLine();
				if (ImGui::InputFloat3("##InputParticleColor", (_float*)(&m_tParticleInfo.vParticleColor), "%.3f", 8))
				{
					m_tParticleInfo.vParticleColor.m128_f32[0] = CLAMP(m_tParticleInfo.vParticleColor.m128_f32[0], 0, 1);
					m_tParticleInfo.vParticleColor.m128_f32[1] = CLAMP(m_tParticleInfo.vParticleColor.m128_f32[1], 0, 1);
					m_tParticleInfo.vParticleColor.m128_f32[2] = CLAMP(m_tParticleInfo.vParticleColor.m128_f32[2], 0, 1);
				}

				ImGui::NewLine();
				ImGui::SeparatorText("Particle Color [Alpha]");
				ImGui::Text("Color : "); ImGui::SameLine();
				/*m_tParticleDesc.vParticleColor.
				ImGui::SliderFloat("          ", &m_fColor[CR_A], 0.0f, 1.0f, "Alpha = %.3f");*/
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Play"))
		{
			ImGui::SeparatorText("Select Shader");
			ImGui::InputInt("##InputMaxParticleShader", &m_iSelectPlayShader);

			ImGui::NewLine();
			ImGui::SeparatorText("Select Texture");
			ImGui::InputInt("##InputMaxParticleTexture", &m_iSelectPlayTexture);

			ImGui::SeparatorText("Select Particle");
			ImGui::Text("Particle : "); ImGui::SameLine();
			ImGui::ListBox("##SelectParticle", &m_iSelectPlayParticleIndex,
				[](void* data, int index, const char** out_text) {
					vector<string>* Particles = static_cast<vector<string>*>(data);

					auto strFindTag = next(Particles->begin(), index);

					if (strFindTag != Particles->end())
					{
						*out_text = strFindTag->c_str();
						return true;
					}

					return false;
				},
				&m_ParticleTags, static_cast<int>(m_ParticleTags.size()), 4
			);

			ImGui::SeparatorText("Action Type");
			if (ImGui::BeginTable("ActionParticlePlay", 2))
			{
				ImGui::TableNextColumn();
				if (ImGui::Button("Play", { -1, 0 }))
				{
					if (m_ParticleTags.size() > m_iSelectPlayParticleIndex)
					{
						if (FAILED(m_pGameInstance->Play_Particle(
							ConvertCToWC(m_ParticleTags[m_iSelectPlayParticleIndex]),
							ConvertCToWC(m_ParticleTags[m_iSelectPlayParticleIndex]),
							m_iSelectPlayShader, m_iSelectPlayTexture)))
							CONSOLE_ERROR("Failed to Play : Particle");
						else
							CONSOLE_INFO("Successed to Play : Particle");
					}
				}

				ImGui::TableNextColumn();
				if (ImGui::Button("Stop", { -1, 0 }))
				{
					if (m_ParticleTags.size() > m_iSelectPlayParticleIndex)
					{
						if (FAILED(m_pGameInstance->Stop_Particle(ConvertCToWC(m_ParticleTags[m_iSelectPlayParticleIndex]))))
							CONSOLE_ERROR("Failed to Stop : Particle");
						else
							CONSOLE_INFO("Successed to Stop : Particle");
					}
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

CToolWindow* CToolWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolWindow* pInstance = new CToolWindow(pDevice, pContext);

	if (nullptr == pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
		MSG_BOX(L"Failed to Create : CToolWindow");
	}

	pInstance->Initialize();

	return pInstance;
}

void CToolWindow::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
