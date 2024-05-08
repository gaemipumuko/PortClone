#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont final : public CBase
{
private:
	CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	SpriteFont*				m_pFont = { nullptr };
	SpriteBatch*			m_pBatch = { nullptr };

public:
	HRESULT Initialize(const wstring& strFontFilePath);
	HRESULT Render(const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin, _bool bAlign);
	HRESULT Render(const wstring& strText, const _fvector& vPosition, _float fScale, _fvector vColor, _float fRotation, const _fvector& vOrigin, _bool bAlign);
	
public:
	static CFont*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath);
	virtual void	Free() override;
};

END