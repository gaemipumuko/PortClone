#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CDungeon final : public CDeciderObject
{
public:
	typedef struct tagDungeonDesc 
	{
		LEVELID eLevel;
	}DUNGEON_DESC;

private:
	CDungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDungeon(const CDungeon& rhs);
	virtual ~CDungeon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

public:
	HRESULT Set_ElementPassID(_uint iIndex, PASSID_VTXMESH ePassID);

	void Reset_ElementPassID();
	
	_bool isCollaps(_uint iIndex);

private:
	HRESULT Add_Components();

private:
	HRESULT Initialize_Dungeon();
	HRESULT Loading_Map();

private:
	CRenderer* m_pRendererCom = { nullptr };

private:
	LEVELID m_eLevel;
	vector<class CDungeonElement*> m_DungeonElements;

public:
	static CDungeon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
