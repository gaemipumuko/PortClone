#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	_uint Get_NextLevel() { return m_eNextLevelID; }

private:
	LEVELID			m_eNextLevelID = { LEVEL_END };
	class CLoader*	m_pLoader = { nullptr };

private:
	HRESULT Ready_Level();
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

public:
	virtual HRESULT Initialize(LEVELID eLevel);
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLevel_Loading*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevel);
	virtual void			Free() override;
};

END