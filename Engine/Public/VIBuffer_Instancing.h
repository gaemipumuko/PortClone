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

	ID3D11Buffer*			m_pVBInstance = { nullptr };		/* Instance의 VB */

	_uint					m_iNumInstance = { };				/* Instance의 개수 */
	_uint					m_iInstanceStride = { };			/* Instance 정점 하나의 크기 */
	_uint					m_iIndexCountPerInstance = { };		/* 하나의 인스턴스를 그릴 때 몇개를 인덱스 버퍼에서 가져올 것인지 */

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render() override;

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void		Free() override;
};

END