#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CDungeonExit final : public CGameObject
{
public:
	typedef struct tagDungeonExitDesc
	{
		LEVELID		eLevel = { };

		_float4		vExitPosition = { };
		_float4		vReservePosition = { };
	} DUNGEON_EXIT;

private:
	CDungeonExit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDungeonExit(const CDungeonExit& rhs);
	virtual ~CDungeonExit() = default;

private:
	class CInteraction_Box*	m_pInteractionBox = { nullptr };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

private:
	LEVELID			m_eLevel = { LEVEL_END };
	_float4			m_vReservePosition = { };

	_bool			m_bReserveLevel = { FALSE };

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
	static CDungeonExit*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END