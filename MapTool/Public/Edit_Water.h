#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CVIBuffer_Terrain;
class CTexture;
class CCollider;
END

BEGIN(Tool)

class CEdit_Water final : public CGameObject
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
	CEdit_Water(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Water(const CEdit_Water& rhs);
	virtual ~CEdit_Water() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float4 Get_Position();
	_float4x4 Get_WorldMatrix();
	_uint Get_SizeX() { return m_iSizeX; }
	_uint Get_SizeZ() { return m_iSizeZ; }
	_float Get_GridSpacing() { return m_fGridSpacing; }
	_float* Get_WaveScale() { return &m_fWaveScale; }
	_float3* Get_UVScale() { return &m_vUVScale; }
	_float3* Get_Noise() { return &m_vNoise; }
	_float2* Get_WaveSpeed() { return &m_vWaveSpeed; }

	void Set_WaveScale(_float* pWaveScale);
	void Set_UVScale(_float* pUVScale);
	void Set_Noise(_float* pNoise);
	void Set_WaveSpeed(_float* pSpeed);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Initialize_Wave();
	void Invalidate_UV(_float fTimeDelta);
	HRESULT Disturb_Wave(_float fTimeDelta);
	HRESULT Simulation_Wave(_float fTimeDelta);

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool m_bWire = false;
	_bool m_bShow = true;

private: /* Common */
	_uint m_iSizeX;
	_uint m_iSizeZ;
	_float m_fGridSpacing;
	_uint m_iNumVertices;
	_float m_fTime = 0.0f;
	_float m_fWaveTime = 0.0f;

private: /* Simulation */	
	_float m_fK1;
	_float m_fK2;
	_float m_fK3;
	
	_float m_fTimeStep;
	_float m_fSpatialStep;
	_float m_fWaveStep;
	_float m_fWaveScale = 10.0f;

	vector<_float3> m_PrevSolution;
	vector<_float3> m_CurSolution;
	vector<_float3> m_Normals;
	vector<_float2> m_TexCoord;
	vector<_float3> m_TangentX;

private: /* Wave Scroll */
	_float3 m_vUVScale = _float3(2.0f, 5.0f, 10.0f);
	_float2 m_vUVOffset = _float2(0.0f, 0.0f);
	_float3 m_vNoise = _float3(0.15f, 0.3f, 0.7f);
	_float2 m_vWaveSpeed = _float2(0.05f, 0.075f);

public:
	static CEdit_Water* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
