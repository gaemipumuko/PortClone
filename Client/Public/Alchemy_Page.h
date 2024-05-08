#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Item.h"
#include "SelRect.h"


BEGIN(Client)

class CAlchemy_Page : public COrtho
{
private:
	CAlchemy_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlchemy_Page(const CAlchemy_Page& rhs);
	virtual ~CAlchemy_Page() = default;

private:
	CTexture*						m_pButtonTex;
	CSelRect*						m_pSelect_Box = { nullptr };

private:
	_bool							m_bDisplaySelectBox = { false };

	_int							m_iMOverIndex = { -1 };
	_int							m_iMOverSlot = {};

	_int							m_iNumItems = {13};

	_int							m_iMora = {};

	vector<_float4x4>				m_ButtonMat = {};
	vector<_float4x4>				m_SlotMatrix = { };
	vector<_float4x4>				m_DecorMat = {};
	_float4x4						m_DisplayMat = {};

	list<class CItem_Material*>		m_ItemList = { };
	vector<class CItem_Material*>	m_VecMaterials = { };

	CItem*							m_pSelectedItem = { nullptr };

	_int							m_iSelectedButton = {-1};
	//Detail Boxes
	_int							m_iSelectedIndex = {};
	

	//Combine
	_int							m_iCombineQty= {1};

	_int							m_iMtrlID0 = {};
	_int							m_iMtrlQty0 = {1};
	_int							m_iMtrl0Owned = {0};
	
	_int							m_iMtrlID1 = {};
	_int							m_iMtrlQty1 = { 0 };
	_int							m_iMtrl1Owned = {0};

	_int							m_iNumMaterials = {1};
	_int							m_AlchemyCost = {};

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

private:
	void Ready_Combination(_int iItemID);

public:
	static CAlchemy_Page*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END