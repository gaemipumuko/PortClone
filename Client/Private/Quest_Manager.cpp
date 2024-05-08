#include "stdafx.h"
#include "Quest_Manager.h"
#include "UI_Manager.h"
#include "Page_Main.h"

#include "GameInstance.h"




CQuest_Manager::CQuest_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice,pContext)
{
}

CQuest_Manager::CQuest_Manager(const CQuest_Manager& rhs)
    :CGameObject(rhs)
{
}


void CQuest_Manager::Add_Quest(CQuest* pQuest)
{
    m_vecQuests.push_back(pQuest);
}

CQuest* CQuest_Manager::Check_Duplicates(wstring strQuestName)
{
    for (auto iter = m_vecQuests.begin(); iter != m_vecQuests.end();)
    {
        if ((*iter)->Get_QuestName() == strQuestName)
            return (*iter);
        ++iter;
    }
    for (auto iter = m_vecCompletedQuests.begin(); iter != m_vecCompletedQuests.end();)
    {
        if ((*iter)->Get_QuestName() == strQuestName)
            return (*iter);
        ++iter;
    }

    return nullptr;
}

HRESULT CQuest_Manager::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CQuest_Manager::Initialize_Clone(void* pArg)
{

    m_pUI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);

    return S_OK;
}

HRESULT CQuest_Manager::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CQuest_Manager::Tick(_float fTimeDelta)
{

    for (auto iter = m_vecQuests.begin(); iter != m_vecQuests.end();)
    {

        if ((*iter)->Get_QuestState() == CQuest::QUEST_COMPLETED)
        {
            CPage_Main* pPage = (CPage_Main*)m_pUI_Manager->Get_UI_Page(PAGE_MAIN);
            pPage->Request_Top_Overlay({ 0.87058823529f,0.7294117647f ,1.f, -0.5f }, TEXT("임무 완료"));

            m_vecCompletedQuests.push_back(*iter);
            iter = m_vecQuests.erase(iter);
        }
        else if ((*iter)->Get_QuestState() == CQuest::QUEST_NOT_ACCEPTED)
        {
            ++iter;
        }
        else if ((*iter)->Get_QuestState() == CQuest::QUEST_ACCEPTED)
        {
            CPage_Main* pPage = (CPage_Main*)m_pUI_Manager->Get_UI_Page(PAGE_MAIN);
            pPage->Register_On_QuestHUD(*iter);
            (*iter)->Set_QuestState(CQuest::QUEST_IN_PROGRESS);
        }
        else
        {
            (*iter)->Update(fTimeDelta);
            ++iter;
        }
    }

    return S_OK;
}

HRESULT CQuest_Manager::LateTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CQuest_Manager::Render()
{
    return S_OK;
}

CQuest_Manager* CQuest_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CQuest_Manager* pInstance = new CQuest_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CQuest_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CQuest_Manager* CQuest_Manager::Clone(void* pArg)
{
    CQuest_Manager* pInstance = new CQuest_Manager(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CQuest_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CQuest_Manager::Free()
{
    __super::Free();

       for (auto iter : m_vecQuests)
       {
           Safe_Release(iter);
       }
       m_vecQuests.clear();
   
       for (auto iter : m_vecCompletedQuests)
       {
           Safe_Release(iter);
       }
       m_vecCompletedQuests.clear();

}
