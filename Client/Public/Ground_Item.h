#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"
#include "ItemGlow.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CGround_Item final : public CDeciderObject
{
public:
	typedef struct tagGroundItemDesc
	{
		_int	iItemIndex, iItemQuantity;
		_vector vPos;

	}DROP_ITEM_DESC;

private:
	CGround_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGround_Item(const CGround_Item& rhs);
	virtual ~CGround_Item() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
 	CCollider*			m_pColliderCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

private:
	_bool				m_bIsOnPlace = {false};

	_float4x4			m_WorldMatrix = {};
	_float				m_fRotatiton = {};
	_float				m_fTime = {};

	_vector				m_vInitialDir = {};

	class CInteraction_Box* m_pInteractionBox = { nullptr };
	class CItem* m_pItem = { nullptr };
	CItem_Glow* m_pItem_Glow = {nullptr};
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
	static CGround_Item*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END