#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
class CVIBuffer_Model_Instancing;
END

BEGIN(Client)

class CInstancing_Element final : public CBase
{
public:
	enum INSTANCE_TYPE
	{
		INSTANCE_GRASS_LUSH,
		INSTANCE_GRASS_NORMAL,
		INSTANCE_GRASS_SILVER,
		INSTANCE_GRASS_PINKTHREELEAF,
		INSTANCE_GRASS_BLUEONELEAF,
		INSTANCE_GRASS_PINKLONG,
		INSTANCE_GRASS_FLOWER_BLUE,
		INSTANCE_GRASS_TORAN,
		//INSTANCE_GRASS_LUSHLEAF,

		INSTANCE_GRASS_FLOWER_Y1,
		INSTANCE_GRASS_FLOWER_YBR,
		INSTANCE_GRASS_FLOWER_Y2,
		INSTANCE_GRASS_FLOWER_R,
		INSTANCE_GRASS_FLOWER_YBR_LOW,
		INSTANCE_GRASS_FLOWER_YB,
		INSTANCE_GRASS_FLOWER_BR,
		INSTANCE_GRASS_FLOWER_YBW,
		INSTANCE_GRASS_FLOWER_B1,
		INSTANCE_GRASS_FLOWER_PUPLE,
		INSTANCE_GRASS_FLOWER_NAPAL_R,
		INSTANCE_GRASS_FLOWER_O1,
		INSTANCE_GRASS_FLOWER_O2,
		INSTANCE_GRASS_FLOWER_PINK,
		INSTANCE_GRASS_FLOWER_B2,
		INSTANCE_GRASS_FLOWER_SUN,
		INSTANCE_GRASS_FLOWER_Y3,
		INSTANCE_END
	};

private:
	CInstancing_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInstancing_Element() = default;

public:
	HRESULT Initialize(LEVELID eLevel);
	HRESULT Update(_float4* pPosition);
	HRESULT Render();
	HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray);

private:
	HRESULT Initialize_Instancing();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Model_Instancing* m_pVIBufferCom[INSTANCE_END] = {};

private:
	LEVELID m_eLevel = LEVEL_END;

public:
	static CInstancing_Element* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eLevel);
	virtual void Free() override;
};

END
