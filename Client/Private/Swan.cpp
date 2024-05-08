#include "stdafx.h"
#include "Swan.h"
#include "Part_Body.h"

#include "GameInstance.h"
#include "InteractionBox.h"
#include "Player.h"
#include "UI_Manager.h"
#include "Camera_Main.h"
#include "Page_Main.h"

CSwan::CSwan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CSwan::CSwan(const CSwan& rhs)
	: CCreature(rhs)
{
}

HRESULT CSwan::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = NPC_SWAN;
	tBodyDesc.eType = CTY_NPC;

	CREATUREID eCreatureID = NPC_SWAN;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CSwan::Ready_Script()
{
	wstring strNpcScript;
	wstring strNpcChoice;
	CDialogue_Page::BREAK_DESC tbreak_Desc = {};

	strNpcScript = TEXT("안녕.기사단의 도움이 필요한가"); //0
	m_NPCScript.push_back(strNpcScript);


	strNpcChoice = TEXT("당신은?"); // to 2 break at 4
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("이만 가볼게요"); // to 4-5                                                         
	m_NPCChoice.push_back(strNpcChoice);


	//BreakPt 1
	tbreak_Desc.iNumBoxes = 2; //선택지 수
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 2,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 1,0 }; //다음 선택지 인덱스
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 1,3};	 //이어갈 대사 위치
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 6,3 }; //종료 위치
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 2,0 }; //다음 선택지 인덱스
	m_BreakPoints.push_back(tbreak_Desc);


	strNpcScript = TEXT("난 보초를 맡은 페보니우스 기사단의 스완이야"); //1
	m_NPCScript.push_back(strNpcScript);
	strNpcScript = TEXT("그리고 << 기사단 매뉴얼 >> 에 따라 행성들을 돕는 일도 하고 있어");
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("그래");										//3
	m_NPCScript.push_back(strNpcScript);


	strNpcChoice = TEXT("<< 기사단 매뉴얼 >> 이요?"); // to 5 break at 6
	m_NPCChoice.push_back(strNpcChoice);
	strNpcChoice = TEXT("몬드에 가볼 만한 곳이 있나요?"); // to 6-7                                                         
	m_NPCChoice.push_back(strNpcChoice);

	//BreakPt 1
	tbreak_Desc.iNumBoxes = 2; //선택지 수
	tbreak_Desc.pNextBreak = new _int[tbreak_Desc.iNumBoxes]{ 0,0 };  //각 선택지 선택시 다음 선택지 위치
	tbreak_Desc.pNextBreakIdx = new _int[tbreak_Desc.iNumBoxes]{ 0,0 }; //다음 선택지 인덱스
	tbreak_Desc.pScriptFlow = new _int[tbreak_Desc.iNumBoxes]{ 4,5 };	 //이어갈 대사 위치
	tbreak_Desc.pNexTerminatorIdx = new _int[tbreak_Desc.iNumBoxes]{ 4,5 }; //종료 위치
	tbreak_Desc.pNextChoiceIdx = new _int[tbreak_Desc.iNumBoxes]{ 0,0 }; //다음 선택지 인덱스
	m_BreakPoints.push_back(tbreak_Desc);


	strNpcScript = TEXT(" 응. << 기사단 매뉴얼 >> 은 페보니우스 기사단의 행동 강령이자, 몬드의 평화와 안전의 초석이지…"); //4
	m_NPCScript.push_back(strNpcScript);

	strNpcScript = TEXT("몬드에 왔으면 몬드성 구경은 해봐야지. 볼거리가 아주 많아"); //5
	m_NPCScript.push_back(strNpcScript);

	return S_OK;
}

void CSwan::Begin_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	pUIManager->Show_Page(PAGE_DIALOGUE);
	m_pDialogue_Page = (CDialogue_Page*)pUIManager->Get_CurrentPage();
	CDialogue_Page::BREAK_DESC	tBreakDesc;

	//Dialogue 1
	m_NPCScriptDisplayed = m_NPCScript;
	tBreakDesc = m_BreakPoints[0];
	m_pDialogue_Page->Set_Script(m_NPCScript[0]);
	m_pDialogue_Page->Set_Target(m_strNPCName, m_strNPCDetail);
	m_iCurrentBreakPt = 0;
	m_iBreakPointPg = 0;
	m_iTerminatorPg = 4;
	m_iParagraphIndex = 0;
	m_iNumChoice = tBreakDesc.iNumBoxes;
	m_iCurrentChoiceIdx = 0;
}

