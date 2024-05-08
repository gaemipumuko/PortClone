#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CEffect abstract : public CDeciderObject
{
public:
	enum EFFECT_PARENT { PARENT_DILUC_SWORD, PARENT_END };

	typedef struct tagEffectDesc
	{
		EFFECTID		eEffectID;
		EFFECT_PARENT	eParent = { PARENT_END };
		CGameObject*	pParent = { nullptr };
		CModel*			pModelCom = { nullptr };
		_float3			vRadians;
		_float			fLifeTime;
		_float			fSpeed;
		_vector			vSpawnPos;
		_vector			vScale;
		_vector			vDir;
		_vector			vBaseColor = {};

	}EFFECT_DESC;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

protected:
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

protected:
	EFFECT_DESC		m_EffectDesc = {};
	_float4x4		m_CombinedWorldMatrix = { };

protected:
	HRESULT		Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void	Free() override;
};

END