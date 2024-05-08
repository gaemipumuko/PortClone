#pragma once

#include "ES_CodeDefine.h"

BEGIN(Engine)

class CExecuteShader final : public CBase
{
private:
	CExecuteShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CExecuteShader() = default;

private:
	class CGameInstance*		m_pGameInstance = { nullptr };

	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView*		m_pMainDepthStencilView = { nullptr };

private:
	_bool						m_bInvenBlur = { FALSE };

public:
	void						Set_InvenBlur(_bool bInvenBlur) { m_bInvenBlur = bInvenBlur; }

private:
	_uint						m_iFlag = { };

	_uint						m_iLUTIndex = { 31 };
	class CTexture*				m_pLUTTexture = { };

	RT_DESC*					m_pAvgLumTarget = { nullptr };
	RT_DESC*					m_pBloomTarget = { nullptr };
	RT_DESC*					m_pProcessedTarget = { nullptr };
	RT_DESC*					m_pInvenBlurTarget = { nullptr };
	list<RT_DESC*>				m_pRenderTargets;

	map<wstring, VS_DESC>		m_VertexShaders;
	map<wstring, PS_DESC>		m_PixelShaders;
	map<wstring, CS_DESC>		m_ComputeShaders;

	ID3D11BlendState*					m_pBlendState;
	map<wstring, ID3D11SamplerState*>	m_SamplerStates;
	map<wstring, ID3D11Buffer*>			m_ConstantBuffers;

public:
	HRESULT		Ready_Shader();

	HRESULT		Ready_VertexShader();
	HRESULT		Ready_PixelShader();
	HRESULT		Ready_ComputeShader();

	HRESULT		Ready_BlendState();
	HRESULT		Ready_SamplerState();
	HRESULT		Ready_ConstantBuffer();

	RT_DESC*	Ready_RenderTarget(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _bool bUAV, _bool bStore = TRUE);
	HRESULT		Ready_VertexShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry);
	HRESULT		Ready_PixelShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry);
	HRESULT		Ready_ComputeShader(const wstring& strShaderTag, const wstring& strShaderFilePath, const string& strEntry);

public:
	HRESULT		Update_LUTIndex(_uint iIndex);
	HRESULT		Update_WindowCB(_float fWINCX, _float fWINCY);
	HRESULT		Update_BloomCB(_float fBloomScale, _float fBloomThreshold);
	HRESULT		Update_ColorGradeCB(_float3 vColorFilter, _float fSaturation, _float fExposure, _float fContrast);

	void	Set_ViewPort(_uint iSizeX, _uint iSizeY);
	void	Execute_PixelShader(ID3D11ShaderResourceView* pSRV,  const wstring& strShaderTag);
	void	Execute_PixelShader(ID3D11ShaderResourceView* pSRV, ID3D11RenderTargetView* pRTV, const wstring& strShaderTag);

	void	Set_CSInputs(ID3D11ShaderResourceView* pSRV0, ID3D11ShaderResourceView* pSRV1 = NULL,
		ID3D11ShaderResourceView* pSRV2 = NULL, ID3D11ShaderResourceView* pSRV3 = NULL);
	void	Clear_CSInputs();

	void	Set_CSOutputs(ID3D11UnorderedAccessView* pUAV0, ID3D11UnorderedAccessView* pUAV1 = NULL,
		ID3D11UnorderedAccessView* pUAV2 = NULL, ID3D11UnorderedAccessView* pUAV3 = NULL);
	void	Clear_CSOutputs();
	
public:
	HRESULT Initialize();
	HRESULT Render_Scene(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT Render_Effect(ID3D11ShaderResourceView* pSceneSRV);

	HRESULT	Calculate_AvgLum(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT	Calculate_BloomRTV(ID3D11ShaderResourceView* pSceneSRV);

	HRESULT	Render_Tone(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT	Render_Outline(ID3D11ShaderResourceView* pSceneSRV);
	HRESULT	Render_InvenBlur(ID3D11ShaderResourceView* pSceneSRV);

public:
	static CExecuteShader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void			Free() override;
};

END