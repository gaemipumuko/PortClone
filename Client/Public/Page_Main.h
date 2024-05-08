#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Main_Party_Buttons.h"
#include "MiniMap.h"
#include "Item.h"
#include "Quest.h"

BEGIN(Client)

class CPage_Main : public COrtho
{
private:
	CPage_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPage_Main(const CPage_Main& rhs);
	virtual ~CPage_Main() = default;

private:
	//CAlphaRect*			m_pHpBase = { nullptr };
	_int				m_iMOverIndex = { -1 };	/* -1은 아무것도 오버되지 않은 상태 */
	vector<_float4x4>	m_RTIconsMatrix;
	vector<_float4x4>	m_vec_ImmobileMatrix;


	CTexture*			m_pBaseTextureCom;
	CTexture*			m_pQuestTextureCom;
	CParty_Buttons*		m_pParty_Buttons;
	CMini_Map*			m_pMiniMap;

	//Overlay Related
	_bool				m_bPresent_Top_Overlay = { false };
	_float				m_fOverlay_DisplayTime = {0.f};
	_vector				m_vOverlayColor;
	wstring				m_strOverlay;

	//Left Overlay Related
	vector<CItem*>		m_Acquired_Items;
	vector<_float4x4>	m_vecLOverlaySlot;
	_bool				m_bPresent_L_Overlay = { false };
	_float				m_fItem_DisplayTime = { 0.f };
	_float				m_fDisplayMargin = { 100.f };
	_int				m_iNumDraw = {0};

	//Region Name
	wstring				m_strRegion = {};
	_bool				m_bRegionNameDisplayed = {};
	_float				m_fFontAlpha = {};
	_float				m_fTimeAcc = {};

	//Flight Test
	vector<_float4x4>	m_vecFlightTestMat;
	_bool				m_bRingFTDisplay = { false };
	_bool				m_bFlightTestDisplay = { false };
	_bool				m_bBoxBlink = {false};
	_float				m_fTime_Remain = {};
	_int				m_iAcquiredRings = {};
	_vector				m_vBoxColor = {};

	//LT Quest HUD
	_bool				m_bQuestUpdated = false;
	_float4x4			m_QuestHUDMat;
	_float				m_fQuestBlinkTimer = {};
	vector<CQuest*>		m_vecQuestDisplayed;


private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();
	void	Display_Acquired(_float fTimedelta);

public:
	_float Get_Stamina() { return m_pParty_Buttons->Get_Stamina(); }

	//For Party Buttons
	void Set_Hp(_int iIndex, _float fHp) { m_pParty_Buttons->Set_Hp(iIndex, fHp); }
	void Set_MaxHp(_int iIndex, _float fHp) { m_pParty_Buttons->Set_MaxHp(iIndex, fHp); }
	void Set_Stamina(_float fStamina) { m_pParty_Buttons->Set_Stamina(fStamina); }
	void Set_InputState(_bool fInputState) { m_pParty_Buttons->Set_InputState(fInputState); }
	void Set_MemberIndex(_int iIndex) { m_pParty_Buttons->Set_MemberIndex(iIndex); }
	void Set_Element_Rate(_float fElementRate) { m_pParty_Buttons->Set_Element_Rate(fElementRate) ; }
	void Set_Skill_CoolDown_Rate(_float fSkillCDR) { m_pParty_Buttons->Set_Skill_CoolDown_Rate(fSkillCDR); }
	void Set_Burst_CoolDown_Rate(_float fBurstCDR) { m_pParty_Buttons->Set_Burst_CoolDown_Rate(fBurstCDR); }
	void Set_BurstReady(PLAYERINDEX eIndex, _bool bReady) { m_pParty_Buttons->Set_BurstReady(eIndex, bReady); }

public:
	void	Register_On_MiniMap(CTransform* pTargetTransform, _int iIconIndex);
	void	Request_Top_Overlay(_vector vColor, wstring strOverlayText);
	_bool	Register_On_AcquiredItems(CItem* pItem);
	void	Display_Region_Name(wstring strRegion);

	void	Display_Timer_UI(_bool bShow) { m_bFlightTestDisplay = bShow; }
	void	Display_Flight_Test_UI(_bool bShow) { m_bFlightTestDisplay = bShow; m_bRingFTDisplay = bShow; }
	void	Update_Timer(_float fTime) {m_fTime_Remain = fTime; }
	void	Update_Acquired_Rings(_int iRings);

	void	Register_On_QuestHUD(CQuest* pQuest);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CPage_Main*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END