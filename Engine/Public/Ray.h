#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CRay final : public CBase
{
private:
	CRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRay() = default;

public:
	_float3 Get_RayPos() { return m_vRayPos; }
	_float3 Get_RayDir() { return m_vRayDir; }

public:
	HRESULT Initialize(_float3 vRayPos, _float3 vRayDir);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	_float3 m_vRayPos = _float3();
	_float3 m_vRayDir = _float3();

public:
	static CRay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float3 vRayPos, _float3 vRayDir);
	static CRay* ScreenPointToRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float2 vScreenPoint);
	static _bool RayCast(_uint iLevel, const wstring& strLayerTag, _float3 vRayPos, _float3 vRayDir, class CGameObject** pHitObject, _float& fDist);
	static _bool RayCast(_float3 vRayPos, _float3 vRayDir, class CGameObject* pTerrain, _float3* pPosition_InTerrain);

	virtual void Free() override;
};

END
