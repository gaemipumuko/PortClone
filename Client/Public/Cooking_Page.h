#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Item.h"
#include "SelRect.h"


BEGIN(Client)

class CCooking_Page : public COrtho
{
private:
	enum GAME_RESULT {GAME_FAILED,GAME_GOOD,GAME_PERFECT,GAME_END};

private:
	CCooking_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCooking_Page(const CCooking_Page& rhs);
	virtual ~CCooking_Page() = default;

private:
	CTexture*						m_pButtonTex;
	CTexture*						m_pBaseTextureCom;
	CSelRect*						m_pSelect_Box = { nullptr };

private:
	_bool							m_bDisplayMiniGame = { false };

	_float							m_fButtonPress = {};

	_int							m_iMOverIndex = { -1 };
	_int							m_iMOverSlot = {};

	_int							m_iNumItems = {12};

	_int							m_iMora = {};

	vector<_float4x4>				m_ButtonMat = {};
	vector<_float4x4>				m_SlotMatrix = { };
	vector<_float4x4>				m_DecorMat = {};
	_float4x4						m_DisplayMat = {};

	list<class CItem_Consumable*>	m_ItemList = { };
	vector<class CItem_Consumable*>	m_VecMaterials = { };

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

	_int							m_iMtrlID2 = {};
	_int							m_iMtrlQty2 = { 0 };
	_int							m_iMtrl2Owned = { 0 };

	_int							m_iNumMaterials = {1};
	_int							m_AlchemyCost = {};

	//MiniGame
	_bool							m_bPlayMiniGame = { false };
	_bool							m_bShowResult = {false};

	GAME_RESULT						m_eGameResult = { GAME_END };

	_matrix							m_SpoonMat = {};
	_float							m_fMiniGameTimer = {};

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
	static CCooking_Page*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END