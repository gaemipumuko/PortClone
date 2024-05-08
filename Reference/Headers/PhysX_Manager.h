#pragma once

#include "Base.h"

BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
private:
	CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysX_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Init_Scene();
	HRESULT PhysX_Simulate(_float fTimeDelta);

public:
	HRESULT Create_StaticMesh(_uint iNumVertices, _float3* pVerticesPos, _uint iIndices, _ulong* pIndices, _float4x4 vWorldMatrix, PxRigidStatic** ppOutRigidStatic = nullptr);
	PxController* Create_Controller(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, PxFilterData tControllerFilterData);
	_bool RayCast_Position(_float3 vOrigin, _float3 vUnitDir, _float fMaxDistance, _float3* pHitPoint, PxQueryFilterData tQueryFilterData);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = {nullptr};

private:
	PxDefaultErrorCallback m_DefaultErrorCallback;
	PxDefaultAllocator m_DefaultAllocator;

	PxFoundation* m_pFoundation = { nullptr };
	PxPvd* m_pPvd = { nullptr };
	PxPhysics* m_pPhysicsSDK = { nullptr };

	// PxCudaContextManager* m_pCudaContextManager = { nullptr };

	PxScene* m_pScene = { nullptr };
	PxDefaultCpuDispatcher* m_pDispatcher = { nullptr };
	
	PxMaterial* m_pMaterial = { nullptr };

	PxControllerManager* m_pControllerManager = { nullptr };

public:
	static CPhysX_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

END
