#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_iNumPass(rhs.m_iNumPass)
	, m_InputLayouts(rhs.m_InputLayouts)
{
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (pTechnique == nullptr)
		return E_FAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);
	if (pPass == nullptr)
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	pPass->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader::Bind_ShaderResourceView(const _char* pConstantName, ID3D11ShaderResourceView* pTexture)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	/* Shader 안에 선언되어 있는 특정 이름을 가진 전역 변수를 Get (void*와 같음) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	/* Texture 타입으로 형변환 */
	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (pShaderResourceVariable == nullptr)
		return E_FAIL;

	/* 전역 변수에 Texture를 Bind */
	pShaderResourceVariable->SetResource(pTexture);

	return S_OK;
}

HRESULT CShader::Bind_ShaderResourceViews(const _char* pConstantName, ID3D11ShaderResourceView** ppTextures, _uint iNumTexture)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	/* Shader 안에 선언되어 있는 특정 이름을 가진 전역 변수를 Get (void*와 같음) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	/* Texture 타입으로 형변환 */
	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (pShaderResourceVariable == nullptr)
		return E_FAIL;

	/* 전역 변수에 Texture를 Bind (배열) */
	pShaderResourceVariable->SetResourceArray(ppTextures, 0, iNumTexture);

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4& Matrix)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	/* Shader 안에 선언되어 있는 특정 이름을 가진 전역 변수를 Get (void*와 같음) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	/* Matrix 타입으로 형변환 */
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (pMatrixVariable == nullptr)
		return E_FAIL;

	/* 전역 변수에 Matrix를 Bind */
	pMatrixVariable->SetMatrix((_float*)&Matrix);

	return S_OK;
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrix)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	/* Shader 안에 선언되어 있는 특정 이름을 가진 전역 변수를 Get (void*와 같음) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	/* Matrix 타입으로 형변환 */
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (pMatrixVariable == nullptr)
		return E_FAIL;

	/* 전역 변수에 Matrix를 Bind (배열) */
	pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrix);

	return S_OK;
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	/* Shader 안에 선언되어 있는 특정 이름을 가진 전역 변수를 Get (void*와 같음) */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	/* 전역 변수에 Texture, Matrix(전치 발생), Bool 타입을 제외하고 Bind */
	if (FAILED(pVariable->SetRawValue(pData, 0, iLength)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Initialize_Prototype(const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const wstring& strShaderFilePath)
{
	_uint		iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	/* Create InputLayout */
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	pTechnique->GetDesc(&TechniqueDesc);
	m_iNumPass = TechniqueDesc.Passes;

	for (_uint idx = 0; idx < m_iNumPass; ++idx)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(static_cast<uint32_t>(idx));

		D3DX11_PASS_DESC	PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);

		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const wstring& strShaderFilePath)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pElements, iNumElements, strShaderFilePath)))
	{
		MSG_BOX(TEXT("Failed to Create : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);
	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
