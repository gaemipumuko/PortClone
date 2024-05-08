#pragma once

#include "EventCollider.h"

BEGIN(Client)

class CLocation final : public CEventCollider
{
public:
	
	typedef struct tagLocationDesc
	{
		LOCATIONID eLocation;
		_float4 vSpawnPosition;
		_float	fRadius;
	}LOCATION_DESC;

private:
	CLocation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLocation(const CLocation& rhs);
	virtual ~CLocation() = default;

private:
	LOCATIONID m_eLocation = { LOCATION_END };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();

public:
	static CLocation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void	Free() override;
};

END
