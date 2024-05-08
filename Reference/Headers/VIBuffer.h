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
	/* ���� ������ ���� */
	_uint							m_iNumVertexBuffers = { 0 };

	/* Create Buffer Data */
	D3D11_BUFFER_DESC				m_BufferDesc = { 0 };			/* ������ �Ӽ��� ���� */
	D3D11_SUBRESOURCE_DATA			m_InitialData = { 0 };			/* �޸𸮿� ä�� �ʱ� �� */

	/* Vertex Buffer, Index Buffer (�迭) */
	ID3D11Buffer*					m_pVB = { nullptr };
	_float3*						m_pVerticesPos = { nullptr };

	ID3D11Buffer*					m_pIB = { nullptr };
	_ulong*							m_pIndices = { nullptr };

	/* VB, IB Stride(ũ��) */
	_uint							m_iVertexStride = { 0 };		/* ���� �ϳ��� ũ�� */
	_uint							m_iIndexStride = { 0 };			/* 2Byte or 4Byte(���� ���� 65,535�� �̻�) */

	_uint							m_iNumVertices = { 0 };			/* ������ ���� */
	_uint							m_iNumIndices = { 0 };			/* �ε��� �迭�� ���� */

	DXGI_FORMAT						m_eIndexFormat = { (DXGI_FORMAT)0 };					/* �ε��� �迭 �� ĭ�� ũ�� */
	D3D11_PRIMITIVE_TOPOLOGY		m_ePrimitiveTopology = { (D3D11_PRIMITIVE_TOPOLOGY)0 };	/* �ﰢ�� Ÿ�� */

protected:
	/* VB�� IB�� ���� */
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