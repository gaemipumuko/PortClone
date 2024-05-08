#pragma once

#include "Client_Defines.h"
#include "Part_Weapon.h"

BEGIN(Client)

class CSlime_IceCone final : public CPart_Weapon
{
private:
	CSlime_IceCone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_IceCone(const CSlime_IceCone& rhs);
	virtual ~CSlime_IceCone() = default;
	
private:
	_float4	m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	_float4	m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };

private:
	_float m_fAttackEndTime = { 0.f };
	//_int   m_iConeIndex = { 0 }; /* hp 깎이면 콘 안보여야함 */

public:
	_matrix Get_CombinedMatrix();

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

public:
	HRESULT Break_IceCone(_int iConeIndex);
	virtual void Release_ColliderID() override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CSlime_IceCone*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END