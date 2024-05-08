#include "stdafx.h"
#include "Ambor.h"
#include "Part_Body.h"

#include "GameInstance.h"
#include "InteractionBox.h"
#include "Player.h"
#include "Inventory.h"
#include "UI_Manager.h"

CAmbor::CAmbor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CAmbor::CAmbor(const CAmbor& rhs)
	: CCreature(rhs)
{
}

HRESULT CAmbor::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = NPC_AMBOR;
	tBodyDesc.eType = CTY_NPC;

	CREATUREID eCreatureID = NPC_AMBOR;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CAmbor::Ready_Script()
{
	wstring strNpcScript;
	wstring strNpcChoice;
	CDialogue_Page::BREAK_DESC tbreak_Desc = {};
	//
	strNpcScript = TEXT("여긴 바람과 민들레가 머무는 목가의 성, 자유의 도시 몬드야.");
	m_NPCScript1.push_back(strNpcScript);
	strNpcScript = TEXT("페보니우스 기사단의 비호를 받는 여행자여, 몬드성에 온 걸 환영해!");
	m_NPCScript1.push_back(strNpcScript);
	//Paimon
	strNpcScript = TEXT("이제 밖에서 노숙하지 않아도 되겠다");
	m_NPCScript1.push_back(strNpcScript);
	strNpcScript = TEXT("근데 성에 있는 사람들을 보니까... 다들 기운이 없어 보여");
	m_NPCScript1.push_back(strNpcScript);
	//
	strNpcScript = TEXT("최근 도시전체가 풍마룡 때문에 고민이라 그래");
	m_NPCScript1.push_back(strNpcScript);
	strNpcScript = TEXT("하지만 진 단장만 있으면 뭐든지 해결할 수 있으니까 괜찮아!");
	m_NPCScript1.push_back(strNpcScript);
	//Paimon
	strNpcScript = TEXT("진 단장?");
	m_NPCScript1.push_back(strNpcScript);
    //
	strNpcScript = TEXT("페보니우스 기사단의 단장 대행이자 몬드성의 수호자야");
	m_NPCScript1.push_back(strNpcScript);
	strNpcScript = TEXT("진과 함께라면 풍마룡 같이 무시무시한 적도 무찌를 수 있을 거야"); //8
	m_NPCScript1.push_back(strNpcScript);

	//BreakPt 0
	strNpcChoice = TEXT("(엄청 대단한 인물인가 보네...)");  // 0
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("(그녀가 바람의 신에 대해 알고 있으면...)");
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 2; //선택지 수
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 0,0 }; //다음 선택지 인덱스
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 9,9 };	 //이어갈 대사 위치
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 10,10 }; //종료 위치
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 0,0 }; //다음 선택지 인덱스
	m_BreakPoints.push_back(tbreak_Desc);


	strNpcScript = TEXT("기사단 본부에 가기 전에 너에게 줄 선물이 있어");
	m_NPCScript1.push_back(strNpcScript);
	strNpcScript = TEXT("이제 날 따라와, 높은 곳으로 가야 해");
	m_NPCScript1.push_back(strNpcScript);

//  (몬드성 벤티 석상 앞)

	strNpcScript = TEXT("자, 아까 말한 선물은 바로---");
	m_NPCScript2.push_back(strNpcScript);
	strNpcScript = TEXT("바람의 날개!");
	m_NPCScript2.push_back(strNpcScript);
	strNpcScript = TEXT("정찰 기사들은 이걸 이용해 공중에서 이동해. 몬드의 시민들도 즐겨 사용하지!");
	m_NPCScript2.push_back(strNpcScript);
	strNpcScript = TEXT("근데 너 아직 비행 허가증 없지");
	m_NPCScript2.push_back(strNpcScript);
	//Paimon
	strNpcScript = TEXT("비행 허가증? 그게 뭐야?");
	m_NPCScript2.push_back(strNpcScript);
    //
	strNpcScript = TEXT("이름만 봐도 알 수 있잖아? 몬드성에서 합법적으로 비행하려면 비행 허가증이 있어야 해");
	m_NPCScript2.push_back(strNpcScript);
	strNpcScript = TEXT("비행 허가증은 시험에 합격해야 기사단에서 발급해주는 거야");
	m_NPCScript2.push_back(strNpcScript);
	strNpcScript = TEXT(" 이거 받아. 이 비행 가이드는 외울 것 까진 없지만, 숙지해두는 게 좋아");
	m_NPCScript2.push_back(strNpcScript);
	strNpcScript = TEXT("다 읽고 나면 바람이 시작되는 곳으로 날 찾으러와"); //8
	m_NPCScript2.push_back(strNpcScript);

