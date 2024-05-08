#pragma once

#include "Client_Defines.h"
#include "Creature.h"
#include "Dialogue_Page.h"
//Test

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CKatheryne final : public CCreature
{
private:
	enum QUEST_STATE {STATE_BEGIN,STATE_DUNGEON_CLEARED,STATE_ANDRIUS_CLEARED, STATE_EFFIGY_CLEARED, STATE_DVALIN_CLEARED, STATE_FINAL_STAGE,STATE_END};
private:
	CKatheryne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKatheryne(const CKatheryne& rhs);
	virtual ~CKatheryne() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	_byte				m_byPartyIndex = { };	/* 현재 선택된 캐릭터 인덱스 */
	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };

private:
	_bool								m_bMoveRun = { FALSE };
	_bool								m_bDialogueEnd = { false };

	_int								m_iParagraphIndex = {0};
	_int								m_iCurrentBreakPt = { -1 };
	_int								m_iBreakPointPg = { -1 };
	_int								m_iNumChoice = {0};
	_int								m_iCurrentChoiceIdx = {0};
	_int								m_iTerminatorPg = { -1 };
	_float								m_fAttackReadyTime = { 0.f };

	wstring								m_strNPCName = {  };
	wstring								m_strNPCDetail = {};

	FSM_STATE							m_ePrevState = { FSM_END };
	NPC_STATE							m_eNpcState = { NPCSTATE_END };
	QUEST_STATE							m_eQuestState = { STATE_BEGIN };

	vector<wstring>						m_NPCScriptDisplayed;

	vector<wstring>						m_NPCScript;
	vector<wstring>						m_NPCScript2;
	vector<wstring>						m_NPCScript3;
	vector<wstring>						m_NPCScript4;
	vector<wstring>						m_NPCScript5;
	vector<wstring>						m_NPCScript6;

	vector<wstring>						m_NPCChoice;

	vector<CDialogue_Page::BREAK_DESC>  m_BreakPoints;
	vector<CDialogue_Page::BREAK_DESC>  m_CurrentBreakPoints;

	CDialogue_Page*						m_pDialogue_Page = {nullptr};
	
	class CKill_Quest*					m_pKillQuest;
	class CKill_Quest*					m_pKillQuest1;
	class CKill_Quest*					m_pKillQuest2;
	class CKill_Quest*					m_pKillQuest3;

	//Test
	class CAlphaRect*					m_pOverHeadUI = {nullptr};
	class CInteraction_Box*				m_pInteractionBox = { nullptr };

private:
	class CCharacterController* m_pController = { nullptr };

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT Ready_Script();
	void Begin_Dialogue();
	void Execute_Dialogue();
	void End_Dialogue();


public:
	//class CPartObject* Get_PartObject(PART eType, _int iIndex) { return m_Parts[eType].pPartySlot[iIndex]; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CKatheryne*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END