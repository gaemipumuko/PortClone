#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
class CShader;
class CVIBuffer_Terrain;
class CTexture;
class CGameInstance;
END

BEGIN(Client)

class CWater final : public CBase
{
public:
	typedef struct tagWaterDesc
	{
		_uint iSizeX;
		_uint iSizeZ;
		_float4x4 vWorldMatrix;
		_float fGridSpacing;

		_float3 vUVScale = _float3(0.0f, 0.0f, 0.0f);
		_float fWaveScale = 0.0f;
		_float3 vNoise = _float3(0.0f, 0.0f, 0.0f);
		_float2 vWaveSpeed = _float2(0.0f, 0.0f);
	}WATER_DESC;

private:
	CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWater() = default;

public:
	HRESULT Initialize(WATER_DESC* pWaterDesc);
	HRESULT Simulate_Wave(_float fTimeDelta);
	HRESULT Invalidate_Culling();
	HRESULT Render();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Initialize_Wave();
	HRESULT Invalidate_UV(_float* pTimeDelta);
	HRESULT Disturb_Wave(_float* pTimeDelta);
	HRESULT Compute_Wave(_float* pTimeDelta);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_uint m_iSizeX;
	_uint m_iSizeZ;
	_float4x4 m_vWorldMatrix;
	_float m_fGridSpacing;

	_float3 m_vUVScale;
	_float2 m_vUVOffset = _float2(0.0f, 0.0f);
	_float3 m_vNoise;
	_float2 m_vWaveSpeed;
	
	_uint m_iNumVertices;
	_float m_fTime = 0.0f;
	_float m_fWaveTime = 0.0f;

private:
	_float m_fK1;
	_float m_fK2;
	_float m_fK3;

	_float m_fTimeStep;
	_float m_fSpatialStep;
	_float m_fWaveStep;
	_float m_fWaveScale;

	vector<_float3> m_PrevSolution;
	vector<_float3> m_CurSolution;
	vector<_float3> m_Normals;
	vector<_float2> m_TexCoord;
	vector<_float3> m_TangentX;

public:
	static CWater* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, WATER_DESC* pWaterDesc);
	virtual void Free() override;
};

END