//(몬드성 성당 앞)

	//TEST_START

	strNpcScript = TEXT("어이-----여기야, 여기");
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("다 설치했으니 시험을 이어가 보자");
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("표식을 지나 결승점까지 한번에 날아가면 돼");
	m_NPCScript.push_back(strNpcScript);

	//Break Pt.1
	strNpcChoice = TEXT("난 준비됐어"); //2
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 1; //선택지 수
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 0 }; //다음 선택지 인덱스
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 2 };	 //이어갈 대사 위치
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 2 }; //종료 위치
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 2 }; //다음 선택지 인덱스
	m_BreakPoints.push_back(tbreak_Desc);

	//RETRY

	strNpcScript = TEXT("괜찮아!? 안 다쳤어?");
	m_NPCTryAgainScript.push_back(strNpcScript);
	strNpcScript = TEXT("잘 준비해서 다시 한번 해 보자");
	m_NPCTryAgainScript.push_back(strNpcScript);
	strNpcScript = TEXT("바람의 날개를 잘 조절해서 바람을 타고 가는 거야");
	m_NPCTryAgainScript.push_back(strNpcScript);

	//Break Pt.2
	strNpcChoice = TEXT("한 번 더 하게 해줘"); //3
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("다시 생각 좀 해 볼게"); //4
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 2; //선택지 수
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 2,2 }; //다음 선택지 인덱스
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 2,3 };	 //이어갈 대사 위치
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 2,3 }; //종료 위치
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 3,3 }; //다음 선택지 텍스트 인덱스
	m_BreakPoints.push_back(tbreak_Desc);

	strNpcScript = TEXT("그래, 다시 생각하고 말해줘");
	m_NPCTryAgainScript.push_back(strNpcScript);

	//END

	strNpcScript = TEXT("바람의 날개로 비행하는 요령을 터득한 거 같네.");
	m_NPCClearedScript.push_back(strNpcScript);
	strNpcScript = TEXT("축하해! 이게 바로 비행 허가증이야, 이제 몬드성에서 합법적으로 비행할 수 있어");
	m_NPCClearedScript.push_back(strNpcScript);
	strNpcScript = TEXT("뭐, 원래도 잘했으니까, 순조로울 거라 생각했어");
	m_NPCClearedScript.push_back(strNpcScript);
	strNpcScript = TEXT("몬드성 비행 규칙은 꼭 지켜, 허가증 취소당하지 않게...");
	m_NPCClearedScript.push_back(strNpcScript);
	strNpcScript = TEXT("비행 시험 합격한 걸 축하해!");
	m_NPCClearedScript.push_back(strNpcScript);

	return S_OK;
}

