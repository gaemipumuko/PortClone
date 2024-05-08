#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Trail;
class CFiniteStateMachine;
END

BEGIN(Client)

class CPart_Glider abstract : public CPartObject
{
public:
	typedef struct tagPartGliderDesc
	{
		class CModel*			pBody;
		class CBone*			pBone;

		RECEIVED_COM			tModelComponent;
		PARTOBJ_DESC			tPartObjDesc;

		CTransform*				pTransform;
		CFiniteStateMachine*	pFiniteStateMachineCom;
	} PARTGLIDER_DESC;

protected:
	CPart_Glider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Glider(const CPart_Glider& rhs);
	virtual ~CPart_Glider() = default;

protected:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CFiniteStateMachine*	m_pParentFSMCom = { nullptr };

protected:
	CTransform*		m_pTransformCom = { nullptr };
	CTransform*		m_pParentTransformCom = { nullptr };

	_float4x4		m_CombinedWorldMatrix = { };	/* Transform * ParentTransform */

protected:
	_int				m_iColliderID = { -1 };
	_int				m_iMeshIndex = { -1 };
	_float				m_fMargin = { -0.2f };

protected:
	void	Set_ZeroMatrix() { XMStoreFloat4x4(&m_CombinedWorldMatrix, _matrix{ }); }
	void	Set_IdentityMatrix() { XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity()); }

protected:
	void	Set_MeshIndex(_int iIndex) { m_iMeshIndex = iIndex; }

protected:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta) { return S_OK; }

protected:
	virtual HRESULT Add_Component(void* pArg);
	virtual HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

	virtual _int	Get_CollisionID() { return m_iColliderID; }
	virtual void	Release_ColliderID() override;

public:
	static CPart_Glider*	Create_Glider(GLIDERID eID, PARTGLIDER_DESC tWeaponDesc);
	virtual void			Free() override;
};

END