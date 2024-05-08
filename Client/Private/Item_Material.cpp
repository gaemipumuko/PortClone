#include "stdafx.h"
#include "Item_Material.h"

#include "GameInstance.h"

CItem_Material::CItem_Material(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CItem_Material::CItem_Material(const CItem_Material& rhs)
	: CItem(rhs)

{
}

HRESULT CItem_Material::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Material::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	m_eItemType = CItem::ITEM_TYPE_EXPBOOK;

	m_eMaterialType =
		static_cast<MATERIAL_TYPE>(m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "subcategory"}));

	m_iExp = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "attack"});
	m_iPrice = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "Price"});

	return S_OK;
}

HRESULT CItem_Material::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Material::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Material::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Material::Render(_float2 fPos,_float2 vRenderScale)
{
	if (FAILED(__super::Render(fPos, vRenderScale)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Material::Render_Detail(_float4x4 vItemMatrix)
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

	m_pGameInstance->Render_Font(L"Font_Default", L"기타 아이템", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);

	//Item Detailed Info
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemInfo, _float2(1115.f, 350.f), .5f, { 0.27450980392f,0.31372549019f,0.39215686274f,1.f}, 0.f, { 0.f,0.f }, false);

	return S_OK;
}

void CItem_Material::Use_item(_uint iQty, PLAYERINDEX ePlayerIndex)
{
	m_iQuantity -= iQty;
}


CItem_Material* CItem_Material::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Material* pInstance = new CItem_Material(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CItem_Material"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Material::Clone(void* pArg)
{
	CItem_Material* pInstance = new CItem_Material(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CItem_Material"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Material::Free()
{
	__super::Free();
}