void CAmbor::Begin_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	pUIManager->Show_Page(PAGE_DIALOGUE);
	CDialogue_Page* pDialogue_Page = (CDialogue_Page*)pUIManager->Get_CurrentPage();

	switch (m_eDialogue_Stage)
	{
	case Client::CAmbor::DIALOGUE_INITIAL:
		m_NPCScriptDisplayed = m_NPCScript1;
		pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
		pDialogue_Page->Set_Target(L"엠버",L"<정찰 기사>");
		m_iCurrentBreakPt = 0;
		m_iBreakPointPg = 8;
		m_iTerminatorPg = 10;
		m_iParagraphIndex = 0;
		m_iNumChoice = 2;
		m_iCurrentChoiceIdx = 0;
		break;
	case Client::CAmbor::DIALOGUE_PLAZA:
		m_NPCScriptDisplayed = m_NPCScript2;
		pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
		pDialogue_Page->Set_Target(L"엠버", L"<정찰 기사>");
		m_iCurrentBreakPt = 0;
		m_iBreakPointPg = -1; //Negative Represents No Multiple Choices
		m_iTerminatorPg = 8;
		m_iParagraphIndex = 0;
		m_iNumChoice = 0;
		m_iCurrentChoiceIdx = 0;
		break;
	case Client::CAmbor::DIALOGUE_TEST:
		m_NPCScriptDisplayed = m_NPCScript;
		pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
		pDialogue_Page->Set_Target(L"엠버", L"<정찰 기사>");
		m_iCurrentBreakPt = 1;
		m_iBreakPointPg = 2;
		m_iTerminatorPg = 3;
		m_iParagraphIndex = 0;
		m_iNumChoice = 1;
		m_iCurrentChoiceIdx = 2;
		break;
	case Client::CAmbor::DIALOGUE_RETRY:
		m_NPCScriptDisplayed = m_NPCTryAgainScript;
		pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
		pDialogue_Page->Set_Target(L"엠버", L"<정찰 기사>");
		m_iCurrentBreakPt = 2;
		m_iBreakPointPg = 2;
		m_iTerminatorPg = 3;
		m_iParagraphIndex = 0;
		m_iNumChoice = 2;
		m_iCurrentChoiceIdx = 3;
		break;
	case Client::CAmbor::DIALOGUE_PASSED:
		m_NPCScriptDisplayed = m_NPCClearedScript;
		pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
		pDialogue_Page->Set_Target(L"엠버", L"<정찰 기사>");
		m_iCurrentBreakPt = 0;
		m_iBreakPointPg = -1;
		m_iTerminatorPg = 4;
		m_iParagraphIndex = 0;
		m_iNumChoice = 0;
		m_iCurrentChoiceIdx = 0;
		break;
	case Client::CAmbor::DIALOGUE_END:
		break;
	default:
		break;
	}

	//if (m_bDialogueEnd == true)
	//{
	//	m_NPCScriptDisplayed = m_NPCClearedScript;
	//	pDialogue_Page->Set_Script(m_NPCScriptDisplayed[4]);
	//	pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
	//	m_iTerminatorPg = 4;
	//	m_iParagraphIndex = 4;
	//}
	//else if (m_bTestClear == true)
	//{
	//	m_NPCScriptDisplayed = m_NPCClearedScript;
	//	pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
	//	pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
	//	m_iTerminatorPg = 3;
	//	m_iParagraphIndex = 0;
	//	m_bDialogueEnd = true;
	//}
	//else
	//{
	//	//Dialogue 1
	//	m_NPCScriptDisplayed = m_NPCScript;
	//	pDialogue_Page->Set_Script(m_NPCScriptDisplayed[0]);
	//	pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
	//	m_iTerminatorPg = 2;
	//	m_iParagraphIndex = 0;
	//}
}

