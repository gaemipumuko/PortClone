#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CBrush final : public CBase
{
private:
	CBrush();
	virtual ~CBrush() = default;

public:
	_bool Get_State() { return m_bActivate; }

	void Set_State(_bool bValue) { m_bActivate = bValue; }

public:
	HRESULT Initialize();
	HRESULT Render();

public:
	HRESULT Invalidate_Brush(_float4* pBrushPos);
	HRESULT Resize_Brush(_float fScale);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_bool m_bActivate = false;

public:
	static CBrush* Create();
	virtual void Free() override;
};

END
