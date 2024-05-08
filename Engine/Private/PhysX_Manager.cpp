#include "PhysX_Manager.h"
#include "Shader.h"
#include "GameInstance.h"

CPhysX_Manager::CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPhysX_Manager::Initialize()
{
	/* PhysX ��ü ������ ���� Foundation ��ü ���� */
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, m_DefaultErrorCallback);
	
	if (nullptr == m_pFoundation)
		return E_FAIL;

	/* ���־� �ð�ȭ ����?�� ������ ����� ���� */
	bool recordMemoryAllocations = true;
	
	//m_pPvd = PxCreatePvd(*m_pFoundation);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	//bool bconnect = m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG);

	/* PhysX SDK ��ġ �ʱ�ȭ */
	m_pPhysicsSDK = PxCreatePhysics(
		PX_PHYSICS_VERSION,
		*m_pFoundation,
		PxTolerancesScale(),
		recordMemoryAllocations
		/* m_pPvd */);

	if (nullptr == m_pPhysicsSDK)
		return E_FAIL;

	/* GPU ������ ���� CUDA �Ŵ��� */
	//PxCudaContextManagerDesc tCudaCOntextManagerDesc;
	//m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, tCudaCOntextManagerDesc, PxGetProfilerCallback());

	m_pDispatcher = PxDefaultCpuDispatcherCreate(4);

	/* ���� ������ ���� �Ӽ�?���� ���� ���׸��� ���� */
	m_pMaterial = m_pPhysicsSDK->createMaterial(0.5f, 0.5f, 0.6f);

	return S_OK;
}

HRESULT CPhysX_Manager::Init_Scene()
{
	if (nullptr != m_pControllerManager)
		PX_RELEASE(m_pControllerManager);

	if (nullptr != m_pScene)
		PX_RELEASE(m_pScene);

	/* ���� ������ �ʿ��� ���͵��� ��� �� ��ü ���� */
	PxSceneDesc SceneDesc(m_pPhysicsSDK->getTolerancesScale());
	{
		SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		SceneDesc.cpuDispatcher = m_pDispatcher;
		SceneDesc.filterShader = PxDefaultSimulationFilterShader;

		/* �� GPU ��� ���� */
		//SceneDesc.cudaContextManager = m_pCudaContextManager;
		//SceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
		//SceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
		//SceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	}

	m_pScene = m_pPhysicsSDK->createScene(SceneDesc);

	/*  ����� �ð�ȭ �÷��� ���� */
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);

	/* ����� ���� */
	//PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();

	//if (nullptr != pPvdClient)
	//{
	//	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	/* ��Ʈ�ѷ��� �ʿ��� ��ü�� ���� ��Ʈ�ѷ� �Ŵ��� */
	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	return S_OK;
}

HRESULT CPhysX_Manager::PhysX_Simulate(_float fTimeDelta)
{
	if (nullptr != m_pScene) 
	{
		m_pScene->simulate(fTimeDelta);
		m_pScene->fetchResults(true);
	}

	return S_OK;
}

