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

class CSwan final : public CCreature
{
private:
	CSwan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwan(const CSwan& rhs);
	virtual ~CSwan() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	enum PART { PART_BODY, PART_END };

	vector<PARTY_SLOTS>	m_Parts;

private:
	class CAlphaRect*		m_pOverHeadUI = { nullptr };
	class CInteraction_Box* m_pInteractionBox = { nullptr };

	/* 대화창 제어 관련*/
	CDialogue_Page*			m_pDialogue_Page = { nullptr };

	_bool								m_bConversation = {false};
	_bool								m_bDialogueEnd = {false};
	wstring								m_strNPCName = {  };
	wstring								m_strNPCDetail = {};

	vector<wstring>						m_NPCChoice;
	vector<wstring>						m_NPCScript;

	vector<wstring>						m_NPCScriptDisplayed;

	vector<CDialogue_Page::BREAK_DESC>  m_BreakPoints;
	vector<CDialogue_Page::BREAK_DESC>  m_CurrentBreakPoints;

	_int								m_iParagraphIndex = { 0 };
	_int								m_iCurrentBreakPt = { -1 };
	_int								m_iBreakPointPg = { -1 };
	_int								m_iNumChoice = { 0 };
	_int								m_iCurrentChoiceIdx = { 0 };
	_int								m_iTerminatorPg = { -1 };

	NPC_STATE							m_eNpcState = { NPCSTATE_END };

private:
	_bool	m_bGreet = { FALSE };
	_int	m_iCameraScene = {};
	_bool	m_bLookPlayer = { FALSE };

	_vector m_vInitLookDir = {};

	_bool	m_bInitDialogue = { FALSE };

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT Ready_Script();
	void Begin_Dialogue();
	void Execute_Dialogue();
	void End_Dialogue();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CSwan*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END