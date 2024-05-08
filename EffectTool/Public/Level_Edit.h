#pragma once

#include "EffectTool_Defines.h"
#include "Level.h"

BEGIN(EffectTool)

class CLevel_Edit final : public CLevel
{
private:
	CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Edit() = default;

private:
	HRESULT Ready_Level();

	HRESULT	Ready_ParticleManager();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const wstring& strLayerTag);

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
