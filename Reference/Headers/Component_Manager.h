#pragma once

#include "Base.h"

#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Point.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Terrain.h"
#include "FiniteStateMachine.h"
#include "VIBuffer_Particle_Instancing.h"

#pragma region FOR_COLLIDER
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_SPHERE.h"
#pragma endregion 

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
protected:
	typedef map<const wstring, class CComponent*> PROTOTYPES;

private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

private:
	_uint			m_iNumLevel = { 0 };
	PROTOTYPES*		m_Prototypes = { nullptr };

public:
	HRESULT				Add_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, class CComponent* pPrototype, _bool bDuplication = FALSE);
	HRESULT				Release_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag);

	class CComponent*	Find_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag);
	class CComponent*	Clone_Component_Prototype(_uint iLevel, const wstring& strPrototypeTag, void* pArg = nullptr);

public:
	HRESULT Initialize(_uint iNumLevel);

public:
	static CComponent_Manager*	Create(_uint iNumLevel);
	void						Clear(_uint iLevel);
	virtual void				Free();
};

END