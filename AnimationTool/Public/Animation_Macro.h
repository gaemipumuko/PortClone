#pragma once

#define BEGIN_TAB(strTag)	if(ImGui::BeginTabItem(strTag)) {
#define END_TAB				ImGui::EndTabItem(); }

#define CONSOLE_INFO(message)							\
g_ConsoleLog.AddLog("[%05d] [Info] %s \n",				\
ImGui::GetFrameCount(), message)

#define CONSOLE_ERROR(message)							\
g_ConsoleLog.AddLog("[%05d] [Error] %s \n",				\
ImGui::GetFrameCount(), message)