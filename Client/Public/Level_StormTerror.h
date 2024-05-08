#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_StormTerror final : public CLevel
{
private:
	CLevel_StormTerror(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_StormTerror() = default;

private:
	HRESULT Ready_Level();

	HRESULT	Ready_Lights();
	HRESULT Ready_ConstantBuffer();
	HRESULT	Ready_ParticleManager();

	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_StormTerror(const wstring& strLayerTag);

	HRESULT Ready_Layer_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_EventCollider(const wstring& strLayerTag);
	HRESULT Ready_Layer_NPC(const wstring& strLayerTag);
	HRESULT Ready_Layer_Icon(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const wstring& strLayerTag);

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLevel_StormTerror* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