void CAmbor::Execute_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	pUIManager->Show_Page(PAGE_DIALOGUE);
	CDialogue_Page* pDialogue_Page = (CDialogue_Page*)pUIManager->Get_CurrentPage();

	//vector<CDialogue_Page::BREAK_DESC> vec_BreakPts;
	//vec_BreakPts = m_BreakPoints;

	if (m_iParagraphIndex == m_iBreakPointPg)
	{
		/*선택지 세팅시 필요한것들.
		_int iNumBoxes;
		_int* pNextBreakIdx;
		_int* pNexTerminatorIdx;
		_int* pScriptFlow;
		_int* pNextBreak;
		*/
		if (m_pInteractionBox->Get_NumBoxes() == 0)
		{
			for (_int iIndex = m_iCurrentChoiceIdx; iIndex < m_iCurrentChoiceIdx + m_iNumChoice; iIndex++)
			{
				m_pInteractionBox->Add_InteractionBox(m_NPCChoice[iIndex], CInteraction_Box::ICON_TALK);
			}
		}

		if (m_pGameInstance->Key_Down('F'))
		{
			//선택지 
			_int fSelect;
			fSelect = m_pInteractionBox->Get_SelectedIndex();
			m_pInteractionBox->Clear_Boxes();
			//선택지 인덱스 갱신
			m_iCurrentChoiceIdx = m_BreakPoints[m_iCurrentBreakPt].pNextChoiceIdx[fSelect];
			//스크립트 흐름 설정
			m_iParagraphIndex = m_BreakPoints[m_iCurrentBreakPt].pScriptFlow[fSelect];
			//다음 대화 종료 시점 설정
			m_iTerminatorPg = m_BreakPoints[m_iCurrentBreakPt].pNexTerminatorIdx[fSelect];
			//다음 선택지 발생 시점 설정
			m_iBreakPointPg = m_BreakPoints[m_iCurrentBreakPt].pNextBreak[fSelect];
			//다음 사용할 선택지 인덱스 설정
			m_iCurrentBreakPt = m_BreakPoints[m_iCurrentBreakPt].pNextBreakIdx[fSelect];
			//다음 선택지의 선택지 갯수
			m_iNumChoice = m_BreakPoints[m_iCurrentBreakPt].iNumBoxes;

			pDialogue_Page->Set_Script(m_NPCScriptDisplayed[m_iParagraphIndex]);
		}

		if (m_iParagraphIndex == m_iTerminatorPg)
		{
			Next_Dialogue();
			End_Dialogue();
		}
	}
	else if(m_pGliding_Test == nullptr)
	{
		if (m_pGameInstance->Key_Down('F'))
		{
			if (m_iParagraphIndex == m_NPCScriptDisplayed.size() - 1 || m_iParagraphIndex == m_iTerminatorPg)
			{
				Next_Dialogue();
				End_Dialogue();
			}
			else
			{
				//페이지 넘기기
				m_iParagraphIndex++;
				pDialogue_Page->Set_Target(L"엠버", L"<정찰 기사>");

				if (m_eDialogue_Stage == DIALOGUE_INITIAL)
				{
					if (m_iParagraphIndex == 2 || m_iParagraphIndex == 3 || m_iParagraphIndex == 6)
					{
						pDialogue_Page->Set_Target(L"페이몬",L"");
					}
				}
				if (m_eDialogue_Stage == DIALOGUE_PLAZA)
				{
					if (m_iParagraphIndex == 4)
					{
						pDialogue_Page->Set_Target(L"페이몬", L"");
					}
				}
				pDialogue_Page->Set_Script(m_NPCScriptDisplayed[m_iParagraphIndex]);
			}

		}
	}
}

void CAmbor::End_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	pUIManager->Show_Page(PAGE_MAIN);
	m_pInteractionBox->Add_InteractionBox(TEXT("엠버"), CInteraction_Box::ICON_TALK);
	m_bConversation = false;
	m_bLookPlayer = FALSE;
	m_iCameraScene = 0;

	if (m_bDialogueEnd == true)
	{

	}
	else
	{
		if (m_bTestClear == FALSE && 
			(m_pGliding_Test == nullptr || m_pGliding_Test->Get_TryAgain() == TRUE) &&
			(m_eDialogue_Stage == DIALOGUE_TEST || m_eDialogue_Stage == DIALOGUE_RETRY))
		{
			//Safe_Release(m_pGliding_Test);

			//CMini_Game::MINIGAME_DESC MiniGame;

			//MiniGame.strMiniGameName = L"Gliding_Test";
			//MiniGame.fTotalTime = 60.f;

			//m_pGliding_Test = CGliding_Test::Create(&MiniGame);

			//if (m_pGliding_Test->Get_TryAgain() == TRUE) /* 재도전 */
			//	m_pGliding_Test->Set_TryAgain(FALSE);
		}
		else if (m_pGliding_Test != nullptr && m_pGliding_Test->Get_ClearGame() == TRUE) /* 비행시험 Clear 후 */
		{

			Safe_Release(m_pGliding_Test);
		}
	}
}

