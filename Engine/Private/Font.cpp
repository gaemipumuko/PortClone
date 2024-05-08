#include "Font.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont::Initialize(const wstring& strFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pFont = new SpriteFont(m_pDevice, strFontFilePath.c_str());
	if (nullptr == m_pFont)
		return E_FAIL;

	return S_OK;
}

HRESULT CFont::Render(const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin, _bool bAlign)
{
	if (m_pFont == nullptr ||
		m_pBatch == nullptr)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	XMVECTOR vMeasure = { }; /* Align */

	if (bAlign == TRUE)
		vMeasure = m_pFont->MeasureString(strText.c_str());

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), { vPosition.x - XMVectorGetX(vMeasure) * 0.5f, vPosition.y }, vColor, fRotation, vOrigin, fScale);

	m_pBatch->End();

	return S_OK;
}

HRESULT CFont::Render(const wstring& strText, const _fvector& vPosition, _float fScale, _fvector vColor, _float fRotation, const _fvector& vOrigin, _bool bAlign)
{
	if (m_pFont == nullptr ||
		m_pBatch == nullptr)
		return E_FAIL;

	XMVECTOR vMeasure = { }; /* Align */

	if (bAlign == TRUE)
		vMeasure = m_pFont->MeasureString(strText.c_str());

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, fRotation, vOrigin, fScale);
	m_pBatch->End();

	return S_OK;
}

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath)
{
	CFont* pInstance = new CFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX(TEXT("Failed to Create : CFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont::Free()
{
	__super::Free();

	Safe_Delete(m_pFont);
	Safe_Delete(m_pBatch);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
