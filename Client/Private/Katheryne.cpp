#include "stdafx.h"
#include "Katheryne.h"

#include "Part_Body.h"
#include "Part_Weapon.h"

#include "GameInstance.h"

#include "CharacterController.h"

#include "AlphaRect.h"
#include "InteractionBox.h"
#include "Dialogue_Page.h"
#include "Page_Main.h"

#include "UI_Manager.h"
#include "InputQuests.h"
#include "KillQuests.h"
#include "Inventory.h"

CKatheryne::CKatheryne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CKatheryne::CKatheryne(const CKatheryne& rhs)
	: CCreature(rhs)
{
}

HRESULT CKatheryne::Add_Parts(void* pArg)
{
	CPartObject*	pPartObject = { };
	PARTY_SLOTS		tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = NPC_KATHERYNE;
	tBodyDesc.eType = CTY_NPC;

	CREATUREID eCreatureID = NPC_KATHERYNE;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	tSlots = { };

	/* For. Part_Weapon */
	//CModel* pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[0]->Find_Component(TEXT("Com_Model")));

	//CPart_Weapon::PARTWEAPON_DESC tWeaponDesc = { };
	//tWeaponDesc.pBody = pBody;
	//tWeaponDesc.tPartObjDesc.pParent = this;
	//tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_WORLD;

	//pPartObject = CPart_Weapon::Create_Weapon(WEAPON_WOLFMOUND, tWeaponDesc);
	//if (pPartObject == nullptr)
	//	return E_FAIL;

	//tSlots.pPartySlot[0] = pPartObject;
	//m_Parts.push_back(tSlots);

	//tSlots = { };

	//OverHead TEST
	COrtho::ORTHO_DESC tOrthoDesc = { };
	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_QuestIcon");
	tOrthoDesc.fPosition = { 0.f,0.f };
	tOrthoDesc.fSize = { 65.f, 68.f };
	tOrthoDesc.ePage = PAGE_MAIN;
	tOrthoDesc.iDepth = 0;
	m_pOverHeadUI = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
	m_pOverHeadUI->Set_Text(TEXT("캐서린"), CAlphaRect::TEXT_DOWN);
	m_pOverHeadUI->Set_Detail_Text(TEXT("<모험가 길드>"));
	m_pOverHeadUI->SetTextureUV({474.f/1500.f,636.f/1000.f}, {65.f/1500.f,68.f/1000.f});

	return S_OK;
}