void CAmbor::Next_Dialogue()
{
	switch (m_eDialogue_Stage)
	{
	case Client::CAmbor::DIALOGUE_INITIAL:
		m_eDialogue_Stage = DIALOGUE_PLAZA;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AmborInfo[m_eDialogue_Stage].vAmborPos);
		m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(AmborInfo[m_eDialogue_Stage].fRadian));
		m_bRotation = TRUE;
		break;
	case Client::CAmbor::DIALOGUE_PLAZA:
		m_eDialogue_Stage = DIALOGUE_TEST;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, AmborInfo[m_eDialogue_Stage].vAmborPos);
		m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(AmborInfo[m_eDialogue_Stage].fRadian));
		m_bRotation = TRUE;
		break;
	case Client::CAmbor::DIALOGUE_TEST:
	{
		if (m_pGliding_Test == nullptr)
		{
			m_bRotation = TRUE;
			Safe_Release(m_pGliding_Test);

			CMini_Game::MINIGAME_DESC MiniGame;

			MiniGame.strMiniGameName = L"Gliding_Test";
			MiniGame.fTotalTime = 60.f;

			m_pGliding_Test = CGliding_Test::Create(&MiniGame);

			if (m_pGliding_Test->Get_TryAgain() == TRUE) /* 재도전 */
				m_pGliding_Test->Set_TryAgain(FALSE);
		}
	}
		break;
	case Client::CAmbor::DIALOGUE_RETRY:
	{
		if (m_pGliding_Test == nullptr)
		{
			m_bRotation = TRUE;
			Safe_Release(m_pGliding_Test);

			CMini_Game::MINIGAME_DESC MiniGame;

			MiniGame.strMiniGameName = L"Gliding_Test";
			MiniGame.fTotalTime = 60.f;

			m_pGliding_Test = CGliding_Test::Create(&MiniGame);

			if (m_pGliding_Test->Get_TryAgain() == TRUE) /* 재도전 */
				m_pGliding_Test->Set_TryAgain(FALSE);
		}
	}
		break;
	case Client::CAmbor::DIALOGUE_PASSED:
		//m_eDialogue_Stage = DIALOGUE_INITIAL;
		break;
	case Client::CAmbor::DIALOGUE_END:
		break;
	default:
		break;
	}
}

HRESULT CAmbor::Set_CameraScene()
{
	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	switch (m_eDialogue_Stage)
	{
	case DIALOGUE_INITIAL:
		if (FAILED(CAmbor::Init_DialogueScene()))
			return E_FAIL;
	break;
	case DIALOGUE_PLAZA:
		if (FAILED(CAmbor::Plaza_DialogueScene()))
			return E_FAIL;
		break; 
	case DIALOGUE_TEST:
		if (FAILED(CAmbor::Test_DialogueScene()))
			return E_FAIL;
		break;
	case DIALOGUE_RETRY:
		if (FAILED(CAmbor::Retry_DialogueScene()))
			return E_FAIL;
		break;
	case DIALOGUE_PASSED:
		if (FAILED(CAmbor::Passed_DialogueScene()))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CAmbor::Init_DialogueScene()
{
	if (m_iParagraphIndex == 0)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK3);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_ONE, 0.5f, 0.3f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_0.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 1)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_CLAP);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_TWO, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_1.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 2)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			CFiniteStateMachine* pPaimonFSM =
				static_cast<CFiniteStateMachine*>(m_pGameInstance
					->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Paimon", L"Com_FiniteStateMachine"));
			if (pPaimonFSM == nullptr)
				return E_FAIL;

			pPaimonFSM->Change_State(FSM_PAIMON_CLAP);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_THREE, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_2.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 3)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			//CTransform* pPaimonTransform = 
			CFiniteStateMachine* pPaimonFSM =
				static_cast<CFiniteStateMachine*>(m_pGameInstance
					->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Paimon", L"Com_FiniteStateMachine"));
			if (pPaimonFSM == nullptr)
				return E_FAIL;

			pPaimonFSM->Change_State(FSM_PAIMON_GUIDE);
		}

		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_3.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 4)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK1);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_FOUR, 0.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_4.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 5)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK2);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_FIVE, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_5.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 6)
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_SIX, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_6.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 7)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK2);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_FOUR, 0.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_7.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 8)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK3);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_TWO, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_8.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
		/*else if (m_iCameraScene == m_iParagraphIndex + 1 && m_pGameInstance->Key_Down('F'))
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_10.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}*/
	}
	else if (m_iParagraphIndex == 9)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK1);
		}

		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_10.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 10)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript1_11.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}

	return S_OK;
}

