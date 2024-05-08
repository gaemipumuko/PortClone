#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CStaff_Elec final : public CPart_Weapon
{
private:
	CStaff_Elec(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaff_Elec(const CStaff_Elec& rhs);
	virtual ~CStaff_Elec() = default;
	
private:
	_float4	m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	_float4	m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };
	//_float a = 0, b = -1.45f, c = 0.15f;
	_float a = 0.f, b = 0.f, c = 0.f;

public:
	_matrix Get_CombinedMatrix();

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

public:
	virtual void Release_ColliderID() override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CStaff_Elec*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END