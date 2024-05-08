#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CCollider;
END

BEGIN(Client)

class CDungeonPlatform final : public CGameObject
{
public:
	typedef struct tagDPlatformDesc
	{
		LEVELID		ePlatformLevel = { LEVEL_END };

		KEYFRAME	eTransformDesc = { };
		_float4		vLiftTranslation = { };
	} DPLATFORM_DESC;

private:
	CDungeonPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDungeonPlatform(const CDungeonPlatform& rhs);
	virtual ~CDungeonPlatform() = default;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool			m_bReacted = { FALSE };
	_float4			m_vLiftTranslation = { };
	PxRigidStatic* m_pRigidStatic = { nullptr };

public:
	void			Set_Reacted(_bool bReacted) { m_bReacted = bReacted; }

private:
	HRESULT			Add_Component(void* pArg);
	HRESULT			Bind_ShaderResources();
	_bool isInPlatform(CGameObject** ppGameObject);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CDungeonPlatform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END