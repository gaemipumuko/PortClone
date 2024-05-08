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

class CShougun_Trail_TEST final : public CDeciderObject
{
public:
	typedef struct tagShougunTrail
	{
		_vector			vTrailPos;
		CTransform*		pParentTransform;
		_int			iTrailIndex;


	}SHOGUN_TRAIL_DESC;

private:
	CShougun_Trail_TEST(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShougun_Trail_TEST(const CShougun_Trail_TEST& rhs);
	virtual ~CShougun_Trail_TEST() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };

private:
	CTransform*			m_pParentTransform = { nullptr };

private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fLifeTime = 0.f;
	_float				m_fTimeAcc = 0.f;

	_int				m_iTrail = {};

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
	static CShougun_Trail_TEST*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END