HRESULT CAmbor::Plaza_DialogueScene()
{
	if (m_iParagraphIndex == 0)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK3);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_FOUR, 0.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_0.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 1)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_CLAP);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_TWO, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_1.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 2)
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_FIVE, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_2.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 3)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK1);
		}

		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_3.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 4)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			CFiniteStateMachine* pPaimonFSM =
				static_cast<CFiniteStateMachine*>(m_pGameInstance
					->Find_Layer_Component(m_pGameInstance->Get_CurrentLevel(), L"Layer_Paimon", L"Com_FiniteStateMachine"));
			if (pPaimonFSM == nullptr)
				return E_FAIL;

			pPaimonFSM->Change_State(FSM_PAIMON_IDLE3);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_THREE, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_4.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 5)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK3);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_TWO, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_5.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 6)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK1);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_SEVEN, 0.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_6.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 7)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_7.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 8)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript2_8.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}

	return S_OK;
}

HRESULT CAmbor::Test_DialogueScene()
{
	if (m_iParagraphIndex == 0)
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_ONE, 0.f, 10.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript_0.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 1)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript_1.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 2)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCScript_2.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	
	return S_OK;
}

HRESULT CAmbor::Retry_DialogueScene()
{
	if (m_iParagraphIndex == 0)
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_ONE, 0.f, 10.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCTryAgainScript_0.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 1)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCTryAgainScript_1.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 2)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCTryAgainScript_3.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}

	return S_OK;
}

HRESULT CAmbor::Passed_DialogueScene()
{
	if (m_iParagraphIndex == 0)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK2);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_SEVEN, 0.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCClearedScript_0.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 1)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_CLAP);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_TWO, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCClearedScript_1.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 2)
	{
		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_TALK1);
		}

		if (m_iCameraScene == m_iParagraphIndex)
		{
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCClearedScript_2.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}
	}
	else if (m_iParagraphIndex == 3)
	{
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == m_iParagraphIndex)
		{
			pCamera->Set_SceneMode(CAMSCENEID_AMBORTALK, SCENE_FIVE, 1.f, 0.f);
			m_iCameraScene++;
			m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
			m_pGameInstance->PlaySoundW(L"NPCClearedScript_3.mp3", 1.f, CHANNELID::SOUND_VOICE);
		}

		if (m_bItem == FALSE)
		{
			CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory"));
			pInven->Add_Item(CItem::Create_Item(129, 1));
			m_bItem = TRUE;
		}
	}
	

	return S_OK;
}

