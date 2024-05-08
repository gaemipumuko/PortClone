#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CVIBuffer_Model_Instancing;
END

BEGIN(Tool)

class CInstancing final : public CGameObject
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
	CInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancing(const CInstancing& rhs);
	virtual ~CInstancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Instance(INSTANCE_TYPE eType, _float4x4 vInstanceWorld);
	HRESULT Load_InstanceData(INSTANCE_TYPE eType, const wstring& strInstanceDataPath);
	HRESULT Save_InstanceData(INSTANCE_TYPE eType, const wstring& strInstanceDataPath);
	HRESULT Clear_InstanceData(INSTANCE_TYPE eType);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Model_Instancing* m_pVIBufferCom[INSTANCE_END] = {};

public:
	static CInstancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
