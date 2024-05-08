#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

protected:
	D3D11_BUFFER_DESC		m_InstanceBufferDesc = { };
	D3D11_SUBRESOURCE_DATA	m_InstanceInitialData = { };

	ID3D11Buffer*			m_pVBInstance = { nullptr };		/* Instance�� VB */

	_uint					m_iNumInstance = { };				/* Instance�� ���� */
	_uint					m_iInstanceStride = { };			/* Instance ���� �ϳ��� ũ�� */
	_uint					m_iIndexCountPerInstance = { };		/* �ϳ��� �ν��Ͻ��� �׸� �� ��� �ε��� ���ۿ��� ������ ������ */

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render() override;

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void		Free() override;
};

END