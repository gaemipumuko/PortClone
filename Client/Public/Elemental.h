#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

#include "Player.h"
#include "Part_Status.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CElemental abstract : public CDeciderObject
{
public:
	typedef struct tagElementalDesc
	{
		ELEMENT			eAttribute;

		_vector			vInitPosition;
		CTransform*		pTargetTransform;
	} ELEMENTAL_DESC;

protected:
	CElemental(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElemental(const CElemental& rhs);
	virtual ~CElemental() = default;

protected:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTransform*			m_pTargetTransformCom = { nullptr };

protected:
	_float				m_fFrame = { };
	_bool				m_bIntersect = { FALSE };

	_float4				m_vAttColor = { };
	ELEMENT				m_eAttribute = { ELEMENT_END };

public:
	_bool	Get_Intersect() { return m_bIntersect; }

public:
	HRESULT	Invalidate_Elemental(void* pArg);

protected:
	HRESULT Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) PURE;
	virtual HRESULT Tick(_float fTimeDelta) PURE;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() PURE;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END