#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

private:
	map<const wstring, class CFont*>	m_Fonts;

public:
	HRESULT			Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	class CFont*	Find_Font(const wstring& strFontTag);

public:
	HRESULT Initialize();
	HRESULT Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, 
		_fvector vColor, _float fRotation, const _float2& vOrigin, _bool bAlign);
	 
public:
	static CFont_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END