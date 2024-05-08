#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CFlameSlash_Trail final : public CDeciderObject
{
public:
	typedef struct tagFlameSlashDesc
	{
		CTransform* pParentsTransform;
		_float		fHeight;
		_float		fAngle;
		_float		fTrailSpeed;
		_bool		bTrailReverse;

	}FLAMESLASH_DESC;

private:
	CFlameSlash_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlameSlash_Trail(const CFlameSlash_Trail& rhs);
	virtual ~CFlameSlash_Trail() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTrailTextureCom = { nullptr };

	CTransform*			m_pParentsTransformCom = { nullptr };

private:
	_float				m_fHeight = { 0.f };
	_float				m_fAngle = { 0.f };
	_float				m_fTrailSpeed = { 0.f };

	_bool				m_bTrailReverse = { FALSE };

	_int				m_iFlameNum = { 0 };

private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fLifeTime = 0.f;
	_float				m_fTimeAcc = 0.f;


private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT	Create_Flame();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CFlameSlash_Trail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END