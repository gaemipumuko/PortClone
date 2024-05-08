#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Hp_Bar.h"
#include "Stamina_Bar.h"

BEGIN(Client)

class CParty_Buttons : public COrtho
{
private:
	CParty_Buttons(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParty_Buttons(const CParty_Buttons& rhs);
	virtual ~CParty_Buttons() = default;

private:
	//CAlphaRect*			m_pHpBase = { nullptr };
	_bool				m_bFadeOut = { true };
	_bool				m_bTagEnabled = { true };
	_bool				m_bBurstReady[4] = { false,false,false,false };

	_int				m_iMOverIndex = { -1 };	/* -1은 아무것도 오버되지 않은 상태 */
	_int				m_iMemberIndex = { 0 };
	_int				m_iNumMembers = { 4 };
	_int				m_iMemberID[4] = {2,6,1,4};

	_float				m_fStaminaAlpha = {0.3f};
	_float				m_fElementRate = { 0.f };
	_float				m_fCoolDownRate = {1.f};
	_float				m_fBurstDownRate = { 1.f };
	_float				m_fTimeAcc = {};

	vector<_float4x4>	m_ButtonBaseMat;
	vector<_float4x4>	m_IconMat;
	vector<_float4x4>	m_ImmobileMat;
	vector<_float4x4>	m_HealthMat;
	vector<_float4x4>	m_StaminaMat;
	vector<_float4x4>	m_BurstSCMat;

	_vector				m_vElementColor[4] = {};

	CTexture*			m_pIconTextureCom;
	CTexture*			m_pBurstTextureCom;
	CTexture*			m_pSkillTextureCom;
	CTexture*			m_pStaminaTextureCom;

	CHp_Bar*			m_pHp_Bar[4];
			
	//Temporary Varibles That is Needed to Replaced by Actual Numbers

	_float				m_fTempCurrentStamina = { 100.f };
	_float				m_fTempTotalStamina = { 100.f };
	_float				m_fTempHp = { 1500.f };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	_float Get_Stamina() { return m_fTempCurrentStamina; }

	void Set_Hp(_int iIndex, _float fHp) { m_pHp_Bar[iIndex]->Set_CurrentHp(fHp); }
	void Set_MaxHp(_int iIndex, _float fHp) { m_pHp_Bar[iIndex]->Set_MaxHp(fHp); }
	void Set_Stamina(_float fStamina) { m_fTempCurrentStamina = fStamina; }
	void Set_InputState(_bool bInput) { m_bTagEnabled = bInput; }
	void Set_MemberIndex(_int iIndex) { m_iMemberIndex = iIndex; }
	void Set_Element_Rate(_float fElementRate) { m_fElementRate = fElementRate; }
	void Set_Skill_CoolDown_Rate(_float fSkillCDR) { m_fCoolDownRate = fSkillCDR; }
	void Set_Burst_CoolDown_Rate(_float fBurstCDR) { m_fBurstDownRate = fBurstCDR; }
	void Set_BurstReady(PLAYERINDEX eIndex, _bool bReady) { m_bBurstReady[eIndex] = bReady; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CParty_Buttons*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END