#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CButtonAlpha final : public COrtho
{
private:
	CButtonAlpha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButtonAlpha(const CButtonAlpha& rhs);
	virtual ~CButtonAlpha() = default;

private:
	HRESULT Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Pressed(_bool bPressed) { m_bPressedDown = bPressed; }

public:
	static CButtonAlpha*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

private:
	_bool		m_bMouseEnter = { true };
	_bool		m_bMouseOver = { false };
	_bool		m_bPressedDown = { false };
};

END