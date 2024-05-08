#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Item.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CSelRect : public COrtho
{

public:
	enum SELRECT_TPYE  { SELRECT_PLAIN, SELRECT_CONSUMABLE, SELRECT_SLIDER,SELRECT_EQUIP, SELRECT_SHOP, SELRECT_ALCHEMY, SELRECT_TPYE_END};
	enum SELRECT_COLOR { SELRECT_NAVY ,SELRECT_IVORY,SELRECT_END};


private:
	CSelRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSelRect(const CSelRect& rhs);
	virtual ~CSelRect() = default;

private:
	_int					m_iMOverIndex = { -1 };
	_int					m_iMOverIndexChar = { -1 };
	_int					m_iSelectedIdx = { -1 };
	_int					m_iSelectedQty = {1};
	_int					m_iSelectedCharIdx = {0};

	_int					m_iOwnedQty = {};

	wstring					m_strText = {};

	_vector					m_vColor;
	_vector					m_vTextColor;

	vector<_float4x4>		m_vecSelRect;
	vector<_float4x4>		m_vecButton;
	vector<_float4x4>		m_vecPortrait;

	SELRECT_TPYE			m_eSelRectType = { SELRECT_TPYE_END };
	SELRECT_COLOR			m_eSelRectColor = { SELRECT_END };

	CItem*					m_pDisplayItem = { nullptr };

	CTexture*				m_pButtonTex = { nullptr };
	CTexture*				m_pRarityTexCom = { nullptr };
	CTexture*				m_pPortraitTexCom = { nullptr };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_int	Get_SelectedIdx() { return m_iSelectedIdx; }
	_int	Get_SelectedCharIdx() { return m_iSelectedCharIdx; }
	_int	Get_SelectedQty() { return m_iSelectedQty;}
	void	Reset_Box() {m_iSelectedIdx = -1; m_iSelectedQty = 1; m_iSelectedCharIdx = 0; m_pDisplayItem = nullptr;}
	void	Set_BoxText(wstring strText) {m_strText = strText;}	
	void	Set_SelRectColor(_int iSelrectIdx);
	void	Set_SelRectType(SELRECT_TPYE eType);
	void	Set_OwnedQty(_int iQty) { m_iOwnedQty = iQty; }

	void	Set_DisplayItem(CItem* pItem) {m_pDisplayItem = pItem;}

public:
	static	CSelRect*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;


};

END