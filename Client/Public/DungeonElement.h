#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CTransform;
class CGameInstance;
END

BEGIN(Client)

class CDungeonElement final : public CBase
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
	CDungeonElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDungeonElement() = default;

public:
	HRESULT Initialize(ELEMENT_DESC* pElementDesc);
	HRESULT Tick(_float fTimeDelta);
	HRESULT Render();
	HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray);

public:
	HRESULT Set_PassID(PASSID_VTXMESH ePassID);

	PASSID_VTXMESH Get_PassID();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };

private:
	LEVELID m_eLevel = LEVEL_END;
	WORLD_ELEMENT_TYPE m_eType = WORLD_ELEMENT_END;
	_float4x4 m_vWorldMatrix;
	wstring m_strModelTag;

private:
	PASSID_VTXMESH m_ePassID = VTXMESH_TERRAIN;

public:
	static CDungeonElement* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ELEMENT_DESC* pElementDesc);
	virtual void Free() override;
};

END
