#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
class CVIBuffer_Terrain;
class CTexture;
END

BEGIN(Client)

class CTerrain final : public CBase
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
		TEX_MASK,
		TEX_END
	};

	typedef struct tagTerrainDesc 
	{
		_uint iRow;
		_uint iColl;
		
		class CTexture* pDiffuse;
		class CTexture* pNormal;

		wstring strElementPath;
	}TERRAIN_DESC;

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTerrain() = default;

public:
	HRESULT Initialize();
	HRESULT Initialize(TERRAIN_DESC* pTerrainDesc);
	HRESULT Update();
	HRESULT Render();

public:
	HRESULT Invalidate_Culling();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom[TEX_END] = {};

private:
	_uint m_iTerrainWidth = 256;
	_uint m_iTerrainHeight = 256;

	_float m_fTerrainX = 0.0f;
	_float m_fTerrainY = 0.0f;

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TERRAIN_DESC* pTerrainDesc);
	virtual void Free() override;
};

END
