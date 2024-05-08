#pragma once

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
protected:
	typedef map<const wstring, class CLayer*>	LAYERS;

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

private:
	_uint										m_iNumLevel = { 0 };
	map<const wstring, class CGameObject*>		m_Prototype;
	LAYERS*										m_pLayers = { nullptr };

public:
	HRESULT				Add_Object_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype, _bool bDuplication = FALSE);
	HRESULT				Add_Object_Layer(_uint iLevel, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg, class CGameObject** ppObject);

	class CGameObject*	Find_Object_Prototype(const wstring& strPrototypeTag); /* Prototype 반환 */
	class CLayer*		Find_Object_Layer(_uint iLevel, const wstring& strLayerTag); /* Layer Group 반환 */
	class CGameObject*	Find_Object(_uint iLevel, const wstring& strLayerTag, _uint iIndex = 0); /* Layer의 특정 Object 반환 */

	class CComponent*	Find_Layer_Component(_uint iLevel, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);

	HRESULT				Release_Object(_uint iLevel, const wstring& strLayerTag, class CGameObject* pGameObject);
	HRESULT				Release_Object_Layer(_uint iLevel, const wstring& strLayerTag);

	list<class CGameObject*>* Find_Objects(_uint iLevel, const wstring& strLayerTag);

public:
	HRESULT Initialize(_uint iNumLevel);
	HRESULT PreviousTick(_float fTimeDelta);
	HRESULT Tick(_float fTimeDelta);
	HRESULT LateTick(_float fTimeDelta);

public:
	static CObject_Manager*		Create(_uint iNumLevel);
	void						Clear(_uint iLevel);
	virtual void				Free() override;
};

END