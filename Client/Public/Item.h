#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CItem abstract : public COrtho
{
public:
	enum ITEM_TYPE	{ ITEM_TYPE_WEAPON, ITEM_TYPE_ARTIFACT, ITEM_TYPE_EXPBOOK, ITEM_TYPE_COOKING, ITEM_TYPE_END };
	enum ITEM_RARE	{ ITEM_GRADE_COMMON, ITEM_GRADE_UNCOMMON, ITEM_GRADE_RARE, ITEM_GRADE_EPIC, ITEM_GRADE_LEGENDARY, ITEM_GRADE_END };

	typedef struct tagItemDesc
	{
		wstring			strItemName;
		wstring			strItemInfo;
		_int			iItemIndex;
		_int			iItemGrade;
		_int			iItemQty;
		ORTHO_DESC		tOrthoDesc;
		RECEIVED_COM	tTextureGrade;
	} ITEM_DESC;

public:
	bool compareByItemVariable(const CItem* other) const {
		return this->Get_Item_Rarity() > other->Get_Item_Rarity();
	}

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

protected:
	CTexture*	m_pTextureGradeCom = { nullptr };

	wstring		m_strItemName = {};
	wstring		m_strItemInfo = {};

	_int		m_iItemID = {};
	_int		m_iQuantity = {1};
	_uint		m_iPrice = {};
protected:
	ITEM_TYPE	m_eItemType = { ITEM_TYPE_END };
	ITEM_RARE	m_eItemRare = { ITEM_GRADE_END };

protected:
	HRESULT Add_Component(void* pArg);
	
public:
	_int		Get_Item_Rarity()const { return static_cast<_int>(m_eItemRare); }
	ITEM_TYPE	Get_Item_Type()const { return m_eItemType; }

	wstring		Get_Item_Name() { return m_strItemName; }
	void		Stash_Item(_int iItemCount) { m_iQuantity += iItemCount; }
	_int		Get_item_Qty() { return m_iQuantity; }
	_uint		Get_Item_Price() { return m_iPrice; }
	_int		Get_Item_ID() {return m_iItemID;}
	
	//Shop & Alchemy & Cooking Use
	void		Set_Current_Texture(_int iIndex);
	void		Set_Grade(ITEM_RARE eGrade);
	void		Set_Item_Qty(_int iQty) { m_iQuantity = iQty; }

	HRESULT		Render_Icon(_float2 vRenderPos, _float2 vRenderScale);
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render(_float2 fRenderPos, _float2 vRenderScale = _float2(100.f, 128.f));
	
	virtual void	Use_item(_uint iQty, PLAYERINDEX ePlayerIndex = PLAYER_END) PURE;
public:
	static CItem*			Create_Item(_int iItemIndex,_int iQuantity = 1);
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END