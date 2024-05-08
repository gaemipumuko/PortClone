#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CButton final : public COrtho
{

private:
	CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButton(const CButton& rhs);
	virtual ~CButton() = default;

private:
	HRESULT Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_Pressed() { return m_bPressedDown; }

	void Set_Text(_float2 fDimentions, wstring& strText) { m_fTextDimentions = fDimentions; lstrcpy(m_szText, strText.c_str()); }
	void Set_Pressed(_bool bPressed) { m_bPressedDown = bPressed; }

public:
	static CButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;

private:
	_float2		m_fTextDimentions = {};
	_tchar		m_szText[MAX_PATH] = {};
	_bool		m_bMouseEnter = {true};
	_bool		m_bMouseOver = { false };
	_bool		m_bPressedDown = { false };


};

END