#pragma once

#include "Component.h"

/* HLSL, FX Ȯ���� ������ DX�� ���� �о ���� */
/* �ۼ��� ���̴� ������ �����Ͽ� ��üȭ */

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

private:
	ID3DX11Effect*				m_pEffect = { nullptr };

	_uint						m_iNumPass = { 0 };
	vector<ID3D11InputLayout*>	m_InputLayouts;

public:
	HRESULT Begin(_uint iPassIndex);

public:
	HRESULT Bind_ShaderResourceView(const _char* pConstantName, ID3D11ShaderResourceView* pTexture);
	HRESULT Bind_ShaderResourceViews(const _char* pConstantName, ID3D11ShaderResourceView** ppTextures, _uint iNumTexture);

	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4& Matrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrix);

	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);

public:
	virtual HRESULT Initialize_Prototype(const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const wstring& strShaderFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CShader*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const wstring& strShaderFilePath);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END