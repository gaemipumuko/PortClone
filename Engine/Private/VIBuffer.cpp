#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
    : CComponent(rhs)
    , m_pVB(rhs.m_pVB)
    , m_pIB(rhs.m_pIB)
    , m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
    , m_iVertexStride(rhs.m_iVertexStride)
    , m_iIndexStride(rhs.m_iIndexStride)
    , m_iNumVertices(rhs.m_iNumVertices)
    , m_iNumIndices(rhs.m_iNumIndices)
    , m_eIndexFormat(rhs.m_eIndexFormat)
    , m_ePrimitiveTopology(rhs.m_ePrimitiveTopology)
    , m_pVerticesPos(rhs.m_pVerticesPos)
    , m_pIndices(rhs.m_pIndices)
{
    Safe_AddRef(m_pVB);
    Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize_Clone(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    /* 인스턴싱을 위한 배열들 */
    ID3D11Buffer*       pVertexBuffer[] = {
        m_pVB,
    };

    _uint				iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint				iOffsets[] = {
        0,
    };

    /* 정점 버퍼들을 장치에 바인딩 (장치에 여러 개의 정점 버퍼를 동시에 바인딩할 수 있음) */
    /* 바인딩 시작 슬롯, 정점 버퍼의 개수, 정점 버퍼 배열(이중포인터), 정점 하나 크기의 배열(포인터), VB의 메모리 시작 바이트 배열(포인터) */
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);

    /* 인덱스 버퍼를 장치에 바인딩 (인덱스 버퍼는 하나만 바인딩) */
    /* 인덱스 버퍼, 인덱스 하나 크기, IB의 메모리 시작 바이트 */
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    
    /* 인덱스로 어떤 방식으로 그릴 것인지 (TriangleList) */
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
    //m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    
    /* 인덱스 버퍼를 이용해서 그림 (0번째 IB, VB부터 그림) */
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);   

    return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppBuffer)
{
    if (m_pDevice == nullptr)
        return E_FAIL;

    return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppBuffer);
}

void CVIBuffer::Free()
{
    __super::Free();

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
}
