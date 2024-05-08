#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CWorldElement final : public CBase
{
public:
	typedef struct tagElementDesc 
	{
		LEVELID eLevel;
		WORLD_ELEMENT_TYPE eType;
		_float4x4 vWorldMatrix;
		wstring strModelTag;
	}ELEMENT_DESC;

private:
	CWorldElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWorldElement() = default;

public:
	HRESULT Initialize(ELEMENT_DESC* pElementDesc);
	HRESULT Tick(_float fTimeDelta);
	HRESULT Render();
	HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray);

public:
	_float4 Get_Position();
	void Set_Show(_bool bShow) { m_bShow = bShow; }
	CCollider* Get_Collider() { return m_pColliderCom; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	LEVELID m_eLevel = LEVEL_END;
	WORLD_ELEMENT_TYPE m_eType = WORLD_ELEMENT_END;
	_float4x4 m_vWorldMatrix;
	wstring m_strModelTag;
	_bool m_bShow = true;

public:
	static CWorldElement* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ELEMENT_DESC* pElementDesc);
	virtual void Free() override;
};

END
