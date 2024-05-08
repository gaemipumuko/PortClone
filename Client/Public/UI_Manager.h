#pragma once
#include "Client_Defines.h"
#include "DeciderObject.h"

#include "InteractionBox.h"
BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CUI_Manager : public CDeciderObject
{

private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Manager(const CUI_Manager& rhs);
	virtual ~CUI_Manager() = default;

private:
	class CRenderer*		m_pRendererCom = { nullptr };

private:
	_int					m_iNumInteraction = { 0 };
	_int					m_iSelectedBoxIdx = {};
	UI_PAGE					m_eCurrentPage = { PAGE_END };
	
private:
	vector<class COrtho*>		m_vec_UI_Pages;
	list<class CDamage_Font*>	m_DamageFontList;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual HRESULT PreviousTick(_float fTimeDelta);
	virtual HRESULT Tick(_float fTimeDelta);
	virtual HRESULT LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Add_UI_Object(UI_PAGE iPageIndex, class COrtho* pGameObject);
	HRESULT Close_Page(UI_PAGE iPageIndex);
	HRESULT Show_Page(UI_PAGE iPageIndex);

	class COrtho* Get_CurrentPage(){ return m_vec_UI_Pages[m_eCurrentPage]; }
	class COrtho* Get_UI_Page(UI_PAGE ePage) { return m_vec_UI_Pages[ePage]; }
	UI_PAGE		  GetCurrentPageName() {return m_eCurrentPage;}

	void	Add_DamageFonts(class CDamage_Font* DamageFont);

	/*For Interaction Boxes*/
	_int	Get_SelectedBoxIdx() { return m_iSelectedBoxIdx; }
	_int	Get_NumBox_Registered() { return m_iNumInteraction; }
	_int	Register_InteractionBox(_int iNumbox);

public:
	static CUI_Manager*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END