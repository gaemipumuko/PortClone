#include "stdafx.h"
#include "Item.h"

#include "Item_Weapon.h"
#include "Item_Artifact.h"
#include "Item_Consumable.h"
#include "Item_Material.h"

#include "GameInstance.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
    : COrtho(rhs)
{

}

HRESULT CItem::Add_Component(void* pArg)
{
	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
		TEXT("Com_Texture_Item"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(pDesc->tTextureGrade.iComponentLevel, pDesc->tTextureGrade.strComponentTag,
		TEXT("Com_Texture_Grade"), (CComponent**)&m_pTextureGradeCom)))
		return E_FAIL;

	return S_OK;
}



void CItem::Set_Current_Texture(_int iIndex)
{
	m_pTextureCom->Set_CurrentTexture((_float)iIndex);
}

void CItem::Set_Grade(ITEM_RARE eGrade)
{
	m_eItemRare = eGrade;
	m_pTextureGradeCom->Set_CurrentTexture(static_cast<_float>(m_eItemRare));
}

HRESULT CItem::Render_Icon(_float2 vRenderPos, _float2 vRenderScale)
{
	_float4x4 CombinedMatrix = { };
	XMStoreFloat4x4(&CombinedMatrix, m_pTransformCom->Get_WorldMatrix());

	CombinedMatrix._11 = vRenderScale.x;
	CombinedMatrix._22 = vRenderScale.y;
	CombinedMatrix._41 = vRenderPos.x;
	CombinedMatrix._42 = vRenderPos.y;

	//CombinedMatrix._41 = -static_cast<_float>(g_iWinSizeX) * 0.408f + iSlotIndex % 8 * (CombinedMatrix._11 + static_cast<_float>(g_iWinSizeX) * 0.014f);
	//CombinedMatrix._42 = static_cast<_float>(g_iWinSizeY) * 0.32f + iSlotIndex / 8 * (CombinedMatrix._22 + static_cast<_float>(g_iWinSizeY) * 0.02f) * -1.f
	//	+ fScrollView * 10.f;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Initialize_Clone(void* pArg)
{
    ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(&pDesc->tOrthoDesc)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

	m_strItemName = pDesc->strItemName;
	m_strItemInfo = pDesc->strItemInfo;
	m_iItemID = pDesc->iItemIndex;
    m_pTextureCom->Set_CurrentTexture(static_cast<_float>(pDesc->iItemIndex));
    m_pTextureGradeCom->Set_CurrentTexture(static_cast<_float>(pDesc->iItemGrade));

	m_eItemRare = static_cast<ITEM_RARE>(pDesc->iItemGrade);
	m_iQuantity = pDesc->iItemQty;

    return S_OK;
}

HRESULT CItem::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CItem::Tick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CItem::LateTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CItem::Render(_float2 fRenderPos, _float2 vRenderScale)
{
	_float4x4 CombinedMatrix = { };
	XMStoreFloat4x4(&CombinedMatrix, m_pTransformCom->Get_WorldMatrix());

	CombinedMatrix._11 = vRenderScale.x;
	CombinedMatrix._22 = vRenderScale.y;
	CombinedMatrix._41 = fRenderPos.x;
	CombinedMatrix._42 = fRenderPos.y;
	 
	//CombinedMatrix._41 = -static_cast<_float>(g_iWinSizeX) * 0.408f + iSlotIndex % 8 * (CombinedMatrix._11 + static_cast<_float>(g_iWinSizeX) * 0.014f);
	//CombinedMatrix._42 = static_cast<_float>(g_iWinSizeY) * 0.32f + iSlotIndex / 8 * (CombinedMatrix._22 + static_cast<_float>(g_iWinSizeY) * 0.02f) * -1.f
	//	+ fScrollView * 10.f;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
		return E_FAIL;

	/* Grade */
	if (FAILED(m_pTextureGradeCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(UI_ITEM)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	/* Item */
	if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(UI_ITEM)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	/* Bar */
	if (FAILED(m_pTextureGradeCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",5)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(UI_ITEM)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	//Item Quantity Info
	if (m_eItemType == ITEM_TYPE_EXPBOOK || m_eItemType == ITEM_TYPE_COOKING)
	{
		if (m_iQuantity > 1)
		{

			_int iNumDigits = (int)log10(m_iQuantity)+1;

			_tchar szQty[MAX_PATH];
			wsprintf(szQty, TEXT("%d"), m_iQuantity);
			m_pGameInstance->Render_Font(L"Font_Default", szQty, _float2(CombinedMatrix._41-7.f* iNumDigits + g_iWinSizeX/2, -(CombinedMatrix._42 - 42.f - g_iWinSizeY/2)), .5f, { 0.27450980392f,0.31372549019f,0.39215686274f,1.f }, 0.f, { 0.f,0.f }, false);
		}
	}

	return S_OK;
}

CItem* CItem::Create_Item(_int iItemIndex, _int iQuantity)
{
	CItem* pItem = { nullptr };

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	_int iItemType = pInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(iItemIndex), "category"});
	if (iItemType == INT_MIN || iItemType < 0 || iItemType > 3)
	{
		RELEASE_INSTANCE(CGameInstance);
		return pItem;
	}
	CItem::ITEM_DESC tItemDesc = { };
	tItemDesc.tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tItemDesc.tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_Item_Texture");
	tItemDesc.tOrthoDesc.fSize = { 1.f, 1.f };
	
	tItemDesc.tTextureGrade.iComponentLevel = LEVEL_STATIC;
	tItemDesc.tTextureGrade.strComponentTag = TEXT("Prototype_Component_Texture_Item_Rarity");
	
	tItemDesc.iItemIndex = iItemIndex;
	tItemDesc.iItemGrade = pInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(iItemIndex), "grade"});
	
	tItemDesc.strItemName = pInstance->Find_JsonStringValue(L"Item_Info", vector<string>{to_string(iItemIndex), "name"});
	tItemDesc.strItemInfo = pInstance->Find_JsonStringValue(L"Item_Info", vector<string>{to_string(iItemIndex), "description"});

	tItemDesc.iItemQty = iQuantity;

	switch (iItemType)
	{
	case ITEM_TYPE_WEAPON:
		pItem = static_cast<CItem_Weapon*>(pInstance->Find_Object_Prototype(L"Prototype_GameObject_Item_Weapon")->Clone(&tItemDesc));
		break;
	case ITEM_TYPE_ARTIFACT:
		pItem = static_cast<CItem_Artifact*>(pInstance->Find_Object_Prototype(L"Prototype_GameObject_Item_Artifact")->Clone(&tItemDesc));
		break;
	case ITEM_TYPE_EXPBOOK:
		pItem = static_cast<CItem_Material*>(pInstance->Find_Object_Prototype(L"Prototype_GameObject_Item_Material")->Clone(&tItemDesc));
		break;
	case ITEM_TYPE_COOKING:
		pItem = static_cast<CItem_Consumable*>(pInstance->Find_Object_Prototype(L"Prototype_GameObject_Item_Consumable")->Clone(&tItemDesc));
		break;
	case ITEM_TYPE_END:
		break;
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return pItem;
}

void CItem::Free()
{
    __super::Free();

	if(m_bCloned)
    Safe_Release(m_pTextureGradeCom);
}
