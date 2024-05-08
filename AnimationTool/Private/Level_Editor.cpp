#include "framework.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Level_Editor.h"

#include "Mannequin.h"
#include "Camera_Main.h"

#include "GameInstance.h"

#define CURRENT_LEVEL LEVEL_EDITOR

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

CLevel_Editor::CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Ready_Level()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_ParticleManager()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Mannequin(L"Layer_Mannequin")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Lights()
{
	LIGHT_DESC			LightDesc = {};

	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(CURRENT_LEVEL, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_ParticleManager()
{
	m_pGameInstance->Prepare_ParticleManager(
		static_cast<CTexture*>(m_pGameInstance->Clone_Component_Prototype(CURRENT_LEVEL, TEXT("Prototype_Component_Texture_Particle"))),
		static_cast<CRenderer*>(m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"))));

	if (FAILED(m_pGameInstance->Load_Particles(L"../../Reference/Resources/Data/Effect/Particle.dat")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Main::CAMERAMAIN_DESC tCameraMainDesc = {  };

	tCameraMainDesc.tCameraInfo.fMouseSensor = 0.1f;

	tCameraMainDesc.tCameraDesc.vEye = _float4(0.f, 2.5f, 2.5f, 1.f);
	tCameraMainDesc.tCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tCameraMainDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
	tCameraMainDesc.tCameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
	tCameraMainDesc.tCameraDesc.fNear = 0.2f;
	tCameraMainDesc.tCameraDesc.fFar = 1000.f;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Camera"), strLayerTag, &tCameraMainDesc)))
		return E_FAIL;

	m_pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(CURRENT_LEVEL, strLayerTag));
	Safe_AddRef(m_pCamera);

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Mannequin(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Mannequin"), strLayerTag)))
		return E_FAIL;

	m_pMannequin = static_cast<CMannequin*>(m_pGameInstance->Find_Object(CURRENT_LEVEL, strLayerTag));
	Safe_AddRef(m_pMannequin);
	
	return S_OK;
}

HRESULT CLevel_Editor::Clear_MaterialModel()
{
	for (auto& iter : m_MaterialModelTags)
	{
		wstring strLayerTag = L"Prototype_Component_Model_";
		strLayerTag += ConvertCToWC(iter.first);

		if (FAILED(m_pGameInstance->Release_Component_Prototype(CURRENT_LEVEL, strLayerTag)))
			return E_FAIL;
	}

	m_MaterialModelTags.clear();

	return S_OK;
}

HRESULT CLevel_Editor::Load_MaterialModel(const wstring& strModelFolderPath, const wstring& strFolderTag)
{
	HANDLE hFind;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = TRUE;

	_tchar	szDir[MAX_PATH] = TEXT("");
	_tchar	szEXT[MAX_PATH] = TEXT("");
	wstring strNewFilePath;
	wstring strFullPath;

	hFind = FindFirstFile(strModelFolderPath.c_str(), &tagFindData);

	if (hFind == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_wsplitpath_s(strModelFolderPath.c_str(), NULL, NULL, szDir, MAX_PATH, NULL, NULL, NULL, NULL);

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

				Load_MaterialModel(strNewFilePath, strFolderTag);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".dat")))
		{
			wstring strLayerTag = L"Prototype_Component_Model_";
			strLayerTag += strFolderTag;

			if (m_pGameInstance->Find_Component_Prototype(CURRENT_LEVEL, strLayerTag) == nullptr)
			{
				strFullPath = L"";
				strFullPath += szDir;
				strFullPath += tagFindData.cFileName;

				string strFilePath = ConvertWCToC(strFullPath.c_str());

				if (FAILED(m_pGameInstance->Add_Component_Prototype(CURRENT_LEVEL, strLayerTag,
					CModel::Create(m_pDevice, m_pContext, strFilePath))))
					return E_FAIL;

				wstring strEventTag = SplitBySeparator(tagFindData.cFileName, L".")[0];
				m_MaterialModelTags.emplace(ConvertWCToC(strEventTag), strLayerTag);
			}
		}

		bResult = FindNextFile(hFind, &tagFindData);
	}

	FindClose(hFind);

	return S_OK;
}

HRESULT CLevel_Editor::Append_SoundEvent()
{
	return S_OK;
}

HRESULT CLevel_Editor::Delete_SoundEvent()
{
	return S_OK;
}

void CLevel_Editor::Main_Window()
{
	ImGui::Begin("Main", 0, g_ImGuiWindowFlags);

	if (ImGui::BeginTabBar("Tab"))
	{
		BEGIN_TAB("Material")
			CLevel_Editor::MainWin_Material();
		END_TAB
		if (m_pMannequin->Find_Component(L"Com_Model") != nullptr)
		{
			BEGIN_TAB("Event")
				CLevel_Editor::MainWin_Event();
			END_TAB
		}
			ImGui::EndTabBar();
	}

	ImGui::End();
}

void CLevel_Editor::Sub_Window()
{
	ImGui::Begin("Sub", 0, g_ImGuiWindowFlags);

	if (ImGui::BeginTabBar("Tab"))
	{
		BEGIN_TAB("Camera")
			CLevel_Editor::SubWin_Camera();
		END_TAB
		if (m_pMannequin->Find_Component(L"Com_Model") != nullptr)
		{
			BEGIN_TAB("Animation")
				CLevel_Editor::SubWin_PlayAnim();
			END_TAB
		}
			ImGui::EndTabBar();
	}

	ImGui::End();
}

void CLevel_Editor::ConsoleLog_Window()
{
	ImGui::Begin("Console Log", 0, g_ImGuiWindowFlags);
	ImGui::End();

	g_ConsoleLog.Draw("Console Log");
}

void CLevel_Editor::MainWin_Material()
{
	if (ImGui::BeginTabBar("MaterialTabBar", 0))
	{
		CLevel_Editor::MaterialTab_Model();

		if (m_pMannequin->Find_Component(L"Com_Model") != nullptr)
		{
			CLevel_Editor::MaterialTab_Animation();
		}

		ImGui::EndTabBar();
	}
}

void CLevel_Editor::MaterialTab_Model()
{
	if (ImGui::BeginTabItem("Model"))
	{
		ImGui::SeparatorText("Model Path");
		ImGui::Text("Path   : "); ImGui::SameLine();
		ImGui::InputText("##MaterialModelPath", m_szMaterialModelPath, sizeof(m_szMaterialModelPath), 8);

		ImGui::NewLine();
		ImGui::SeparatorText("Select Model");
		ImGui::Text("Model  : "); ImGui::SameLine();
		if (ImGui::ListBox("##SelectModel", &m_iSelectMaterialModelIndex,
			[](void* data, int index, const char** out_text) {
				map<string, wstring>* mapModelTags = static_cast<map<string, wstring>*>(data);

				auto strFindTag = next(mapModelTags->begin(), index);

				if (strFindTag != mapModelTags->end())
				{
					*out_text = strFindTag->first.c_str();
					return true;
				}

				return false;
			},
			&m_MaterialModelTags, static_cast<int>(m_MaterialModelTags.size()), 4
		))
		{
			m_pMannequin->Bind_SpecificModel(CURRENT_LEVEL, next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->second);
		}

		ImGui::NewLine();
		if (ImGui::Button("Load Model", { -1, 0 }))
		{
			/* Clear */
			if (FAILED(CLevel_Editor::Clear_MaterialModel()))
			{
				CONSOLE_ERROR("Failed to Clear : Material Model");
			}
			else
			{
				CONSOLE_INFO("Successed to Clear : Material Model");

				/* Load */
				if (FAILED(Load_MaterialModel(ConvertCToWC(m_szMaterialModelPath))))
				{
					CONSOLE_ERROR("Failed to Load : Material Model");
				}
				else
				{
					CONSOLE_INFO("Successed to Load : Material Model");
				}
			}
		}

		ImGui::EndTabItem();
	}
}

void CLevel_Editor::MaterialTab_Animation()
{
	if (ImGui::BeginTabItem("Animation"))
	{
		m_fCurrAnimDuration = static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationCurrentDuration();
		m_fMaxAnimDuration = static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationDuration(m_pMannequin->Get_AnimationIndex());

		ImGui::SeparatorText("Select Animation");
		ImGui::Text("Anim     : "); ImGui::SameLine();

		_int iAnimIndex = m_pMannequin->Get_AnimationIndex();
		if (ImGui::ListBox("##SelectModel", &iAnimIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecAnimTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecAnimTags->begin(), index);

				if (strFindTag != vecAnimTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags(), 
			static_cast<int>(static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationCount()), 4
		))
		{
			m_pMannequin->Bind_Animation(iAnimIndex);
		}
		ImGui::Text("Anim Index : %d", iAnimIndex); //////////////////////////////////////////

		ImGui::NewLine();
		ImGui::SeparatorText("KeyFrame");
		ImGui::Text("KeyFrame : "); ImGui::SameLine();
		if (ImGui::SliderFloat("##SliderKeyFrame", &m_fCurrAnimDuration, 0.f, m_fMaxAnimDuration), "%.3f")
		{
			static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Set_AnimationCurrentDuration(m_fCurrAnimDuration);
		}

		ImGui::NewLine();
		ImGui::SeparatorText("Play Info");
		ImGui::Text("Speed    : "); ImGui::SameLine();
		if (ImGui::BeginTable("PlayAnimSpeed", 1))
		{
			_float fSpeed = m_pMannequin->Get_AnimationSpeed();
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
			if (ImGui::InputFloat("##InputAnimSpeed", &fSpeed, 0.f, 0.f, "%.3f", 8))
				m_pMannequin->Set_AnimationSpeed(fSpeed);

			ImGui::EndTable();
		}

		if (ImGui::BeginTable("PlayAction", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Play", { -1, 0 }))
			{
				m_pMannequin->Set_PlayAnimation(TRUE);
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Stop", { -1, 0 }))
			{
				m_pMannequin->Set_PlayAnimation(FALSE);
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void CLevel_Editor::MainWin_Event()
{
	if (ImGui::BeginTabBar("EventTabBar", 0))
	{
		CLevel_Editor::EventTab_Data();
		CLevel_Editor::EventTab_Sound();
		CLevel_Editor::EventTab_Effect();
		CLevel_Editor::EventTab_Particle();
		CLevel_Editor::EventTab_Function();
		CLevel_Editor::EventTab_Translation();

		ImGui::EndTabBar();
	}
}

void CLevel_Editor::EventTab_Data()
{
	if (ImGui::BeginTabItem("Data"))
	{
		ImGui::SeparatorText("Event Data Path");
		ImGui::Text("Path   : "); ImGui::SameLine();
		ImGui::InputText("##EventDataPath", m_szEventDataFilePath, sizeof(m_szEventDataFilePath), 8);

		ImGui::NewLine();
		ImGui::SeparatorText("Event Caller");
		ImGui::Text("Caller : "); ImGui::SameLine();
		if (ImGui::ListBox("##SelectEventCaller", &m_iSelectEventCallerIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecEventCallerTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecEventCallerTags->begin(), index);

				if (strFindTag != vecEventCallerTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&m_pGameInstance->Get_EventCallerTags(), static_cast<_int>(m_pGameInstance->Get_EventCallerTags().size()), 4
		))
		{
			// m_pMannequin->Bind_Animation(iAnimIndex);
		}

		ImGui::NewLine();
		ImGui::SeparatorText("Modify Caller");
		if (ImGui::BeginTable("ModifyCallerAction", 3))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Create", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Create_EventCaller(
					ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
					static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model")))))
				{
					CONSOLE_ERROR("Failed to Create : Event Caller");
				}
				else
				{
					CONSOLE_INFO("Successed to Create : Event Caller");
				}
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Delete", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Delete_EventCaller(ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first))))
				{
					CONSOLE_ERROR("Failed to Delete : Event Caller");
				}
				else
				{
					CONSOLE_INFO("Successed to Delete : Event Caller");
				}
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Invalidate", { -1, 0 }))
			{
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Invalidate_EventCaller();
				CONSOLE_INFO("Successed to Invalidate : Event Caller");
			}

			ImGui::EndTable();
		}

		ImGui::NewLine();
		ImGui::SeparatorText("Action");
		if (ImGui::BeginTable("EventDataAction", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Save", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Save_EventDataFile(m_szEventDataFilePath)))
				{
					CONSOLE_ERROR("Failed to Save : Event Data");
				}
				else
				{
					CONSOLE_INFO("Successed to Save : Event Data");
				}
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Load", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Load_EventDataFile(m_szEventDataFilePath)))
				{
					CONSOLE_ERROR("Failed to Load : Event Data");
				}
				else
				{
					CONSOLE_INFO("Successed to Load : Event Data");
				}
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void CLevel_Editor::EventTab_Sound()
{
	if (ImGui::BeginTabItem("Sound"))
	{
		m_fCurrAnimDuration = static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationCurrentDuration();

		ImGui::SeparatorText("Sound List");
		ImGui::Text("Sound    : "); ImGui::SameLine();
		ImGui::ListBox("##SelectSound", &m_iSelectSoundIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecAnimTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecAnimTags->begin(), index);

				if (strFindTag != vecAnimTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&m_pGameInstance->Get_SoundTags(), static_cast<_int>(m_pGameInstance->Get_SoundTags().size()), 4
		);

		if (ImGui::Button("Play Sound"))
		{
			m_pGameInstance->StopAll();
			m_pGameInstance->PlaySoundW(ConvertCToWC(m_pGameInstance->Get_SoundTags()[m_iSelectSoundIndex]));
		}

		ImGui::NewLine();
		ImGui::SeparatorText("Sound Event List");
		ImGui::Text("Event    : "); ImGui::SameLine();
		ImGui::ListBox("##SelectSoundEvent", &m_iSelectSoundEventIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecAnimTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecAnimTags->begin(), index);

				if (strFindTag != vecAnimTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&m_pGameInstance->Get_EventSounds(
				ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first), 
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()]),
			static_cast<_int>(m_pGameInstance->Get_EventSounds(
				ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()]).size()), 4);

		ImGui::NewLine();
		ImGui::SeparatorText("Event Condition");
		ImGui::Text("KeyFrame : "); ImGui::SameLine();
		if (ImGui::InputFloat("##EVConditionKeyFrame", &m_fCurrAnimDuration))
		{
			static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Set_AnimationCurrentDuration(m_fCurrAnimDuration);
		}

		ImGui::NewLine();
		ImGui::SeparatorText("Action");
		if (ImGui::BeginTable("EventDataAction", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Append", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Append_SoundEvent(
					ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
					static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()],
					ConvertCToWC(m_pGameInstance->Get_SoundTags()[m_iSelectSoundIndex]), m_fCurrAnimDuration
				)))
				{
					CONSOLE_ERROR("Failed to Append : Sound Event");
				}
				else
				{
					CONSOLE_INFO("Successed to Append : Sound Event");
				}
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Delete", { -1, 0 }))
			{
				 if (FAILED(m_pGameInstance->Delete_SoundEvent(ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
					 static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()],
					 m_iSelectSoundEventIndex)))
				 {
				 	CONSOLE_ERROR("Failed to Delete : Sound Data");
				 }
				 else
				 {
				 	CONSOLE_INFO("Successed to Delete : Sound Data");
				 }
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void CLevel_Editor::EventTab_Effect()
{
	if (ImGui::BeginTabItem("Effect"))
	{
		ImGui::SeparatorText("Model Path");
		ImGui::Text("Path   : "); ImGui::SameLine();
		ImGui::InputText("##MaterialModelPath", m_szMaterialModelPath, sizeof(m_szMaterialModelPath), 8);



		ImGui::EndTabItem();
	}
}

void CLevel_Editor::EventTab_Particle()
{
	if (ImGui::BeginTabItem("Particle"))
	{
		m_fCurrAnimDuration = static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationCurrentDuration();

		vector<string> ParticleTags;
		for (auto& iter : m_pGameInstance->Get_ParticleTags())
		{
			ParticleTags.push_back(ConvertWCToC(iter));
		}

		ImGui::SeparatorText("Shader Pass Number");
		ImGui::Text("Shader   : "); ImGui::SameLine();
		ImGui::InputInt("##SelectShaderPass", &m_iSelectShaderPass);

		ImGui::NewLine();
		ImGui::SeparatorText("Select Texture Index");
		ImGui::Text("Texture  : "); ImGui::SameLine();
		ImGui::InputInt("##SelectShaderPass", &m_iSelectTextureIndex);

		ImGui::NewLine();
		ImGui::SeparatorText("Particle List");
		ImGui::Text("Particle : "); ImGui::SameLine();
		ImGui::ListBox("##SelectParticle", &m_iSelectParticleIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecParticleTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecParticleTags->begin(), index);

				if (strFindTag != vecParticleTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			& ParticleTags, static_cast<_int>(ParticleTags.size()), 4
		);

		ImGui::NewLine();
		ImGui::SeparatorText("Bone List");
		ImGui::Text("Bone     : "); ImGui::SameLine();
		ImGui::ListBox("##SelectBone", &m_iSelectBoneIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecBoneTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecBoneTags->begin(), index);

				if (strFindTag != vecBoneTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_BoneTags(), static_cast<_int>(static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_BoneTags().size()), 4
		);

		ImGui::NewLine();
		ImGui::SeparatorText("Particle Event List");
		ImGui::Text("Event    : "); ImGui::SameLine();
		ImGui::ListBox("##SelectParticleEvent", &m_iSelectParticleEventIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecParticleEventTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecParticleEventTags->begin(), index);

				if (strFindTag != vecParticleEventTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&m_pGameInstance->Get_EventParticles(
				ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()]),
			static_cast<_int>(m_pGameInstance->Get_EventParticles(
				ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()]).size()), 4);

		ImGui::NewLine();
		ImGui::SeparatorText("Event Condition");
		ImGui::Text("KeyFrame : "); ImGui::SameLine();
		if (ImGui::InputFloat("##EVConditionKeyFrame", &m_fCurrAnimDuration))
		{
			static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Set_AnimationCurrentDuration(m_fCurrAnimDuration);
		}
		ImGui::Text("Duation  : "); ImGui::SameLine();
		ImGui::InputFloat("##EVConditionDuration", &m_fParticleEventDuration);

		ImGui::NewLine();
		ImGui::SeparatorText("Action");
		if (ImGui::BeginTable("EventDataAction", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Append", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Append_ParticleEvent(
					ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first), 
					static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()],
					m_iSelectShaderPass, m_iSelectTextureIndex,
					m_pGameInstance->Get_ParticleTags()[m_iSelectParticleIndex], 
					static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_BoneTags()[m_iSelectBoneIndex], m_fParticleEventDuration, m_fCurrAnimDuration
				)))
				{
					CONSOLE_ERROR("Failed to Append : Particle Event");
				}
				else
				{
					CONSOLE_INFO("Successed to Append : Particle Event");
				}
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Delete", { -1, 0 }))
			{
				// if (FAILED(m_pGameInstance->Load_EventDataFile(m_szEventDataFilePath)))
				// {
				// 	CONSOLE_ERROR("Failed to Load : Event Data");
				// }
				// else
				// {
				// 	CONSOLE_INFO("Successed to Load : Event Data");
				// }
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void CLevel_Editor::EventTab_Function()
{
	if (ImGui::BeginTabItem("Function"))
	{
		m_fCurrAnimDuration = static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationCurrentDuration();

		ImGui::NewLine();
		ImGui::SeparatorText("Function Event List");
		ImGui::Text("Event    : "); ImGui::SameLine();
		ImGui::ListBox("##SelectFunctionEvent", &m_iSelectFunctionEventIndex,
			[](void* data, int index, const char** out_text) {
				vector<string>* vecAnimTags = static_cast<vector<string>*>(data);

				auto strFindTag = next(vecAnimTags->begin(), index);

				if (strFindTag != vecAnimTags->end())
				{
					*out_text = strFindTag->c_str();
					return true;
				}

				return false;
			},
			&m_pGameInstance->Get_EventFunctions(
				ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()]),
			static_cast<_int>(m_pGameInstance->Get_EventFunctions(
				ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
				static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()]).size()), 4);

		ImGui::NewLine();
		ImGui::SeparatorText("Function Event Tag");
		ImGui::Text("Name   : "); ImGui::SameLine();
		ImGui::InputText("##FunctionTag", m_szFunctionTag, sizeof(m_szFunctionTag), 8);

		ImGui::SeparatorText("Event Condition");
		ImGui::Text("KeyFrame : "); ImGui::SameLine();
		if (ImGui::InputFloat("##EVConditionKeyFrame", &m_fCurrAnimDuration))
		{
			static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Set_AnimationCurrentDuration(m_fCurrAnimDuration);
		}

		ImGui::NewLine();
		ImGui::SeparatorText("Action");
		if (ImGui::BeginTable("EventDataAction", 2))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Append", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Append_FunctionEvent(
					ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
					static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()],
					ConvertCToWC(m_szFunctionTag) , m_fCurrAnimDuration
				)))
				{
					CONSOLE_ERROR("Failed to Append : Function Event");
				}
				else
				{
					CONSOLE_INFO("Successed to Append : Function Event");
				}
			}

			ImGui::TableNextColumn();
			if (ImGui::Button("Delete", { -1, 0 }))
			{
				if (FAILED(m_pGameInstance->Delete_FunctionEvent(
					ConvertCToWC(next(m_MaterialModelTags.begin(), m_iSelectMaterialModelIndex)->first),
					static_cast<CModel*>(m_pMannequin->Find_Component(L"Com_Model"))->Get_AnimationTags()[m_pMannequin->Get_AnimationIndex()],
					m_iSelectFunctionEventIndex
				)))
				{
					CONSOLE_ERROR("Failed to Delete : Function Event");
				}
				else
				{
					CONSOLE_INFO("Successed to Delete : Function Event");
				}
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void CLevel_Editor::EventTab_Translation()
{
	if (ImGui::BeginTabItem("Translation"))
	{
		ImGui::SeparatorText("Model Path");
		ImGui::Text("Path   : "); ImGui::SameLine();
		ImGui::InputText("##MaterialModelPath", m_szMaterialModelPath, sizeof(m_szMaterialModelPath), 8);



		ImGui::EndTabItem();
	}
}

void CLevel_Editor::SubWin_Camera()
{
	ImGui::SeparatorText("Camera Move");
	if (ImGui::BeginTable("CameraMove", 2))
	{
		ImGui::TableNextColumn();
		if (ImGui::RadioButton("TRUE", m_bCameraMove == TRUE))
			m_bCameraMove = TRUE;

		ImGui::TableNextColumn();
		if (ImGui::RadioButton("FALSE", m_bCameraMove == FALSE))
			m_bCameraMove = FALSE;

		ImGui::EndTable();
	}
}

void CLevel_Editor::SubWin_PlayAnim()
{
	ImGui::SeparatorText("Play Info");
	ImGui::Text("Speed    : "); ImGui::SameLine();
	if (ImGui::BeginTable("PlayAnimSpeed", 1))
	{
		_float fSpeed = m_pMannequin->Get_AnimationSpeed();
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
		if (ImGui::InputFloat("##InputAnimSpeed", &fSpeed, 0.f, 0.f, "%.3f", 8))
			m_pMannequin->Set_AnimationSpeed(fSpeed);

		ImGui::EndTable();
	}

	if (ImGui::BeginTable("PlayAction", 2))
	{
		ImGui::TableNextColumn();
		if (ImGui::Button("Play", { -1, 0 }))
		{
			m_pMannequin->Set_PlayAnimation(TRUE);
		}

		ImGui::TableNextColumn();
		if (ImGui::Button("Stop", { -1, 0 }))
		{
			m_pMannequin->Set_PlayAnimation(FALSE);
		}

		ImGui::EndTable();
	}

}

HRESULT CLevel_Editor::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	g_ImGuiWindowFlags |= ImGuiWindowFlags_NoMove;
	//g_ImGuiWindowFlags |= ImGuiWindowFlags_NoResize;

	return S_OK;
}

HRESULT CLevel_Editor::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	if (m_bCameraMove == TRUE)
		m_pCamera->Camera_Move(fTimeDelta);

	return S_OK;
}

HRESULT CLevel_Editor::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	CLevel_Editor::Main_Window();
	CLevel_Editor::Sub_Window();
	CLevel_Editor::ConsoleLog_Window();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

CLevel_Editor* CLevel_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Editor* pLevel = new CLevel_Editor(pDevice, pContext);

	if (FAILED(pLevel->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Editor"));
		Safe_Release(pLevel);
	}

	return pLevel;
}

void CLevel_Editor::Free()
{
	__super::Free();

	Safe_Release(m_pCamera);
	Safe_Release(m_pMannequin);
}