HRESULT CPhysX_Manager::Create_StaticMesh(_uint iNumVertices, _float3* pVerticesPos, _uint iNumIndices, _ulong* pIndices, _float4x4 vWorldMatrix, PxRigidStatic** ppOutRigidStatic)
{
	if (nullptr == pVerticesPos || nullptr == pIndices)
		return E_FAIL;

	/* ���� ���� ī�� */
	PxArray<PxVec3> Vertices;
	Vertices.resize(iNumVertices);

	for (_uint i = 0; i < iNumVertices; i++)
		Vertices[i] = PxVec3(pVerticesPos[i].x, pVerticesPos[i].y, pVerticesPos[i].z);

	/* �ε��� ���� ī�� */
	PxArray<PxU32> Indices;
	Indices.resize(iNumIndices);
	
	for (_uint j = 0; j < iNumIndices; j++)
		Indices[j] = static_cast<PxU32>(pIndices[j]);

	/* �ݶ��̴� ��ũ���� */
	PxTriangleMeshDesc meshDesc = {};
	{
		meshDesc.points.count = Vertices.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = Vertices.begin();

		meshDesc.triangles.count = iNumIndices / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = Indices.begin();
	}

	PxTolerancesScale scale;
	PxCookingParams params(scale);

	/* Cooking GPU�� �����ϴ� ��� */
	// params.buildGPUData = true;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);

	if (false == status)
		return E_FAIL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	PxTriangleMesh* pTriangleMesh = m_pPhysicsSDK->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry Geom(pTriangleMesh);

	PxVec3 PxRight = PxVec3(vWorldMatrix._11, vWorldMatrix._12, vWorldMatrix._13);
	PxVec3 PxUp = PxVec3(vWorldMatrix._21, vWorldMatrix._22, vWorldMatrix._23);
	PxVec3 PxLook = PxVec3(vWorldMatrix._31, vWorldMatrix._32, vWorldMatrix._33);
	PxVec3 PxPosition = PxVec3(vWorldMatrix._41, vWorldMatrix._42, vWorldMatrix._43);

	PxMat33 PxRotateMatrix = PxMat33(PxRight.getNormalized(), PxUp.getNormalized(), PxLook.getNormalized());
	PxQuat PxRotate = PxQuat(PxRotateMatrix);

	PxTransform PxSRT = PxTransform(PxPosition, PxRotate);

	PxRigidStatic* pRigidStatic = m_pPhysicsSDK->createRigidStatic(PxSRT);
	
	if (nullptr != ppOutRigidStatic)
		*ppOutRigidStatic = pRigidStatic;

	PxShape* pShape = m_pPhysicsSDK->createShape(Geom, *m_pMaterial);
	pRigidStatic->attachShape(*pShape);
	
	m_pScene->addActor(*pRigidStatic);

	pShape->release();

	return S_OK;
}

PxController* CPhysX_Manager::Create_Controller(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, PxFilterData tControllerFilterData)
{
	PxCapsuleControllerDesc ControllerDesc = {};
	{
		ControllerDesc.height = fHeight;
		ControllerDesc.radius = fRadius;
		ControllerDesc.stepOffset = fStepOffset;
		ControllerDesc.position = PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z);
		ControllerDesc.material = m_pMaterial;
	}

	PxController* pController = m_pControllerManager->createController(ControllerDesc);

	PxU32 PxShapeCount = pController->getActor()->getNbShapes();

	vector<PxShape*> pShapes(PxShapeCount);

	pController->getActor()->getShapes(pShapes.data(), PxShapeCount);

	for (PxShape* pShape : pShapes) 
	{
		//pShape->setSimulationFilterData(tControllerFilterData);
		pShape->setQueryFilterData(tControllerFilterData);
	}

	return pController;
}

_bool CPhysX_Manager::RayCast_Position(_float3 vOrigin, _float3 vUnitDir, _float fMaxDistance, _float3* pHitPoint, PxQueryFilterData tQueryFilterData)
{
	PxVec3 PxOrigin = PxVec3(vOrigin.x, vOrigin.y, vOrigin.z);
	PxVec3 PxUnitDir = PxVec3(vUnitDir.x, vUnitDir.y, vUnitDir.z);
	PxReal PxMaxDistance = fMaxDistance;
	PxRaycastBuffer PxHit;

	if (m_pScene->raycast(PxOrigin, PxUnitDir, PxMaxDistance, PxHit, PxHitFlag::eDEFAULT, tQueryFilterData))
	{
		PxVec3 PxHitPoint = PxHit.block.position;

		(*pHitPoint).x = PxHitPoint.x;
		(*pHitPoint).y = PxHitPoint.y;
		(*pHitPoint).z = PxHitPoint.z;

		return true;
	}

	return false;
}

CPhysX_Manager* CPhysX_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysX_Manager* pInstance = new CPhysX_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPhysX_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysX_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	PX_RELEASE(m_pControllerManager);
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysicsSDK);

	/* GPU �� CUDA ������ */
	//PX_RELEASE(m_pCudaContextManager);

	/* ���־� ����� ������ */
	//if (nullptr != m_pPvd) 
	//{
	//	PxPvdTransport* transport = m_pPvd->getTransport();
	//	PX_RELEASE(m_pPvd);
	//	m_pPvd = nullptr;
	//	PX_RELEASE(transport);
	//}

	PX_RELEASE(m_pFoundation);
}
