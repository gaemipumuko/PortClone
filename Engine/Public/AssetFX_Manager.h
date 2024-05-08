#pragma once

#include "AssetFX.h"

BEGIN(Engine)

class CAssetFX_Manager final : public CBase
{
private:
	CAssetFX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAssetFX_Manager() = default;

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };

private:
	map<wstring, class CAssetFX*>	m_Prototypes;	/* 프로토타입 */
	map<wstring, class CAssetFX*>	m_Effects;		/* 실사용객체 */

public:
	HRESULT	Ready_FXPrototype(const wstring& strPrototypeTag, class CAssetFX* pAssetFX);

	HRESULT	Play_AssetFX(const wstring& strPrototypeTag, const wstring& strEffectTag, void* pArgs);
	HRESULT	Stop_AssetFX(const wstring& strEffectTag, _float fDelay);
	HRESULT	Pause_AssetFX(const wstring& strEffectTag);

public:
	HRESULT Initialize();
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);

public:
	static CAssetFX_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

END