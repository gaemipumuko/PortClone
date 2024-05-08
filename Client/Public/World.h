#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CWorld final : public CDeciderObject
{
	enum WORLD_TYPE { WT_TERRAIN = 0b0001, WT_ELEMENT = 0b0010, WT_INSTANCING = 0b0100, WT_WATER = 0b1000, WT_END = 0b0000 };

private:
	CWorld(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWorld(const CWorld& rhs);
	virtual ~CWorld() = default;

private:
	_byte	m_byFlag = { WT_END };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

private:
	HRESULT Add_Components();
	
	HRESULT Initialize_World();

	HRESULT Initialize_Terrain();
	HRESULT Initialize_Element();
	HRESULT Initialize_Instancing();
	HRESULT Initialize_Water();
	HRESULT Initialize_Spawner();

	HRESULT Invalidate_Culling();

private:
	CRenderer* m_pRendererCom = { nullptr };

private:
	class CTerrain* m_pWorldTerrain = nullptr;
	vector<class CWorldElement*> m_Elements;
	class CInstancing_Element* m_pInstancing_Elements = nullptr;
	vector<class CWater*> m_Waters;
	_bool m_bDebugRender = false;

public:
	static CWorld* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
