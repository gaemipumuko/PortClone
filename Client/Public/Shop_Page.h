#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Item.h"
#include "SelRect.h"

BEGIN(Client)

class CShop_Page : public COrtho
{
private:
	CShop_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShop_Page(const CShop_Page& rhs);
	virtual ~CShop_Page() = default;

private:
	CTexture*							m_pButtonTex;
	CSelRect*							m_pSelect_Box = { nullptr };

private:
	_bool								m_bDisplaySelectBox = { false };

	_int								m_iMOverIndex = { -1 };
	_int								m_iMOverSlot = {};

	_int								m_iNumItems = {5};

	_int								m_iMora = {0};

	vector<_float4x4>					m_ButtonMat = {};
	vector<_float4x4>					m_SlotMatrix = { };
	vector<_float4x4>					m_DetailBoxMatrix = {};
	_float4x4							m_DecorMat = {};

	list<class CItem_Consumable*>		m_ItemList = { };


	_int								m_iSelectedButton = {-1};
	//Detail Boxes
	_int								m_iSelectedIndex = {};
	_int								m_iSelectedGrade = {};

	_float2								m_vStripUVPos;
	_float2								m_vStripUVScale;
	_float2								m_vItemBgUVPos;
	_float2								m_vItemBgUVScale;

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
	static CShop_Page*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END