#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase
{
public:
	typedef struct tagColliderObject
	{
		_bool				bCollision = { TRUE };
		class CCollider*	pCollider = { nullptr };
		class CGameObject*	pGameObject = { nullptr };
	} COLLIDER_OBJECT;

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

private:
	class CGameInstance*			m_pGameInstance = { nullptr };

private:
	_int							m_iNumObjectID = { 0 };
	_uint							m_iNumLayer = { 0 };
	map<_int, COLLIDER_OBJECT>*		m_CollObjects = { nullptr };

public:
	void				Set_IsCollision(_uint iLayer, _int iObjectID, _bool bCollision);

public:
	HRESULT				Add_Collider_Object(_uint iLayer, OUT _int* pCreateObjectID, class CCollider* pCollider, class CGameObject* pGameObject);
	HRESULT				Release_Collider_Object(_uint iLayer, _int iObjectID);

	class CGameObject*			Find_ObjectID(_uint iLayer, _int iObjectID);
	class CGameObject*			Find_AllCollision_Object(_uint iLayer, _int iObjectID);
	vector<class CGameObject*>	Find_Collision_Objects(_uint iLayer, _int iObjectID, _uint iFindLayer);
	vector<class CGameObject*>	Find_Collision_Objects(class CCollider* pCollider, _uint iFindLayer);

public:
	HRESULT Initialize(_uint iNumLayer);
	void Clear();

public:
	static CCollider_Manager*	Create(_uint iNumLayer);
	virtual void				Free();
};

END