HRESULT CKatheryne::Ready_Script()
{
	wstring strNpcScript;
	wstring strNpcChoice;
	CDialogue_Page::BREAK_DESC tbreak_Desc = {};

	strNpcScript = TEXT("별과 심연을 향해! 모험가 길드에 온 것을 환영해.");
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("제 이름은 캐서린, 길드에서 안내원을 맡고 있답니다.");
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("저희 길드에서는 새로운 모험가들을 위해 다양한 서비스를 제공하고있습니다.");
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("오늘은 어떤 서비스가 필요하신가요?"); //3
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("필요한 것이 있다면 다시 들러주세요!"); //4
	m_NPCScript.push_back(strNpcScript);

	strNpcChoice = TEXT("퀘스트 수주"); //if First Quest = to 5-7 break8 , if First Quest == STATE_INPROGRESS = to 14 break 15 if FirstQuest == Completed = to 18-21break 21 if SecondQuest ==COMPLETED 16-18 break18
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("모험가 길드에 대해"); // to 11-13                                                         
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("떠난다"); // to 4
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 3;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 8,18,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 1,0,0 }; //다음 선택지 인덱스
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 15,0,4 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 5,17,4 };
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 3,0,0 };
	m_BreakPoints.push_back(tbreak_Desc);

	strNpcScript = TEXT("모험가님에게 적합한 퀘스트를 찾아볼게요."); //Script 5
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("여행자님, 마침 길드에 안드리우스 토벌 관련 의뢰가 들어왔어요!"); //Script 6
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("이 의뢰는 꽤 위험한 편이라 자격 요건을 확인해야 합니다"); //Script 7
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("혹시 자격 요건을 충족하셨나요?"); // 선택지2 //Script 8
	m_NPCScript.push_back(strNpcScript);



	//선택지 1 정보
	strNpcChoice = TEXT("자격 요건이 뭔데?"); // Choice Idx 3
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 1;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 10 }; //Page That Next Break Point is Called
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 2 }; // Next BreakPoint Within Container
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 15 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 9 }; //Page This Choice will Lead to
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 4 }; // Next Choice String
	m_BreakPoints.push_back(tbreak_Desc);



	strNpcScript = TEXT("시련의 비경을 클리어하면 자격 요건이 충족됩니다"); //Script 9
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("혹시 이미 그 시련을 이겨낸 경험이 있으신가요?");//Script 10
	m_NPCScript.push_back(strNpcScript);



	//Break 2

	strNpcChoice = TEXT("아니, 없어"); //Choice Idx 4
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 1;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 12 };
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 3 };
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 15 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 13 };
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 5 };
	m_BreakPoints.push_back(tbreak_Desc);



	strNpcScript = TEXT("안타깝게도, 아직 시련의 비경을 클리어하지 않으셨군요");//Script 11
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("먼저 비경을 클리어하고 나서 안드리우스 토벌에 도전하시겠어요?");//Script 12
	m_NPCScript.push_back(strNpcScript);


	//Break 3
	strNpcChoice = TEXT("그래, 알겠어"); //Choice Idx 5
	m_NPCChoice.push_back(strNpcChoice);

	tbreak_Desc.iNumBoxes = 1;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0 };
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 0 };
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 15 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 14 };
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 0 };
	m_BreakPoints.push_back(tbreak_Desc);



	strNpcScript = TEXT("비경을 클리어하신 후에 언제든 모험가 길드에 들려주세요!"); //Script 13
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("그때 안드리우스 토벌에 관한 의뢰를 전해드릴게요");//Script 14
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("행운을 빌어요!"); //Script 15
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("저희 모험가 길든는 초보 모험가들의 정착을 지원하기 위해 세워진 길드입니다."); //Script 16
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("몬드 성 주변에서 초보 모험가분들에게 적합한 의뢰들을 접수하여 소개해드리고 있습니다."); //Script 17
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("다른 용무가 있으실까요?"); //Script 18
	m_NPCScript.push_back(strNpcScript);


	//Script2 Start STATE_DUNGEON_CLEARED


	strNpcScript = TEXT("별과 심연을 향해!"); //Script2 0 
	m_NPCScript2.push_back(strNpcScript);

	strNpcScript = TEXT("모험가 길드에 온 것을 환영해"); //Script2 1
	m_NPCScript2.push_back(strNpcScript);

	strNpcScript = TEXT("제 이름은 캐서린, 모험가 길드에서 안내를 담당하고 있어요"); //Script2 
	m_NPCScript2.push_back(strNpcScript);

	strNpcScript = TEXT("어떤 이유로 저를 찾아주셨나요 ?"); //Script2 3 -- Break1
	m_NPCScript2.push_back(strNpcScript);


	//BREAK4
	tbreak_Desc.iNumBoxes = 2;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 4,4 }; //다음 선택지 인덱스
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 7,4 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 5,4 };
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 6,6 };
	m_BreakPoints.push_back(tbreak_Desc);


	strNpcChoice = TEXT("안드리우스 토벌 의뢰를 받고 싶어"); //Choice Idx 6
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("떠난다"); //Choice Idx 7
	m_NPCChoice.push_back(strNpcChoice);


	strNpcScript = TEXT("필요한 것이 있다면 다시 들러주세요!"); //Script2 4
	m_NPCScript2.push_back(strNpcScript);

	strNpcScript = TEXT("시련 비경 클리어에 성공하셨군요! 축하드려요!"); //Script2 5
	m_NPCScript2.push_back(strNpcScript);

	strNpcScript = TEXT("이제 안드리우스 토벌에 도전할 준비가 다 되었습니다"); //Script2 6
	m_NPCScript2.push_back(strNpcScript);

	strNpcScript = TEXT("상당한 보상이 여행자님을 기다리고 있으니 행운을 빌게요!"); //Script2 7
	m_NPCScript2.push_back(strNpcScript);


	//Script 3 Start STATE_ANDRIUS_CLEARED

	strNpcScript = TEXT("별과 심연을 향해!"); //Script3 0 
	m_NPCScript3.push_back(strNpcScript);

	strNpcScript = TEXT("모험가 길드에 온 것을 환영해"); //Script3 1
	m_NPCScript3.push_back(strNpcScript);

	strNpcScript = TEXT("어떤 이유로 저를 찾아주셨나요 ?"); //Script3 2 -- Break1
	m_NPCScript3.push_back(strNpcScript);

	//BREAK5

	tbreak_Desc.iNumBoxes = 2;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 5,5 }; //다음 선택지 인덱스
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 6,3 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 4,3 };
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 8,8 };
	m_BreakPoints.push_back(tbreak_Desc);

	strNpcChoice = TEXT("새로운 퀘스트"); //Choice Idx 8
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("떠난다"); //Choice Idx 9
	m_NPCChoice.push_back(strNpcChoice);


	strNpcScript = TEXT("필요한 것이 있다면 다시 들러주세요!"); //Script3 3
	m_NPCScript3.push_back(strNpcScript);

	strNpcScript = TEXT("모험가님에게 알맞는 퀘스트가 들어와 있어요"); //Script3 4
	m_NPCScript3.push_back(strNpcScript);

	strNpcScript = TEXT("숲을 불태우는 무상의 불을 토벌해 주세요."); //Script3 5
	m_NPCScript3.push_back(strNpcScript);

	strNpcScript = TEXT("무상의 불은 상당히 위험하니 꼭 음식을 챙겨 가세요!"); //Script3 6
	m_NPCScript3.push_back(strNpcScript);

	//Script 4 Start STATE_EFFIGY_CLEARED

	strNpcScript = TEXT("여행자님!큰 일이 벌어졌어요"); //Script3 0
	m_NPCScript4.push_back(strNpcScript);

	strNpcScript = TEXT("풍마룡이 침공하여 현재 몬드의 상황이 매우 위급합니다"); //Script3 1
	m_NPCScript4.push_back(strNpcScript);

	strNpcScript = TEXT("길드에서 쌓은 경험과 힘을 발휘하면 여행자님은 충분히 이 상황을 해결하실 수 있을거예요"); //Script3 2
	m_NPCScript4.push_back(strNpcScript);

	strNpcScript = TEXT("풍마룡의 침공에 대처하여 저희 몬드를 지키는데 도움을 주세요"); //Script3 3
	m_NPCScript4.push_back(strNpcScript);


	//Script 5 Start STATE_DVALIN_CLEARED

	strNpcScript = TEXT("역시 여행자님이 저희 몬드를 지켜주실거라고 믿고있었어요!"); //Script4 0
	m_NPCScript5.push_back(strNpcScript);

	strNpcScript = TEXT("보답으로 저희의 마음을 담은 선물을 드릴게요"); //Script4 1
	m_NPCScript5.push_back(strNpcScript);

	strNpcScript = TEXT("여행자님의 모험에 항상 행운이 가득하길!"); //Script4 2
	m_NPCScript5.push_back(strNpcScript);
	
	//Script 6 Start STATE_FINAL_STAGE

	strNpcScript = TEXT("별과 심연을 향해!"); //Script6 0 
	m_NPCScript6.push_back(strNpcScript);

	strNpcScript = TEXT("모험가 길드에 온 것을 환영해"); //Script6 1
	m_NPCScript6.push_back(strNpcScript);

	strNpcScript = TEXT("오늘은 어떤 이유로 저를 찾아주셨나요?"); //Script6 2 -- Break1
	m_NPCScript6.push_back(strNpcScript);


	tbreak_Desc.iNumBoxes = 2;
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 2,2 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 6,6 }; //다음 선택지 인덱스
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 6,3 };
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 4,3 };
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 10,10 };
	m_BreakPoints.push_back(tbreak_Desc);
	//Script 5 Break 1

	strNpcChoice = TEXT("이제 뭘 하면 좋을까?"); //Choice Idx 10
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("떠난다"); //Choice Idx 11
	m_NPCChoice.push_back(strNpcChoice);

	strNpcScript = TEXT("어떤 이유로 저를 찾아주셨나요 ?"); //Script6 3 
	m_NPCScript6.push_back(strNpcScript);
	strNpcScript = TEXT("소문에 의하면 연금술 조합에는 소유자에게 새로운 기회를 열어 줄 수 있는,"); //Script6 4
	m_NPCScript6.push_back(strNpcScript);
	strNpcScript = TEXT("신비한 아이템을 만들 수 있는 아주 특별한 조합법이 숨겨져 있다고 해요."); //Script6 5 
	m_NPCScript6.push_back(strNpcScript);
	strNpcScript = TEXT("그 비밀을 파헤쳐보는건 어떨까요?"); //Script6 6
	m_NPCScript6.push_back(strNpcScript);

	//Repeat
	strNpcScript = TEXT("연금술사는 광장쪽에 있어요."); //Script6 7
	m_NPCScript6.push_back(strNpcScript);

	CKill_Quest::KILL_QUEST_DESC tKillQuestDesc1 = {};
	wstring strTarget = { L"Dvalin" };
	tKillQuestDesc1.CreatureID = strTarget;
	tKillQuestDesc1.iKillQty = 1;
	tKillQuestDesc1.eQuest_State = CQuest::QUEST_NOT_ACCEPTED;
	tKillQuestDesc1.eQuest_Type = CQuest::QUEST_LEGENDARY;
	tKillQuestDesc1.strQuestName = TEXT("드발린 토벌");
	tKillQuestDesc1.strQuestDetail = TEXT("드발린 1마리 토벌");
	tKillQuestDesc1.strObjectiveTexts = TEXT("폭풍룡 드발린을 토벌하였다.");
	m_pKillQuest = (CKill_Quest*)CKill_Quest::Create(&tKillQuestDesc1);

	CKill_Quest::KILL_QUEST_DESC tKillQuestDesc2 = {};
	strTarget = { L"Effigy_Fire" };
	tKillQuestDesc2.CreatureID = strTarget;
	tKillQuestDesc2.iKillQty = 1;
	tKillQuestDesc2.eQuest_State = CQuest::QUEST_NOT_ACCEPTED;
	tKillQuestDesc2.eQuest_Type = CQuest::QUEST_LEGACY;
	tKillQuestDesc2.strQuestName = TEXT("무상의 불 토벌");
	tKillQuestDesc2.strQuestDetail = TEXT("무상의 불 1마리 토벌");
	tKillQuestDesc2.strObjectiveTexts = TEXT("무상의 불을 토벌하였다.");
	m_pKillQuest1 = (CKill_Quest*)CKill_Quest::Create(&tKillQuestDesc2);

	CKill_Quest::KILL_QUEST_DESC tKillQuestDesc = {};
	strTarget = { L"Andrius" };
	tKillQuestDesc.CreatureID = strTarget;
	tKillQuestDesc.iKillQty = 1;
	tKillQuestDesc.eQuest_State = CQuest::QUEST_NOT_ACCEPTED;
	tKillQuestDesc.eQuest_Type = CQuest::QUEST_LEGACY;
	tKillQuestDesc.strQuestName = TEXT("안드리우스 토벌");
	tKillQuestDesc.strQuestDetail = TEXT("안드리우스 1마리 토벌");
	tKillQuestDesc.strObjectiveTexts = TEXT("북부 늑대의 영주 안드리우스를 토벌하였다.");
	m_pKillQuest2 = (CKill_Quest*)CKill_Quest::Create(&tKillQuestDesc);


	CKill_Quest::KILL_QUEST_DESC tKillQuestDesc3 = {};
	strTarget = { L"Abyss_Mage" };
	tKillQuestDesc3.CreatureID = strTarget;
	tKillQuestDesc3.iKillQty = 1;
	tKillQuestDesc3.eQuest_State = CQuest::QUEST_NOT_ACCEPTED;
	tKillQuestDesc3.eQuest_Type = CQuest::QUEST_NORMAL;
	tKillQuestDesc3.strQuestName = TEXT("자격 증명");
	tKillQuestDesc3.strQuestDetail = TEXT("심연 메이지 1마리 토벌");
	tKillQuestDesc3.strObjectiveTexts = TEXT("모험가 길드에서 제시한 시련을 통과해 자격을 증명하였다.");
	m_pKillQuest3 = (CKill_Quest*)CKill_Quest::Create(&tKillQuestDesc3);

	return S_OK;
}

