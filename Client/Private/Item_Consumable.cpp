#include "stdafx.h"
#include "Item_Consumable.h"

#include "GameInstance.h"
#include "Player.h"
#include "Part_Status.h"

CItem_Consumable::CItem_Consumable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CItem_Consumable::CItem_Consumable(const CItem_Consumable& rhs)
	: CItem(rhs)

{
}

HRESULT CItem_Consumable::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Consumable::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	m_eItemType = CItem::ITEM_TYPE_COOKING;

	m_eConsumType =
		static_cast<CONSUM_TYPE>(m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "subcategory"}));

	m_iBaseDamage = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "attack"});
	m_iHp = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "hitpoint"});
	m_fCritRate = m_pGameInstance->Find_JsonFloatValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "CR"});
	m_fCritDmg = m_pGameInstance->Find_JsonFloatValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "CD"});
	m_iPrice = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "Price"});

	return S_OK;
}

HRESULT CItem_Consumable::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Consumable::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Consumable::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Consumable::Render(_float2 fPos, _float2 vRenderScale)
{
	if (FAILED(__super::Render(fPos, vRenderScale)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Consumable::Render_Detail(_float4x4 vItemMatrix)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", vItemMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	_int	iValue = 0;

	//Item Name
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemName, _float2(1115.f, 105.f), 0.7f, Colors::White, 0.f, { 0.f,0.f }, false);

	//Item SubCategory
	m_pGameInstance->Render_Font(L"Font_Default", L"À½½Ä", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

	//Item Detailed Info
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemInfo, _float2(1115.f, 350.f), .5f, { 0.27450980392f,0.31372549019f,0.39215686274f,1.f}, 0.f, { 0.f,0.f }, false);


	return S_OK;
}

void CItem_Consumable::Use_item(_uint iQty, PLAYERINDEX ePlayerIndex)
{
	if (m_iQuantity >= iQty)
	{
		m_iQuantity -= iQty;

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		if (pPlayer == nullptr)
		{
			MSG_BOX(TEXT("Failed to Find : Player"));
			return;
		}

		if(ePlayerIndex != PLAYER_END)
		pPlayer->Set_Heal(ePlayerIndex, (_float)(m_iHp* iQty));
		//CPart_Status* pStatus = static_cast<CPart_Status*>(static_cast<CPlayer*>(pPlayer)->Get_PartObject(CPlayer::PART_STATUS, ePlayerIndex));
		//pStatus->Set_Heal(m_iHp);
		//pPlayer->Update_CurrentHp(ePlayerIndex);

	}
}


CItem_Consumable* CItem_Consumable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Consumable* pInstance = new CItem_Consumable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CItem_Consumable"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Consumable::Clone(void* pArg)
{
	CItem_Consumable* pInstance = new CItem_Consumable(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CItem_Consumable"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Consumable::Free()
{
	__super::Free();
}
