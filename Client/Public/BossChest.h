#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CBossChest final : public CGameObject
{
public:
	typedef struct tagBossChestDesc
	{
		LEVELID		eLevel = { };

		_float4		vRotation = { };
		_float4		vPosition = { };
		_float4		vReservePosition = { };

		vector<pair<_int, _int>>	DropList;
	} BOSS_CHEST;

private:
	CBossChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossChest(const CBossChest& rhs);
	virtual ~CBossChest() = default;

private:
	class CInteraction_Box*	m_pInteractionBox = { nullptr };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

private:
	LEVELID						m_eLevel = { LEVEL_END };

	_bool						m_bOpened = { FALSE };
	_bool						m_bGrowned = { FALSE };
	vector<pair<_int, _int>>	m_DropList;

	_float4						m_vReservedPosition = { };

private:
	_bool			isInPlayer();

private:
	HRESULT			Add_Component(void* pArg);
	HRESULT			Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CBossChest*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END