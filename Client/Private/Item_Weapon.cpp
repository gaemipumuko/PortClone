#include "stdafx.h"
#include "Item_Weapon.h"

#include "GameInstance.h"

CItem_Weapon::CItem_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CItem_Weapon::CItem_Weapon(const CItem_Weapon& rhs)
	: CItem(rhs)

{
}

HRESULT CItem_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Weapon::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	m_eItemType = CItem::ITEM_TYPE_WEAPON;

	m_eWeaponType =
		static_cast<WEAPON_TYPE>(m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "subcategory"}));

	m_iBaseDamage = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "attack"});

	return S_OK;
}

HRESULT CItem_Weapon::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Weapon::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Weapon::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Weapon::Render(_float2 fPos, _float2 vRenderScale)
{
	if (FAILED(__super::Render(fPos, vRenderScale)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Weapon::Render_Detail(_float4x4 vItemMatrix)
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
	_tchar szDamage[MAX_PATH];
	//Item Name
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemName, _float2(1115.f, 105.f), 0.7f, Colors::White, 0.f, { 0.f,0.f }, false);

	//Item SubCategory
	switch (m_eWeaponType)
	{
	case Client::CItem_Weapon::WTYPE_SWORD:
		m_pGameInstance->Render_Font(L"Font_Default", L"한손검", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Weapon::WTYPE_CLAYMORE:
		m_pGameInstance->Render_Font(L"Font_Default", L"대검", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Weapon::WTYPE_POLEARM:
		m_pGameInstance->Render_Font(L"Font_Default", L"도끼창", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Weapon::WTYPE_CATALYST:
		m_pGameInstance->Render_Font(L"Font_Default", L"법구", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Weapon::WTYPE_BOW:
		m_pGameInstance->Render_Font(L"Font_Default", L"활", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Weapon::WTYPE_END:
		break;
	default:
		break;
	}
	//Item Primary_Attribute
	m_pGameInstance->Render_Font(L"Font_Default", L"기초 공격력", _float2(1115.f, 220.f), 0.5f, {1.f, 1.f, 1.f, 0.5f}, 0.f, { 0.f,0.f }, false);

	//Item Primary_Attribute_Value
	wsprintf(szDamage, TEXT("%d"), m_iBaseDamage);
	m_pGameInstance->Render_Font(L"Font_Number", szDamage, _float2(1115.f, 243.f), 1.f, { 1.f, 1.f, 1.f, 1.f }, 0.f, { 0.f,0.f }, false);

	//Item Detailed Info
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemInfo, _float2(1115.f, 350.f), .5f, { 0.27450980392f,0.31372549019f,0.39215686274f,1.f}, 0.f, { 0.f,0.f }, false);

	return S_OK;
}

void CItem_Weapon::Use_item(_uint iQty, PLAYERINDEX ePlayerIndex)
{
}


CItem_Weapon* CItem_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Weapon* pInstance = new CItem_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CItem_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Weapon::Clone(void* pArg)
{
	CItem_Weapon* pInstance = new CItem_Weapon(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CItem_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Weapon::Free()
{
	__super::Free();
}
