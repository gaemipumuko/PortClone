#pragma once

#include "Client_Defines.h"
#include "Ortho.h"


BEGIN(Client)

class CInteraction_Box : public COrtho
{
public:
	enum INTERACTION_ICON { ICON_TALK, ICON_END };

	typedef struct tagInteractionBox
	{
		wstring				strBoxText;
		INTERACTION_ICON	eBoxIcon;

	}INTERACTION_DESC;

private:
	CInteraction_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Box(const CInteraction_Box& rhs);
	virtual ~CInteraction_Box() = default;

private:
	_bool					m_bSelected = {};

	_int					m_iSelectedIndex = { 0 };
	_int					m_iNumBoxes = { 0 };
	_int					m_iIndexOnScreen = {0};
	_int					m_iNumBoxOnScreen = {};

	_float					m_fSinRepeat;
	_float					m_fTimeAcc = { 0.f };
	_float					m_fFontAlpha = {0.f};
	_float					m_fDisplayMargin = { 0.f };

	vector<_float4x4>		m_vecBoxMat;

	list<INTERACTION_DESC>	m_BoxDescList;

	wstring					m_strBox0 = {};
	wstring					m_strBox1 = {};
	wstring					m_strBox2 = {};
	wstring					m_strBox3 = {};
	wstring					m_strBox4 = {};
	wstring					m_strBox5 = {};
		
	INTERACTION_ICON		m_eIconBox0 = { ICON_END };
	INTERACTION_ICON		m_eIconBox1 = { ICON_END };
	INTERACTION_ICON		m_eIconBox2 = { ICON_END };
	INTERACTION_ICON		m_eIconBox3 = { ICON_END };
	INTERACTION_ICON		m_eIconBox4 = { ICON_END };
	INTERACTION_ICON		m_eIconBox5 = { ICON_END };

	CTexture*				m_pInteractionIconCom = {nullptr};
	class CUI_Manager*		m_pUI_Manager = { nullptr };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();


public:
	_int	Get_NumBoxes() { return m_iNumBoxes; }
	void	Clear_Boxes();

	void	Add_InteractionBox(wstring strInteractionBox, INTERACTION_ICON eIconType);
	void	Set_SelectedIndex(_uint iSelectedIndex) { m_iSelectedIndex = iSelectedIndex; }
	_int	Get_SelectedIndex();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CInteraction_Box*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;


};

END