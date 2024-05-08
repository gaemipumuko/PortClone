#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_Edit final : public CLevel
{
private:
	CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Edit() = default;

private:
	HRESULT Ready_Level();

	HRESULT Ready_Light();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const wstring& strLayerTag);
	HRESULT Ready_Layer_GameObjects(const wstring& strLayerTag);
	HRESULT Ready_Layer_Instancing(const wstring& strLayerTag);
	HRESULT Ready_Layer_Water(const wstring& strLayerTag);
	HRESULT Ready_Layer_Spawner(const wstring& strLayerTag);

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLevel_Edit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

END
