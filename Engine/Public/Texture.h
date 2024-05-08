#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

private:
	_uint									m_iNumTextures = { 0 };
	_float									m_fCurrentTexture = { 0.f };
	vector<ID3D11ShaderResourceView*>		m_Textures;

private:
	HRESULT	CreateTGATextureFromFile(_In_ ID3D11Device* d3dDevice,
		_In_z_ const wchar_t* szFileName,
		_Outptr_opt_ ID3D11Resource** texture,
		_Outptr_opt_ ID3D11ShaderResourceView** textureView,
		_In_ size_t maxsize = 0);

public:
	_uint	Get_NumTexture() { return m_iNumTextures; }

	_float	Get_CurrentTexture() { return m_fCurrentTexture; }
	void	Set_CurrentTexture(_float fNumTexture);

	ID3D11ShaderResourceView*	Get_ShaderResourceView(_uint iSRVIndex = 0);

public:
	void	Set_NextAnimTexture(_float fTimeDelta, _float fScale, _float fMinTexture = 0.f, _float fMaxTexture = -1.f);

public:
	HRESULT Bind_ShaderCurrentTexture(class CShader* pShader, const _char* pConstantName);

	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex = 0);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CTexture*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END