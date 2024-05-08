#pragma once

#include "ComputeShader_Defines.h"

BEGIN(Engine)

class CComputeShader final : public CBase
{
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComputeShader() = default;

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pMainDepthStencilView = { nullptr };

private: /* For. Bloom */
	class CShader*				m_pShader = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix = { };
	_float4x4					m_ViewMatrix = { }, m_ProjMatrix = { };

private:
	_uint						m_iFlag = { };
	
	RT_DESC*					m_pAvgLumTarget = { nullptr };
	RT_DESC*					m_pBloomTarget = { nullptr };
	list<RT_DESC*>				m_pRenderTargets;

	map<wstring, VS_DESC>		m_VertexShaders;
	map<wstring, PS_DESC>		m_PixelShaders;
	map<wstring, CS_DESC>		m_ComputeShaders;

	map<wstring, ID3D11Buffer*>	m_ConstantBuffers;

public:
	HRESULT		Ready_Shader();

	HRESULT		Ready_VertexShader();
	HRESULT		Ready_PixelShader();
	HRESULT		Ready_ComputeShader();
	HRESULT		Ready_SamplerState();
	HRESULT		Ready_ConstantBuffer();

	RT_DESC*	Ready_RenderTarget(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _uint iMipLevels, _bool bUAV, _bool bStore = TRUE);
	HRESULT		Ready_VertexShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry);
	HRESULT		Ready_PixelShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry);
	HRESULT		Ready_ComputeShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry);

public:
#ifdef _DEBUG
	HRESULT		Update_BloomCB(_float fBloomScale, _float fBloomThreshold);
#endif
	void	Set_ViewPort(_uint iSizeX, _uint iSizeY);
	void	Execute_PixelShader(ID3D11ShaderResourceView* pSRV, ID3D11RenderTargetView* pRTV, const wstring& strShaderTag);

	void	Set_CSInputs(ID3D11ShaderResourceView* pSRV0, ID3D11ShaderResourceView* pSRV1 = NULL,
		ID3D11ShaderResourceView* pSRV2 = NULL, ID3D11ShaderResourceView* pSRV3 = NULL);
	void	Clear_CSInputs();

	void	Set_CSOutputs(ID3D11UnorderedAccessView* pUAV0, ID3D11UnorderedAccessView* pUAV1 = NULL,
		ID3D11UnorderedAccessView* pUAV2 = NULL, ID3D11UnorderedAccessView* pUAV3 = NULL);
	void	Clear_CSOutputs();
	
public:
	HRESULT Initialize();
	HRESULT Render(ID3D11ShaderResourceView* pSceneSRV);

	HRESULT	Calculate_AvgLum(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT	Calculate_BloomRTV(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT	Render_ToneMapping(ID3D11ShaderResourceView* pSceneSRV);

public:
	static CComputeShader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END