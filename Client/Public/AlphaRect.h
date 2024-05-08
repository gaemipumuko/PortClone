#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CAlphaRect final : public COrtho
{
public:
	enum TEXT_POSITION { TEXT_ON, TEXT_UP,TEXT_DOWN,TEXT_LEFT,TEXT_RIGHT,TEXT_END};
private:
	CAlphaRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlphaRect(const CAlphaRect& rhs);
	virtual ~CAlphaRect() = default;

private:
	wstring					m_strText = {};
	wstring					m_strDetailBelow = {};
	TEXT_POSITION			m_eTextPosition = TEXT_END;

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
	void Set_Text(wstring strName, TEXT_POSITION eTextPosition) {
		m_strText = strName;
		m_eTextPosition = eTextPosition;
	}
	void Set_Detail_Text(wstring strDetail) {
		m_strDetailBelow = strDetail;
	}
	
public:
	static CAlphaRect*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END