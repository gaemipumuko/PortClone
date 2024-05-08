#include "stdafx.h"
#include "Item_Artifact.h"

#include "GameInstance.h"

CItem_Artifact::CItem_Artifact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CItem_Artifact::CItem_Artifact(const CItem_Artifact& rhs)
	: CItem(rhs)

{
}

HRESULT CItem_Artifact::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Artifact::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	m_eItemType = CItem::ITEM_TYPE_ARTIFACT;

	m_eArtifactType =
		static_cast<ARTIFACT_TYPE>(m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "subcategory"}));


	switch (m_eArtifactType)
	{

	case Client::CItem_Artifact::ATYPE_FLOWER:
		m_iHitPoint = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "hitpoint"});
		break;
	case Client::CItem_Artifact::ATYPE_FEATHER:
		m_iBaseDamage = m_pGameInstance->Find_JsonIntValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "attack"});
		break;
	case Client::CItem_Artifact::ATYPE_WATCH:
		m_fDefenceRate = m_pGameInstance->Find_JsonFloatValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "defence"});
		break;
	case Client::CItem_Artifact::ATYPE_GRAIL:
		m_fCritRate = m_pGameInstance->Find_JsonFloatValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "CR"});
		break;
	case Client::CItem_Artifact::ATYPE_CROWN:
		m_fCritDmg = m_pGameInstance->Find_JsonFloatValue(L"Item_Info", vector<string>{to_string(pDesc->iItemIndex), "CD"});
		break;
	case Client::CItem_Artifact::ATYPE_END:
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CItem_Artifact::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Artifact::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Artifact::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CItem_Artifact::Render(_float2 fPos, _float2 vRenderScale)
{
	if (FAILED(__super::Render(fPos, vRenderScale)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Artifact::Render_Detail(_float4x4 vItemMatrix)
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
	wstring strPercent;
	//Item Name
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemName, _float2(1115.f, 105.f), 0.7f, Colors::White, 0.f, { 0.f,0.f }, false);

	//Item SubCategory
	switch (m_eArtifactType)
	{

	case Client::CItem_Artifact::ATYPE_FLOWER:
		m_pGameInstance->Render_Font(L"Font_Default", L"생명의 꽃", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_FEATHER:
		m_pGameInstance->Render_Font(L"Font_Default", L"죽음의 깃털", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_WATCH:
		m_pGameInstance->Render_Font(L"Font_Default", L"시간의 모래", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_GRAIL:
		m_pGameInstance->Render_Font(L"Font_Default", L"공간의 성배", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_CROWN:
		m_pGameInstance->Render_Font(L"Font_Default", L"이성의 왕관", _float2(1115.f, 155.f), 0.5f, Colors::White, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_END:
		break;
	default:
		break;
	}

	wstring strDefPercent = to_wstring(m_fDefenceRate);
	size_t decimalPointPos = strDefPercent.find(L'.');
	if (decimalPointPos != std::wstring::npos && decimalPointPos + 2 < strDefPercent.size()) {
		strDefPercent.erase(decimalPointPos + 2);
	}
	strDefPercent += L"%";

	wstring strCRPercent = to_wstring(m_fCritRate);
	decimalPointPos = strCRPercent.find(L'.');
	if (decimalPointPos != std::wstring::npos && decimalPointPos + 2 < strCRPercent.size()) {
		strCRPercent.erase(decimalPointPos + 2);
	}
	strCRPercent += L"%";

	wstring strCDPercent = to_wstring(m_fCritDmg);
	decimalPointPos = strCDPercent.find(L'.');
	if (decimalPointPos != std::wstring::npos && decimalPointPos + 2 < strCDPercent.size()) {
		strCDPercent.erase(decimalPointPos + 2);
	}
	strCDPercent += L"%";

	switch (m_eArtifactType)
	{

	case Client::CItem_Artifact::ATYPE_FLOWER:
		//Item Primary_Attribute
		m_pGameInstance->Render_Font(L"Font_Default", L"추가 체력", _float2(1115.f, 220.f), 0.5f, { 1.f, 1.f, 1.f, 0.5f }, 0.f, { 0.f,0.f }, false);

		//Item Primary_Attribute_Value
		wsprintf(szDamage, TEXT("%d"), m_iHitPoint);
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, _float2(1115.f, 243.f), 1.f, { 1.f, 1.f, 1.f, 1.f }, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_FEATHER:
		//Item Primary_Attribute
		m_pGameInstance->Render_Font(L"Font_Default", L"기초 공격력", _float2(1115.f, 220.f), 0.5f, { 1.f, 1.f, 1.f, 0.5f }, 0.f, { 0.f,0.f }, false);

		//Item Primary_Attribute_Value
		wsprintf(szDamage, TEXT("%d"), m_iBaseDamage);
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, _float2(1115.f, 243.f), 1.f, { 1.f, 1.f, 1.f, 1.f }, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_WATCH:
		//Item Primary_Attribute
		m_pGameInstance->Render_Font(L"Font_Default", L"방어력", _float2(1115.f, 220.f), 0.5f, { 1.f, 1.f, 1.f, 0.5f }, 0.f, { 0.f,0.f }, false);

		//Item Primary_Attribute_Value
		m_pGameInstance->Render_Font(L"Font_Number", strDefPercent, _float2(1115.f, 243.f), 1.f, { 1.f, 1.f, 1.f, 1.f }, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_GRAIL:
		//Item Primary_Attribute
		m_pGameInstance->Render_Font(L"Font_Default", L"치명타 확룔", _float2(1115.f, 220.f), 0.5f, { 1.f, 1.f, 1.f, 0.5f }, 0.f, { 0.f,0.f }, false);

		//Item Primary_Attribute_Value
		m_pGameInstance->Render_Font(L"Font_Number", strCRPercent, _float2(1115.f, 243.f), 1.f, { 1.f, 1.f, 1.f, 1.f }, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_CROWN:
		//Item Primary_Attribute
		m_pGameInstance->Render_Font(L"Font_Default", L"치명타 피해", _float2(1115.f, 220.f), 0.5f, { 1.f, 1.f, 1.f, 0.5f }, 0.f, { 0.f,0.f }, false);

		//Item Primary_Attribute_Value
		m_pGameInstance->Render_Font(L"Font_Number", strCDPercent, _float2(1115.f, 243.f), 1.f, { 1.f, 1.f, 1.f, 1.f }, 0.f, { 0.f,0.f }, false);
		break;
	case Client::CItem_Artifact::ATYPE_END:
		break;
	default:
		break;
	}


	//Item Detailed Info
	m_pGameInstance->Render_Font(L"Font_Default", m_strItemInfo, _float2(1115.f, 350.f), .5f, { 0.27450980392f,0.31372549019f,0.39215686274f,1.f}, 0.f, { 0.f,0.f }, false);

	return S_OK;
}

void CItem_Artifact::Use_item(_uint iQty, PLAYERINDEX ePlayerIndex)
{
}


CItem_Artifact* CItem_Artifact::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Artifact* pInstance = new CItem_Artifact(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CItem_Artifact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Artifact::Clone(void* pArg)
{
	CItem_Artifact* pInstance = new CItem_Artifact(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CItem_Artifact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Artifact::Free()
{
	__super::Free();
}
