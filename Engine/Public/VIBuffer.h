#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

protected:
	/* 정점 버퍼의 개수 */
	_uint							m_iNumVertexBuffers = { 0 };

	/* Create Buffer Data */
	D3D11_BUFFER_DESC				m_BufferDesc = { 0 };			/* 버퍼의 속성을 정의 */
	D3D11_SUBRESOURCE_DATA			m_InitialData = { 0 };			/* 메모리에 채울 초기 값 */

	/* Vertex Buffer, Index Buffer (배열) */
	ID3D11Buffer*					m_pVB = { nullptr };
	_float3*						m_pVerticesPos = { nullptr };

	ID3D11Buffer*					m_pIB = { nullptr };
	_ulong*							m_pIndices = { nullptr };

	/* VB, IB Stride(크기) */
	_uint							m_iVertexStride = { 0 };		/* 정점 하나의 크기 */
	_uint							m_iIndexStride = { 0 };			/* 2Byte or 4Byte(정점 개수 65,535개 이상) */

	_uint							m_iNumVertices = { 0 };			/* 정점의 개수 */
	_uint							m_iNumIndices = { 0 };			/* 인덱스 배열의 개수 */

	DXGI_FORMAT						m_eIndexFormat = { (DXGI_FORMAT)0 };					/* 인덱스 배열 한 칸의 크기 */
	D3D11_PRIMITIVE_TOPOLOGY		m_ePrimitiveTopology = { (D3D11_PRIMITIVE_TOPOLOGY)0 };	/* 삼각형 타입 */

protected:
	/* VB와 IB를 생성 */
	HRESULT Create_Buffer(ID3D11Buffer** ppBuffer);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render();

public:
	virtual CComponent*		Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END