void CSwan::Execute_Dialogue()
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
			if (m_bDialogueEnd == TRUE && (m_iCameraScene % 2) == 1) /* 끝난 대화 */
			{
				CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
				if (pCamera != nullptr)
				{
					pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_TWO, 3.f, 10.f);
				}
				m_iCameraScene = 0;
				m_bDialogueEnd = FALSE;
			}
		}
	}
}

void CSwan::End_Dialogue()
{
	CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	pUIManager->Show_Page(PAGE_MAIN);
	m_eNpcState = STATE_NORMAL;
	m_pInteractionBox->Add_InteractionBox(TEXT("스완"), CInteraction_Box::ICON_TALK);
	m_bGreet = FALSE;
	m_bDialogueEnd = TRUE;
	m_bLookPlayer = FALSE;
}

HRESULT CSwan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwan::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Script()))
		return E_FAIL;

	m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_SOLIDER_IDLE);
	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(90.f));
	m_vInitLookDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 451.2f, 10.f, 252.5f, 1.f }); /* 몬드성 정문 앞 */

	m_strNPCName = TEXT("스완");
	m_strNPCDetail = TEXT("<페보니우스 기사>  ");

	m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
	m_pInteractionBox->Add_InteractionBox(TEXT("스완"), CInteraction_Box::ICON_TALK);

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
	m_pOverHeadUI->SetTextureUV({ 505.f / 1500.f,853.f / 1000.f }, { 63.f / 1500.f,77.f / 1000.f });

	return S_OK;
}

HRESULT CSwan::PreviousTick(_float fTimeDelta)
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

HRESULT CSwan::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;
		
		if (m_eNpcState == STATE_DIALOGUE)
		{
			m_pInteractionBox->Tick(fTimeDelta);
			Execute_Dialogue();
		}
		else
		{

#pragma region OHD Placement

		_float2 vPosition2D;
		_vector vPos;
		_matrix ProjMatrix, ViewMatrix, WorldMatrix;
		ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
		ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += 2.5f;

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
		m_pOverHeadUI->SetTextureUV({ 474.f / 1500.f,638.f / 1000.f }, { 65.f / 1500.f,68.f / 1000.f });
#pragma endregion
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));

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
			XMVector3Length(vToPlayer).m128_f32[0] <= 3.f)
		{
			CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);

			m_pInteractionBox->Tick(fTimeDelta);

			if (m_pGameInstance->Key_Down('F'))
			{
				if (m_pInteractionBox->Get_SelectedIndex() == 0)
				{
					m_pInteractionBox->Clear_Boxes();
					Begin_Dialogue();
					m_eNpcState = STATE_DIALOGUE;
					m_bLookPlayer = TRUE;
				}
				if (m_bGreet == FALSE)
				{
					m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_SOLIDER_START);
					m_bGreet = TRUE;
				}
				if ((m_iCameraScene % 2) == 0)
				{
					if (pCamera != nullptr)
					{
						pCamera->Set_SceneMode(CAMSCENEID_BASICTALK, SCENE_ONE, 0.f, 10.f);
						m_iCameraScene++;
					}
				}
			}
		}
		if (pCamera != nullptr && pCamera->End_Scene(CAMSCENEID_BASICTALK) == TRUE) /* Scene 끝나면 원래카메라 */
		{
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
		}
	}
		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}
	}

	if (m_bLookPlayer == TRUE)
	{
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
	else
	{
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(m_vInitLookDir, 13.f);
	}

	return S_OK;
}

HRESULT CSwan::LateTick(_float fTimeDelta)
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
		if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0 &&
			XMVector3Length(vToPlayer).m128_f32[0] <= 3.f)
		{
			if (m_eNpcState != STATE_DIALOGUE)
			{
				m_pOverHeadUI->LateTick(fTimeDelta);
			}
			m_pInteractionBox->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CSwan::Render()
{
	return S_OK;
}

CSwan* CSwan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwan* pInstance = new CSwan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSwan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwan::Clone(void* pArg)
{
	CSwan* pInstance = new CSwan(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSwan"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwan::Free()
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

	Safe_Release(m_pInteractionBox);
	Safe_Release(m_pOverHeadUI);
}