HRESULT CAmbor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAmbor::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Script()))
		return E_FAIL;

	POS_DESC AmborPos = {};
	AmborPos.vAmborPos = { 431.9f, 10.1f, 253.1f, 1.f };
	AmborPos.fRadian = 0.f;
	AmborInfo.push_back(AmborPos); /* 몬드성 정문 앞 */

	AmborPos.vAmborPos = { 246.16f, 50.1f, 257.2f, 1.f };
	AmborPos.fRadian = 90.f;
	AmborInfo.push_back(AmborPos); /* 벤티 석상 앞 */

	AmborPos.vAmborPos = { 172.3f, 60.2f, 258.f, 1.f };
	AmborPos.fRadian = 180.f;
	AmborInfo.push_back(AmborPos); /* 성당 앞 */

	AmborPos.vAmborPos = { 171.3f, 60.2f, 258.f, 1.f };
	AmborPos.fRadian = 180.f;
	AmborInfo.push_back(AmborPos); /* 성당 앞 */

	AmborPos.vAmborPos = { 281.6f, 58.45f, 196.2f, 1.f };
	AmborPos.fRadian = 210.f;
	AmborInfo.push_back(AmborPos); /* 기사단 옥상 */

	m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	m_pTransformCom->Set_Scale({ 1.1f, 1.1f, 1.1f, 0.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, AmborInfo[m_eDialogue_Stage].vAmborPos); /* 몬드성 정문 앞 */
	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(AmborInfo[m_eDialogue_Stage].fRadian));

	m_strNPCName = TEXT("엠버");
	m_strNPCDetail = TEXT("<정찰 기사>  ");

	m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
	m_pInteractionBox->Add_InteractionBox(TEXT("엠버"), CInteraction_Box::ICON_TALK);

	COrtho::ORTHO_DESC tOrthoDesc = { };
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_QuestIcon");
	tOrthoDesc.fPosition = { 0.f,0.f };
	tOrthoDesc.fSize = { 65.f, 68.f };
	tOrthoDesc.ePage = PAGE_MAIN;
	tOrthoDesc.iDepth = 0;
	m_pOverHeadUI = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
	m_pOverHeadUI->Set_Text(m_strNPCName, CAlphaRect::TEXT_DOWN);
	m_pOverHeadUI->Set_Detail_Text(m_strNPCDetail);
	m_pOverHeadUI->SetTextureUV({ 474.f / 1500.f,636.f / 1000.f }, { 65.f / 1500.f,68.f / 1000.f });

	return S_OK;
}

HRESULT CAmbor::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		m_pOverHeadUI->PreviousTick(fTimeDelta);
		m_pInteractionBox->PreviousTick(fTimeDelta);

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CAmbor::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_bRotation == FALSE)
		{
			if (FAILED(__super::Tick(fTimeDelta)))
				return E_FAIL;
		}

		if (m_bConversation == true) /* Camera Scene */
		{
			if (FAILED(CAmbor::Set_CameraScene()))
				return E_FAIL;
		}
		else if((m_eDialogue_Stage == DIALOGUE_TEST || m_eDialogue_Stage == DIALOGUE_RETRY) && m_bConversation == FALSE)
		{
			CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
			if (pCamera != nullptr && pCamera->Get_CamScene() == CAMSCENEID_BASICTALK && pCamera->Get_SceneId(CAMSCENEID_BASICTALK) != SCENE_TWO)
			{
				pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_TWO, 3.f, 10.f);
			}
		}

#pragma region OHD Placement

		_float2 vPosition2D;
		_vector vPos;
		_matrix ProjMatrix, ViewMatrix, WorldMatrix;
		ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
		ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += 2.1f;

		WorldMatrix = XMMatrixIdentity();
		XMStoreFloat2(&vPosition2D, XMVector3Project(vPos,
			0, 0, g_iWinSizeX, g_iWinSizeY, 0, 1, ProjMatrix, ViewMatrix, WorldMatrix));

		// 월드 좌표를 정규화된 장치 좌표로 변환

		XMMATRIX worldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjMatrix;
		XMFLOAT4 clipCoord;
		XMStoreFloat4(&clipCoord, XMVector3TransformCoord(vPos, worldViewProjectionMatrix));

		// 정규화된 장치 좌표를 화면 좌표로 변환
		float screenX = (clipCoord.x + 1.0f) * 0.5f * 1600.0f;
		float screenY = (1.0f - clipCoord.y) * 0.5f * 900.0f;

		//-400 = Height
		m_pOverHeadUI->Update_Matrix(screenX - g_iWinSizeX / 2, -(screenY - g_iWinSizeY / 2), 50.f, 50.f);
		m_pOverHeadUI->SetTextureUV({ 474.f / 1500.f,636.f / 1000.f }, { 65.f / 1500.f,68.f / 1000.f });
