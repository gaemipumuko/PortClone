#include "RenderTarget_Manager.h"

#include "RenderTarget.h"

#include "GameInstance.h"

CRenderTarget_Manager::CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget_Manager::Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float& fValue)
{
	if (Find_RenderTarget(strTargetTag) != nullptr)
		return E_FAIL;

	CRenderTarget* pTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, fValue);
	if (pTarget == nullptr)
		return E_FAIL;

	m_RenderTargets.insert({ strTargetTag, pTarget });

	return S_OK;
}

HRESULT CRenderTarget_Manager::Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor)
{
	if (Find_RenderTarget(strTargetTag) != nullptr)
		return E_FAIL;

	CRenderTarget* pTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vColor);
	if (pTarget == nullptr)
		return E_FAIL;

	m_RenderTargets.insert({ strTargetTag, pTarget });

	return S_OK;
}

HRESULT CRenderTarget_Manager::Create_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, _int iArraySize)
{
	if (Find_RenderTarget(strTargetTag) != nullptr)
		return E_FAIL;

	CRenderTarget* pTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vColor, iArraySize);
	if (pTarget == nullptr)
		return E_FAIL;

	m_RenderTargets.insert({ strTargetTag, pTarget });

	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_RenderTarget(CShader* pShader, _char* pConstantName, const wstring& strTargetTag)
{
	CRenderTarget* pTarget = Find_RenderTarget(strTargetTag);
	if (pTarget == nullptr)
		return E_FAIL;

	return pTarget->Bind_RenderTargetView(pShader, pConstantName);
}

HRESULT CRenderTarget_Manager::Add_MultiRenderTarget(const wstring& strMRTTag, const wstring& strTargetTag)
{
	CRenderTarget* pTarget = Find_RenderTarget(strTargetTag);
	if (pTarget == nullptr)
		return E_FAIL;

	list<class CRenderTarget*>* pMRTList = Find_MultiRenderTarget(strMRTTag);
	if (pMRTList == nullptr)
	{
		list<class CRenderTarget*> MRTList;
		MRTList.push_back(pTarget);

		m_MultiRenderTargets.insert({ strMRTTag, MRTList });
	}
	else
		pMRTList->push_back(pTarget);

	Safe_AddRef(pTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MultiRenderTarget(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencilView, _bool bBackBuffer, _bool bClearBuffer)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MultiRenderTarget(strMRTTag);
	if (pMRTList == nullptr)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	ID3D11DepthStencilView* pDSV = { nullptr };

	if (nullptr == pDepthStencilView)
		pDSV = m_pMainDepthStencilView;
	else
	{
		pDSV = pDepthStencilView;
		m_pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	ID3D11RenderTargetView* pRTVs[8] = { nullptr };

	_uint iNumViews = { 0 };
	for (auto& pRenderTarget : *pMRTList)
	{
		if (bClearBuffer == TRUE)
			pRenderTarget->Clear_RenderTargetView();

		pRTVs[iNumViews++] = pRenderTarget->Get_RenderTargetView();
	}

	if (bBackBuffer == TRUE)
		pRTVs[iNumViews++] = m_pBackBufferRTV;
	
	/* 장치에 렌더타겟들을 바인딩한다. */
	if (iNumViews < 1 || iNumViews > 8)
		return E_FAIL;

	m_pContext->OMSetRenderTargets(iNumViews, pRTVs, pDSV);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_NonDepthMultiRenderTarget(const wstring& strMRTTag, _bool bBackBuffer, _bool bClearBuffer)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MultiRenderTarget(strMRTTag);
	if (pMRTList == nullptr)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	ID3D11RenderTargetView* pRTVs[8] = { nullptr };

	_uint iNumViews = { 0 };
	for (auto& pRenderTarget : *pMRTList)
	{
		if (bClearBuffer == TRUE)
			pRenderTarget->Clear_RenderTargetView();

		pRTVs[iNumViews++] = pRenderTarget->Get_RenderTargetView();
	}

	if (bBackBuffer == TRUE)
		pRTVs[iNumViews++] = m_pBackBufferRTV;

	/* 장치에 렌더타겟들을 바인딩한다. */
	if (iNumViews < 1 || iNumViews > 8)
		return E_FAIL;

	m_pContext->OMSetRenderTargets(iNumViews, pRTVs, NULL);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MultiRenderTarget()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);

	return S_OK;
}

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CRenderTarget_Manager::Find_MultiRenderTarget(const wstring& strMRTTag)
{
	auto iter = m_MultiRenderTargets.find(strMRTTag);

	if (iter == m_MultiRenderTargets.end())
		return nullptr;

	return &iter->second;
}

ID3D11ShaderResourceView* CRenderTarget_Manager::Find_ShaderResourceView(const wstring& strTargetTag)
{
	auto iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second->Get_ShaderResourceView();
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_Debug(const wstring& strMTRTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	list<CRenderTarget*>* pMRTList = Find_MultiRenderTarget(strMTRTag);

	for (auto& pTarget : *pMRTList)
	{
		pTarget->Render(pShader, pVIBuffer);
	}

	return S_OK;
}
#endif

HRESULT CRenderTarget_Manager::Initialize()
{
	return S_OK;
}

CRenderTarget_Manager* CRenderTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderTarget_Manager* pInstance = new CRenderTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CRenderTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_MultiRenderTargets)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MultiRenderTargets.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