void CKatheryne::Begin_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC,TEXT("Layer_UIManager"),0);
	pUIManager->Show_Page(PAGE_DIALOGUE);
	m_pDialogue_Page = (CDialogue_Page*)pUIManager->Get_CurrentPage();
	CDialogue_Page::BREAK_DESC	tBreakDesc;

	if (m_pKillQuest3->Get_QuestState() == CQuest::QUEST_CAN_COMPLETE)
	{
		m_pKillQuest3->Set_QuestState(CQuest::QUEST_COMPLETED);
		m_eQuestState = CKatheryne::STATE_DUNGEON_CLEARED;
		m_bDialogueEnd = false;
	}
	if (m_pKillQuest2->Get_QuestState() == CQuest::QUEST_CAN_COMPLETE)
	{
		m_pKillQuest2->Set_QuestState(CQuest::QUEST_COMPLETED);
		m_eQuestState = CKatheryne::STATE_ANDRIUS_CLEARED;
		m_bDialogueEnd = false;
	}
	if (m_pKillQuest1->Get_QuestState() == CQuest::QUEST_CAN_COMPLETE)
	{
		m_pKillQuest1->Set_QuestState(CQuest::QUEST_COMPLETED);
		m_eQuestState = CKatheryne::STATE_EFFIGY_CLEARED;
		m_bDialogueEnd = false;
	}
	if (m_pKillQuest->Get_QuestState() == CQuest::QUEST_CAN_COMPLETE)
	{
		m_pKillQuest->Set_QuestState(CQuest::QUEST_COMPLETED);
		m_eQuestState = CKatheryne::STATE_DVALIN_CLEARED;
		m_bDialogueEnd = false;
	}
	if (m_eQuestState == CKatheryne::STATE_DVALIN_CLEARED && m_bDialogueEnd == true)
	{
		m_eQuestState = STATE_FINAL_STAGE;
		m_bDialogueEnd = false;
	}

	switch (m_eQuestState)
	{
	case Client::CKatheryne::STATE_BEGIN:
		//Dialogue 1
		if (m_bDialogueEnd == true)
		{
			m_NPCScriptDisplayed = m_NPCScript;
			tBreakDesc = m_BreakPoints[0];
			m_pDialogue_Page->Set_Script(m_NPCScript[15]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 0;
			m_iBreakPointPg = 3;
			m_iTerminatorPg = 15;
			m_iParagraphIndex = 15;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 0;
		}
		else
		{
			m_NPCScriptDisplayed = m_NPCScript;
			tBreakDesc = m_BreakPoints[0];
			m_pDialogue_Page->Set_Script(m_NPCScript[0]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 0;
			m_iBreakPointPg = 3;
			m_iTerminatorPg = 16;
			m_iParagraphIndex = 0;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 0;
		}
		break;
	case Client::CKatheryne::STATE_DUNGEON_CLEARED:
		if (m_bDialogueEnd == true)
		{
			m_NPCScriptDisplayed = m_NPCScript2;
			tBreakDesc = m_BreakPoints[4];
			m_pDialogue_Page->Set_Script(m_NPCScript2[7]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 4;
			m_iBreakPointPg = 3;
			m_iTerminatorPg = 7;
			m_iParagraphIndex = 7;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 6;
		}
		else
		{
			m_NPCScriptDisplayed = m_NPCScript2;
			tBreakDesc = m_BreakPoints[4];
			m_pDialogue_Page->Set_Script(m_NPCScript2[0]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 4;
			m_iBreakPointPg = 3;
			m_iTerminatorPg = 7;
			m_iParagraphIndex = 0;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 6;
		}
		break;
	case Client::CKatheryne::STATE_ANDRIUS_CLEARED:
		if (m_bDialogueEnd == true)
		{
			m_NPCScriptDisplayed = m_NPCScript3;
			tBreakDesc = m_BreakPoints[5];
			m_pDialogue_Page->Set_Script(m_NPCScript3[6]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 5;
			m_iBreakPointPg = 2;
			m_iTerminatorPg = 6;
			m_iParagraphIndex = 6;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 8;
		}
		else
		{
			m_NPCScriptDisplayed = m_NPCScript3;
			tBreakDesc = m_BreakPoints[5];
			m_pDialogue_Page->Set_Script(m_NPCScript3[0]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 5;
			m_iBreakPointPg = 2;
			m_iTerminatorPg = 6;
			m_iParagraphIndex = 0;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 8;
		}
		break;
	case Client::CKatheryne::STATE_EFFIGY_CLEARED:
		if (m_bDialogueEnd == true)
		{
			m_NPCScriptDisplayed = m_NPCScript4;
			tBreakDesc = m_BreakPoints[0];
			m_pDialogue_Page->Set_Script(m_NPCScript4[3]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 0;
			m_iBreakPointPg = -1;
			m_iTerminatorPg = 3;
			m_iParagraphIndex = 3;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 10;
		}
		else
		{
			m_NPCScriptDisplayed = m_NPCScript4;
			tBreakDesc = m_BreakPoints[5];
			m_pDialogue_Page->Set_Script(m_NPCScript4[0]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 0;
			m_iBreakPointPg = -1;
			m_iTerminatorPg = 3;
			m_iParagraphIndex = 0;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 10;
		}
		break;
	case Client::CKatheryne::STATE_DVALIN_CLEARED:
		m_NPCScriptDisplayed = m_NPCScript5;
		tBreakDesc = m_BreakPoints[5];
		m_pDialogue_Page->Set_Script(m_NPCScript5[0]);
		m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
		m_iCurrentBreakPt = -1;
		m_iBreakPointPg = -1;
		m_iTerminatorPg = 2;
		m_iParagraphIndex = 0;
		m_iNumChoice = tBreakDesc.iNumBoxes;
		m_iCurrentChoiceIdx = -1;
		break;
	case Client::CKatheryne::STATE_FINAL_STAGE:
		if (m_bDialogueEnd == true)
		{
			m_NPCScriptDisplayed = m_NPCScript6;
			tBreakDesc = m_BreakPoints[6];
			m_pDialogue_Page->Set_Script(m_NPCScript6[7]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 6;
			m_iBreakPointPg = -1;
			m_iTerminatorPg = 7;
			m_iParagraphIndex = 7;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 10;
		}
		else
		{
			m_NPCScriptDisplayed = m_NPCScript6;
			tBreakDesc = m_BreakPoints[6];
			m_pDialogue_Page->Set_Script(m_NPCScript6[0]);
			m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
			m_iCurrentBreakPt = 6;
			m_iBreakPointPg = 2;
			m_iTerminatorPg = 6;
			m_iParagraphIndex = 0;
			m_iNumChoice = tBreakDesc.iNumBoxes;
			m_iCurrentChoiceIdx = 10;
		}
		break;
	case Client::CKatheryne::STATE_END:

		break;
	default:
		break;
	}

	CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
	{
		pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_ONE, 0.f, 10.f);
	}
}

void CKatheryne::Execute_Dialogue()
{
	vector<CDialogue_Page::BREAK_DESC> vec_BreakPts;
	vec_BreakPts = m_BreakPoints;


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

			m_pDialogue_Page->Set_Script(m_NPCScriptDisplayed[m_iParagraphIndex]);
		}

	}
	else
	{

		if (m_pGameInstance->Key_Down('F'))
		{
			if (m_iParagraphIndex == m_NPCScriptDisplayed.size() - 1 || m_iParagraphIndex == m_iTerminatorPg)
			{
				End_Dialogue();
			}
			else
			{
			//페이지 넘기기
			m_iParagraphIndex++;
			m_pDialogue_Page->Set_Script(m_NPCScriptDisplayed[m_iParagraphIndex]);
			}

		}
	}

}

void CKatheryne::End_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC,TEXT("Layer_Inventory"));
	pUIManager->Show_Page(PAGE_MAIN);
	m_eNpcState = STATE_NORMAL;
	m_pInteractionBox->Add_InteractionBox(TEXT("대화하기"), CInteraction_Box::ICON_TALK);

	if (m_eQuestState == STATE_BEGIN && m_iParagraphIndex == 15)
	{
		m_bDialogueEnd = true;
		if (m_pKillQuest3->Get_QuestState() == CQuest::QUEST_NOT_ACCEPTED)
		{
			m_pKillQuest3->Set_QuestState(CQuest::QUEST_ACCEPTED);
		}
	}

	if (m_eQuestState == STATE_DUNGEON_CLEARED && m_iParagraphIndex == 7)
	{
		m_bDialogueEnd = true;
		if (m_pKillQuest2->Get_QuestState() == CQuest::QUEST_NOT_ACCEPTED)
		{
			m_pKillQuest2->Set_QuestState(CQuest::QUEST_ACCEPTED);
		}
	}

	if (m_eQuestState == STATE_ANDRIUS_CLEARED && m_iParagraphIndex == 6)
	{
		m_bDialogueEnd = true;
		if (m_pKillQuest1->Get_QuestState() == CQuest::QUEST_NOT_ACCEPTED)
		{
			m_pKillQuest1->Set_QuestState(CQuest::QUEST_ACCEPTED);
		}
	}

	if (m_eQuestState == STATE_EFFIGY_CLEARED && m_iParagraphIndex == 3)
	{
		m_bDialogueEnd = true;
		if (m_pKillQuest->Get_QuestState() == CQuest::QUEST_NOT_ACCEPTED)
		{
			m_pKillQuest->Set_QuestState(CQuest::QUEST_ACCEPTED);
		}
	}

	if (m_eQuestState == STATE_DVALIN_CLEARED && m_iParagraphIndex == 2)
	{
		m_bDialogueEnd = true;
	}

	if (m_eQuestState == STATE_FINAL_STAGE && m_iParagraphIndex == 6)
	{
		m_bDialogueEnd = true;
	}

	//if (m_pKillQuest->Get_QuestState() == CQuest::QUEST_CAN_COMPLETE)
	//{
	//	m_pKillQuest->Set_QuestState(CQuest::QUEST_COMPLETED);
	//	pInven->Add_Item(CItem::Create_Item(125, 1600));
	//}

	//if (m_pKillQuest->Get_QuestState() == CQuest::QUEST_COMPLETED)
	//{
	//	m_BreakPoints[0].pNextBreak[0] = 25;
	//	m_BreakPoints[0].pScriptFlow[0] = 16;
	//	m_BreakPoints[0].pNexTerminatorIdx[0] = 17;
	//}

	CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
	{
		pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_TWO, 3.f, 10.f);
	}

}

HRESULT CKatheryne::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKatheryne::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Script()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 1.f, 1.f, 1.f, 0.f });
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->Get_CurrCellCenter()); /* 임시 */
	_float4 vPos = _float4(415.5f, 12.5f, 246.2f, 1.0f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ FSM_DASH, FSM_JUMP, FSM_FLY_START, FSM_FLY_NORMAL, FSM_FLY_LEFT, FSM_FLY_RIGHT, FSM_DROP_DOWN,
																FSM_FALL_ATTACK_START1, FSM_FALL_ATTACK_START2, FSM_FALL_ATTACK, FSM_FALL_ATTACK_END1, FSM_FALL_ATTACK_END2,
																FSM_ATTACK1, FSM_ATTACK2, FSM_ATTACK3, FSM_ATTACK4,
																FSM_ATTACK_CHARGED1, FSM_ATTACK_CHARGED2, FSM_ELEMENTAL_BURST,
																FSM_ELEMENTAL_SKILL1, FSM_ELEMENTAL_SKILL2, FSM_ELEMENTAL_SKILL3,
																FSM_ELEMENTAL_SKILL1_IDLE, FSM_ELEMENTAL_SKILL2_IDLE, FSM_ELEMENTAL_SKILL3_IDLE});

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//m_pController = CCharacterController::Create(1.0f, 0.25f, vPosition, m_pTransformCom);

	//COrtho::ORTHO_DESC tOrthoDesc;


	//tOrthoDesc.eTextureLevel = LEVEL_WORLD;
	//tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Interaction_Box");
	//tOrthoDesc.fPosition = { static_cast<_float>(1080.f - g_iWinSizeX / 2), static_cast<_float>(-(450.f - g_iWinSizeY / 2)) };
	//tOrthoDesc.fSize = { 240.f,45.f };
	//tOrthoDesc.ePage = PAGE_MAIN;
	//m_pInteractionBox = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
	//m_pInteractionBox->Set_Text(TEXT("대화한다"), CAlphaRect::TEXT_ON);

	m_strNPCName = TEXT("캐서린");

	m_strNPCDetail = TEXT("모험가 길드");

	m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);

	m_pInteractionBox->Add_InteractionBox(TEXT("대화하기"), CInteraction_Box::ICON_TALK);

	return S_OK;
}

