#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CSkillObj abstract : public CDeciderObject
{
public:
	typedef struct tagSkillObjDesc
	{
		ELEMENTAL_REACTION	eReaction;
		SKILLOBJID			eSkillObjID;
		ELEMENT				eElement;
		COLLID				eCollider;
		_float				fLifeTime;
		_float				fCreateDelay;
		_float				fDamage;
		_vector				vSpawnPos;
		_vector				vDir;
		_vector				vScale;
		_vector				vColor = {};
	}SKILLOBJ_DESC;	

protected:
	CSkillObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillObj(const CSkillObj& rhs);
	virtual ~CSkillObj() = default;

protected:
	_int		m_iColliderID = { -1 };

protected:
	HRESULT		Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void	Free() override;
};

END