#pragma endregion

		if (m_bConversation == true)
		{
			m_pInteractionBox->Tick(fTimeDelta);
			Execute_Dialogue();
		}
		else
		{
			if (m_pGliding_Test != nullptr && m_pGliding_Test->Get_ClearGame() == TRUE) /* 시험 통과 */
			{
				m_bTestClear = TRUE;
				m_eDialogue_Stage = DIALOGUE_PASSED;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, AmborInfo[m_eDialogue_Stage].vAmborPos);
				m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(AmborInfo[m_eDialogue_Stage].fRadian));
			}

			//플레이어 위치 찾아서 3.f 안에 들어오면 상호작용 가능
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vToPlayer = vPlayerPos - vPosition;
			XMVector3Length(vToPlayer);
			XMVector3Normalize(vToPlayer);

			if (XMVector3Length(vToPlayer).m128_f32[0] <= 3.f)
			{
				m_pInteractionBox->Tick(fTimeDelta);
				m_pOverHeadUI->Tick(fTimeDelta);

				if (m_pGameInstance->Key_Down('F') && (m_pGliding_Test == nullptr || m_pGliding_Test->Get_ClearGame() == TRUE))
				{
					if (m_pInteractionBox->Get_SelectedIndex() == 0)
					{
						m_bLookPlayer = TRUE;
						m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_TURN_START);
						m_pInteractionBox->Clear_Boxes();
						Begin_Dialogue();
						m_bConversation = true;
					}
					if(m_pGliding_Test != nullptr && m_pGliding_Test->Get_ClearGame() == TRUE)
					{
						m_pGliding_Test->Release_Object();
						Safe_Release(m_pGliding_Test);
					}
				}
			}
		}

		if (m_pGliding_Test != nullptr) /* Gliding Test */
			m_pGliding_Test->Update(fTimeDelta);

		if (m_pGliding_Test != nullptr && m_pGliding_Test->Get_TryAgain() == TRUE) /* Gliding Test TryAgain */
		{
			m_pGliding_Test->Release_Object();
			Safe_Release(m_pGliding_Test);

			m_eDialogue_Stage = DIALOGUE_RETRY;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, AmborInfo[m_eDialogue_Stage].vAmborPos);
			m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(AmborInfo[m_eDialogue_Stage].fRadian));
		}

		if (m_bLookPlayer == TRUE)
		{
			m_bRotation = FALSE;
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vSetDir = vPlayerPos - vPos;
			vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector4Normalize(vSetDir);

			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir, 13.f);

			pPlayerTransform->Reset_Look_At_Direction();
			pPlayerTransform->Set_Look_At_Direction(vSetDir * -1.f, 13.f);
		}

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && m_iCameraScene == 0 && m_bConversation == false
			&& (pCamera->Get_CamScene() == CAMSCENEID_AMBORTALK || pCamera->End_Scene(CAMSCENEID_BASICTALK) == TRUE)) /* Scene 끝나면 원래카메라 */
		{
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
		}

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CAmbor::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->LateTick(fTimeDelta);
		}

		//플레이어와 가까울 때만 LateTick
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;

		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);

		if (XMVector3Length(vToPlayer).m128_f32[0] <= 3.f )
		{
			if(m_bConversation == false)
				m_pOverHeadUI->LateTick(fTimeDelta);
			m_pInteractionBox->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CAmbor::Render()
{
	return S_OK;
}

CAmbor* CAmbor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAmbor* pInstance = new CAmbor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CAmbor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAmbor::Clone(void* pArg)
{
	CAmbor* pInstance = new CAmbor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CAmbor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAmbor::Free()
{
	__super::Free();
	
	for (auto& pPart : m_Parts)
	{
		pPart.pPartySlot->Release_ColliderID();
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();

	for (auto& iter : m_BreakPoints)
	{
		Safe_Delete_Array(iter.pNextBreak);
		Safe_Delete_Array(iter.pNextBreakIdx);
		Safe_Delete_Array(iter.pNexTerminatorIdx);
		Safe_Delete_Array(iter.pScriptFlow);
		Safe_Delete_Array(iter.pNextChoiceIdx);
	}
	m_BreakPoints.clear();
	AmborInfo.clear();

	Safe_Release(m_pGliding_Test);
	Safe_Release(m_pInteractionBox);
	Safe_Release(m_pOverHeadUI);
}