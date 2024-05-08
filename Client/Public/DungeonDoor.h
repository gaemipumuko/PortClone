#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CDungeonDoor final : public CGameObject
{
public:
	typedef struct tagDDoorDesc
	{
		LEVELID		eDoorLevel = { LEVEL_END };

		KEYFRAME	eTransformDesc = { };
	} DDOOR_DESC;

private:
	CDungeonDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDungeonDoor(const CDungeonDoor& rhs);
	virtual ~CDungeonDoor() = default;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

private:
	_byte			m_byStep = { 0 };
	_bool			m_bReacted = { FALSE };
	_int			m_iCameraScene = { 0 };

public:
	void			Set_Reacted(_bool bReacted) { m_bReacted = bReacted; }

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
	static CDungeonDoor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END