#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CLineTrap final : public CEffect
{
public:
	typedef struct tagLineTrapDesc
	{
		_float fSpeed;
		_float fAccel;
		_float fMoveRange;
		_vector vStartDir;
		_vector vPointA;
		_vector vPointB;
	}LINE_TRAP_DESC;

private:
	CLineTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLineTrap(const CLineTrap& rhs);
	virtual ~CLineTrap() = default;

private:
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pCollider = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float		m_fMaxSpeed = { 0.f };		/* �ְ� �ӵ� */
	_float		m_fSpeed = { 0.f };			/* �̵� �ӵ� */
	_float		m_fAccel = { 0.f };			/* ���� */
	_float		m_fMaxMoveRange = { 0.f };	/* �պ� �ִ� �Ÿ� */
	_float		m_fMoveRange{ 0.f };		/* ���� �̵��� �Ÿ� */
	_float3		m_vDir = {};				/* �̵� ���� */

private:
	HRESULT	Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLineTrap*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END