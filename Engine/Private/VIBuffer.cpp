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
    /* �ν��Ͻ��� ���� �迭�� */
    ID3D11Buffer*       pVertexBuffer[] = {
        m_pVB,
    };

    _uint				iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint				iOffsets[] = {
        0,
    };

    /* ���� ���۵��� ��ġ�� ���ε� (��ġ�� ���� ���� ���� ���۸� ���ÿ� ���ε��� �� ����) */
    /* ���ε� ���� ����, ���� ������ ����, ���� ���� �迭(����������), ���� �ϳ� ũ���� �迭(������), VB�� �޸� ���� ����Ʈ �迭(������) */
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);

    /* �ε��� ���۸� ��ġ�� ���ε� (�ε��� ���۴� �ϳ��� ���ε�) */
    /* �ε��� ����, �ε��� �ϳ� ũ��, IB�� �޸� ���� ����Ʈ */
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    
    /* �ε����� � ������� �׸� ������ (TriangleList) */
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
    //m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    
    /* �ε��� ���۸� �̿��ؼ� �׸� (0��° IB, VB���� �׸�) */
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
