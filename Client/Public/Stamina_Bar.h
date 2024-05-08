#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "AlphaRect.h"

BEGIN(Client)

class CStamina_Bar final : public COrtho
{
private:
	CStamina_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStamina_Bar(const CStamina_Bar& rhs);
	virtual ~CStamina_Bar() = default;

private:
	wstring					m_strText = {};
	_uint					m_iNumPass = {};
	_vector					m_vColor = {};
	_float2					m_vUVScale = { 1.f, 1.f };
	_float2					m_vUVPosition = { 0.f, 0.f };
	CAlphaRect*				m_pStamina_Bar = { nullptr };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT	Set_Texture_Color(_vector vColor);
	void	Update_Stamina(_float fCurrentSt, _float fMaxSt);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;




public:
	static CStamina_Bar*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END