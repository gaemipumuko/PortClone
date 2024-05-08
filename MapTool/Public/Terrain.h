#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CVIBuffer_Terrain;
class CTexture;
END

BEGIN(Tool)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURE_TYPE 
	{ 
		GRASS_DIFFUSE,
		GRASS_NORMAL,
		ROAD_DIFFUSE,
		ROAD_NORMAL,
		STONE_DIFFUSE,
		STONE_NORMAL,
		TEX_END
	};

	enum MASK_TYPE { MASK_GRASS, MASK_ROAD, MASK_STONE, MASK_END };

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public: /* For.Terrain */
	HRESULT Resize_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ);
	HRESULT Apply_HeightMap(const wstring& strDataFilePath);
	HRESULT Save_HeightMap(const wstring& strDataFilePath);
	HRESULT Create_Mask(const wstring& strDataFilePath);
	HRESULT Save_Mask(const wstring& strDataFilePath);
	HRESULT Brushing_OnTerrain(_float& fBrushForce, _float3& vBrushPos, _float& fBrushSize, _float& fStrength, _float& fMaxHeight);
	HRESULT Masking_OnTerrain(_float3& vBrushPos, _float& fBrushSize, MASK_TYPE eType);
	HRESULT Get_TerrainSize(_float2* pTerrainSize);

public: /* For.Brush */
	HRESULT Activate_Brush(_bool bValue);
	HRESULT Resize_Brush(_float fScale);
	void Set_BrushPos(_float4 vBrushPos);
	
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_float Compute_MaskInfluence(_float3& vBrushPoint, _float3& vBrushPos, _float& fBrushSize);

	
private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	class CNaviMesh* m_pNaviCom = { nullptr };
	CTexture* m_pTextureCom[TEX_END] = {nullptr};

private:
	class CBrush* m_pBrush = { nullptr };
	_float3 m_vBrushPos = {};
	_bool m_bMask = false;
	ID3D11Texture2D* m_pMaskTexture = { nullptr };
	ID3D11ShaderResourceView* m_pMaskSRV = { nullptr };
	_ulong* m_pMaskData = { nullptr };
	_bool m_bWire = false;
	_bool m_bShow = true;

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
