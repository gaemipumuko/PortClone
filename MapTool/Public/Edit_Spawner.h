#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CVIBuffer_Cube;
class CTexture;
class CCollider;
END

BEGIN(Tool)

class CEdit_Spawner final : public CGameObject
{
public:
	enum CREATURE_TYPE 
	{
		CHARACTER_TRAVELER,
		CHARACTER_BARBARA, CHARACTER_DILUC, CHARACTER_GANYU, CHARACTER_ZHONGLI,
		NPC_PAIMON, NPC_KATHERYNE,
		MONSTER_CHUCHU,
		MONSTER_ANDRIUS,
		MONSTER_SLIME_WATER, MONSTER_SLIME_ICE,
		MONSTER_HILI_NORMAL,
		MONSTER_BRUTE_SHIELD,
		MONSTER_BRUTE_AXE,
		CREATURE_END
	};

	typedef struct tagMonsterInfo 
	{
		CREATURE_TYPE eCreatureType = CREATURE_END;
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
	CEdit_Spawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Spawner(const CEdit_Spawner& rhs);
	virtual ~CEdit_Spawner() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint Get_SpawnerLevel() { return m_iLevel; }
	_float4 Get_SpawnerPosition() { return m_vSpawnerPos; }
	_float Get_SpawnerRange() { return m_fSpawnRange; }

public:
	vector<MONSTER_INFO>* Get_MonsterInfo() { return &m_MonsterInfo; }
	HRESULT Add_MonsterInfo(MONSTER_INFO* pInfo);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_uint m_iLevel;
	_float4 m_vSpawnerPos;
	_float m_fSpawnRange;
	vector<MONSTER_INFO> m_MonsterInfo;

public:
	static CEdit_Spawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
