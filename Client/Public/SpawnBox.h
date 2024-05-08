#pragma once

#include "Client_Defines.h"
#include "EventCollider.h"

BEGIN(Client)

class CSpawnBox final : public CEventCollider
{
public:
	typedef struct tagMonsterInfo
	{
		CREATUREID eCreatureId = CREATURE_END;
		_float4 vSpawnPosition;
	}MONSTER_INFO;

	typedef struct tagSpawnerDesc
	{
		_uint iLevel;
		_float4 vSpawnerPos;
		_float fSpawnRange;
		vector<MONSTER_INFO>* pMonsterInfo = nullptr;
	}SPAWNER_DESC;

private:
	CSpawnBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnBox(const CSpawnBox& rhs);
	virtual ~CSpawnBox() = default;

private:
	_uint m_iLevel;
	_float4 m_vSpawnerPos;
	_float m_fSpawnRange;
	vector<MONSTER_INFO> m_MonsterInfo;

	_bool m_bisColl = false;

public:
	wstring Get_SpawnerTag();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();

public:
	static CSpawnBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void	Free() override;

};

END