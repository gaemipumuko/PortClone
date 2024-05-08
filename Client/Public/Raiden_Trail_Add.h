#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CRaiden_Trail_Add final : public CDeciderObject
{
public :
	enum TRAIL_TPYE {TRAIL_COMMON, TRAIL_RING, TRAIL_RANDOM, TRAIL_END};
	enum TRAIL_EXCEPT_AXIS {
		AXIS_X = 1,
		AXIS_Y = 2,
		AXIS_Z = 4
	};

	typedef struct tagSlashDesc
	{
		TRAIL_TPYE  eTrail_Type;
		ELEMENT		eElementColor;
		CTransform* pParentsTransform;
		_uint		iExceptAxis;
		_vector		vScale;
		_float		fLifeTime;
		_float		fHeight;
		_float		fAngle;
		_float		fTrailSpeed;
		_float		vMoveLook;
		_bool		bTrailReverse;
	}SLASH_DESC;

private:
	CRaiden_Trail_Add(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaiden_Trail_Add(const CRaiden_Trail_Add& rhs);
	virtual ~CRaiden_Trail_Add() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTrailTextureCom = { nullptr };
	CTransform*			m_pParentsTransformCom = { nullptr };

	CModel* m_pModelCom1 = { nullptr };
	CModel* m_pModelCom2 = { nullptr };
	CModel* m_pModelCom3 = { nullptr };
	CModel* m_pModelCom4 = { nullptr };
	CModel* m_pModelCom5 = { nullptr };
	CModel* m_pModelCom6 = { nullptr };
	CModel* m_pModelCom7= { nullptr };
	CModel* m_pModelCom8 = { nullptr };
	CModel* m_pModelCom9 = { nullptr };

	CModel* m_pCurrentModel = { nullptr };

private:
	TRAIL_TPYE			m_eTrailType = {TRAIL_END};
	_float				m_fHeight = { 0.f };
	_float				m_fTrailSpeed = { 0.f };
	_float				m_vMoveLook = {};

	_int				m_iCurrentModel = {};

private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fLifeTime = { 0.f };
	_float				m_fTimeAcc = { 0.f };

	_bool				m_bReCycle = { FALSE };
	
	_vector				m_vTrailColor = {};

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CRaiden_Trail_Add*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END