HRESULT CKatheryne::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		m_Parts[0].pPartySlot->PreviousTick(fTimeDelta);

		m_pOverHeadUI->PreviousTick(fTimeDelta);
		m_pInteractionBox->PreviousTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CKatheryne::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_eNpcState == STATE_DIALOGUE)
		{
			m_pInteractionBox->Tick(fTimeDelta);
			Execute_Dialogue();
		}
		else
		{
			m_pTransformCom->Verify_Look_At_Direction();
			m_pTransformCom->Invalidate_Look_At_Lerp(fTimeDelta);

			m_Parts[0].pPartySlot->Tick(fTimeDelta);


			_float2 vPosition2D;
			_vector vPos;
			_matrix ProjMatrix, ViewMatrix, WorldMatrix;
			ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
			ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
			vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.m128_f32[1] += 2.4f;

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

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vToPlayer = vPlayerPos - vPosition;

			XMVector3Length(vToPlayer);
			XMVector3Normalize(vToPlayer);
			if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0 &&
				XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
			{
				CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
				CPage_Main* m_pPage_Main = (CPage_Main*)pUIManager->Get_UI_Page(PAGE_MAIN);

				m_pPage_Main->Register_On_MiniMap(m_pTransformCom, 2);
				m_pInteractionBox->Tick(fTimeDelta);

				if (m_pGameInstance->Key_Down('F'))
				{
					if (m_pInteractionBox->Get_SelectedIndex() == 0)
					{
						m_pInteractionBox->Clear_Boxes();
						Begin_Dialogue();
						m_eNpcState = STATE_DIALOGUE;						
					}
				}

				CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
				if (pCamera != nullptr && pCamera->End_Scene(CAMSCENEID_BASICTALK) == TRUE) /* Scene 끝나면 원래카메라 */
				{
					pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
				}
			}

		}
			m_pOverHeadUI->Tick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CKatheryne::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	m_Parts[0].pPartySlot->LateTick(fTimeDelta);

	////TEST
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;

	XMVector3Length(vToPlayer);
	XMVector3Normalize(vToPlayer);
	if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0 &&
		XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
	{

		if (m_eNpcState != STATE_DIALOGUE)
		{
			m_pOverHeadUI->LateTick(fTimeDelta);
		}

		m_pInteractionBox->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CKatheryne::Render()
{
	return S_OK;
}

CKatheryne* CKatheryne::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKatheryne* pInstance = new CKatheryne(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CKatheryne"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKatheryne::Clone(void* pArg)
{
	CKatheryne* pInstance = new CKatheryne(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CKatheryne"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKatheryne::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
	{
		pPart.pPartySlot->Release_ColliderID();
		Safe_Release(pPart.pPartySlot);
	}

	m_Parts.clear();

	if (m_bCloned == true)
	{
		Safe_Release(m_pOverHeadUI);
		Safe_Release(m_pInteractionBox);

		for (auto& iter : m_BreakPoints)
		{
			Safe_Delete_Array(iter.pNextBreak);
			Safe_Delete_Array(iter.pNextBreakIdx);
			Safe_Delete_Array(iter.pNexTerminatorIdx);
			Safe_Delete_Array(iter.pScriptFlow);
			Safe_Delete_Array(iter.pNextChoiceIdx);
		}
		m_BreakPoints.clear();
	}

	Safe_Release(m_pController);
}
