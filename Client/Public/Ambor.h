#pragma once

#include "Client_Defines.h"
#include "Creature.h"
#include "Gliding_Test.h"
#include "Dialogue_Page.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CAmbor final : public CCreature
{
private:
	CAmbor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAmbor(const CAmbor& rhs);
	virtual ~CAmbor() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	typedef struct tagPosDesc
	{
		_vector		vAmborPos = {};
		_float		fRadian = {};
	} POS_DESC;

	enum PART { PART_BODY, PART_END };
	enum DIALOGUE_STAGE {DIALOGUE_INITIAL,DIALOGUE_PLAZA,DIALOGUE_TEST,DIALOGUE_RETRY,DIALOGUE_PASSED,DIALOGUE_END};


	vector<PARTY_SLOTS>	m_Parts;

private:
	class CAlphaRect*		m_pOverHeadUI = { nullptr };
	class CInteraction_Box* m_pInteractionBox = { nullptr };

	DIALOGUE_STAGE						m_eDialogue_Stage = { DIALOGUE_INITIAL};
	/* 대화창 제어 관련*/
	_bool								m_bConversation = {false};
	_bool								m_bDialogueEnd = {false};
	wstring								m_strNPCName = {  };
	wstring								m_strNPCDetail = {};

	vector<wstring>						m_NPCScript;
	vector<wstring>						m_NPCScript1;
	vector<wstring>						m_NPCScript2;
	vector<wstring>						m_NPCTryAgainScript;
	vector<wstring>						m_NPCClearedScript;

	vector<wstring>						m_NPCChoice;
	vector<wstring>						m_NPCScriptDisplayed;

	vector<CDialogue_Page::BREAK_DESC>  m_BreakPoints;
	vector<CDialogue_Page::BREAK_DESC>  m_CurrentBreakPoints;

	_int								m_iParagraphIndex = { 0 };
	_int								m_iCurrentBreakPt = { -1 };
	_int								m_iBreakPointPg = { -1 };
	_int								m_iNumChoice = { 0 };
	_int								m_iCurrentChoiceIdx = { 0 };
	_int								m_iTerminatorPg = { -1 };

private:
	_bool					m_bLookPlayer = { FALSE };
	_bool					m_bRotation = { FALSE };
	vector<POS_DESC>		AmborInfo;

private:
	_int					m_iCameraScene = {};
	_bool					m_bSceneEnd = { FALSE };

private:
	CGliding_Test*			m_pGliding_Test = { nullptr };
	_bool					m_bTestClear = { FALSE };

private:
	_bool					m_bItem = { FALSE };

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT Ready_Script();
	void Begin_Dialogue();
	void Execute_Dialogue();
	void End_Dialogue();
	void Next_Dialogue();

private:
	HRESULT Set_CameraScene();
	HRESULT Init_DialogueScene();
	HRESULT Plaza_DialogueScene();
	HRESULT Test_DialogueScene();
	HRESULT Retry_DialogueScene();
	HRESULT Passed_DialogueScene();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CAmbor*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END