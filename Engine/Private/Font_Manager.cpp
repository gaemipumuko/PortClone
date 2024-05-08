#include "Font_Manager.h"

#include "Font.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (Find_Font(strFontTag) != nullptr)
		return E_FAIL;

	CFont* pFont = CFont::Create(m_pDevice, m_pContext, strFontFilePath);
	if (pFont == nullptr)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

CFont* CFont_Manager::Find_Font(const wstring& strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

HRESULT CFont_Manager::Initialize()
{
	return S_OK;
}

HRESULT CFont_Manager::Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin, _bool bAlign)
{
	CFont* pFont = Find_Font(strFontTag);
	if (pFont == nullptr)
		return E_FAIL;

	return pFont->Render(strText, vPosition, fScale, vColor, fRotation, vOrigin, bAlign);
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CFont_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
