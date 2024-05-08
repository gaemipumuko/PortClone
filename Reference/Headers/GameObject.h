#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagReceivedComponent
	{
		_uint			iComponentLevel;
		wstring			strComponentTag;
	} RECEIVED_COM;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	map<const wstring, class CComponent*>		m_Components;

public:
	HRESULT				Add_Component(_uint iLevel, const wstring& strPrototypeTag, const wstring& strComponentTag, 
		class _Inout_ CComponent** ppComponent, void* pArg = nullptr);
	HRESULT				Release_Component(const wstring& strComponentTag);

	class CComponent*	Find_Component(const wstring& strComponentTag);

public:
	wstring		Get_ParticleTag(wstring strParticleTag);

protected:
	static _uint			m_iUniqueID;
	_uint					m_iObjectID = { INT_MAX };
	_uint					m_iParticleIndex = { 0 };

protected:
	static vector<CGameObject*>		m_MonsterTarget;

protected:
	void Release_Target();
	void Add_Target(CGameObject* pGameObject);
	void Clear_Target();

protected:
	_bool	m_bDead = { FALSE };
	_bool	m_bDestroy = { FALSE };
	_bool	m_bCloned = { FALSE };

public:
	_bool	Get_Dead() { return m_bDead; }
	void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	_bool	Get_Destroy() { return m_bDestroy; }
	void	Set_Destroy(_bool bDestroy) { m_bDestroy = bDestroy; }

	_bool	Get_Cloned() { return m_bCloned; }

protected:
	typedef struct tagMTrailDesc
	{
		_float4x4		WorldMatrix;
		class CModel*	pModelComponent;
	}MTRAIL_DESC;

	_float				m_fMTrailFrame = { };
	list<MTRAIL_DESC>	m_MTrailList;

public:
	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual HRESULT PreviousTick(_float fTimeDelta) PURE;
	virtual HRESULT Tick(_float fTimeDelta) PURE;
	virtual HRESULT LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() { return S_OK; }
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) { return S_OK; }
	virtual HRESULT Render_Trail() { return S_OK; }
	virtual HRESULT Render_MTrail() { return S_OK; }

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END