#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CHp_Bar final : public COrtho
{
public:
	enum BAR_TYPE { BAR_TYPE_PLAYER, BAR_TYPE_BOSS, BAR_TYPE_MONSTER, BAR_TYPE_SHIELD, BAR_TYPE_END};

private:
	CHp_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHp_Bar(const CHp_Bar& rhs);
	virtual ~CHp_Bar() = default;

private:
	_float4x4				m_ReductionMat;
	_float4x4				m_HealthMat;
	_float4x4				m_ShieldMat;
	_float4x4				m_ShieldReducMat;


	wstring					m_strOwnerName = {};
	wstring					m_strOwnerDetail = {};
	_uint					m_iOwnerLevel = {};
	_uint					m_iNumPass = {};
	_vector					m_vColor = {};
	_float2					m_vReductionUVScale = { 1.f,1.f };

	_float					m_fCurrentHp = { 1500.f };
	_float					m_fMaxHp = { 1500.f };
	_float2					m_vUVScale = { 1.f, 1.f };
	_float2					m_vUVPosition = { 0.f, 0.f };

	_float					m_fCurrentShield = { 0.f };
	_float					m_fMaxShield = { 0.f };
	_float2					m_vUVShieldScale = { 1.f, 1.f };
	_float2					m_vUVShieldPosition = { 0.f, 0.f };
	_float2					m_vShieldReductionUVScale = { 1.f,1.f };
	BAR_TYPE				m_eBarType = { BAR_TYPE_END };

private:

	HRESULT Add_Component(void* pArg);
	HRESULT	Set_Texture_Color(_vector vColor);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_Hp_Matrix(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void Set_MaxHp(_float fMHp) { m_fMaxHp = fMHp; }
	void Set_CurrentHp(_float fCHp) { m_fCurrentHp = fCHp; }

	void Update_Shield_Matrix(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void Set_MaxShield(_float fMShield) { m_fMaxShield = fMShield; }
	void Set_CurrentShield(_float fCShield) { m_fCurrentShield = fCShield; }

	void Set_BarType(BAR_TYPE eType) { m_eBarType = eType; }
	void Set_OwnerInfo(wstring strName, wstring strDetail, _uint iLevel) { m_strOwnerName = strName; 
																		m_strOwnerDetail = strDetail;  m_iOwnerLevel = iLevel; }



public:
	static CHp_Bar*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END