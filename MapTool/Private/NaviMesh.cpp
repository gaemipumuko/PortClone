#include "NaviMesh.h"
#include "Triangle.h"
#include "Shader.h"
#include "GameInstance.h"

_float4x4	CNaviMesh::m_WorldMatrix = {};

CNaviMesh::CNaviMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CComponent(rhs)
	, m_pTriangles(rhs.m_pTriangles)
#ifdef _DEBUG
	, m_pDebugShader(rhs.m_pDebugShader)
#endif
{
	for (auto& pTriangle : m_pTriangles)
		Safe_AddRef(pTriangle);

#ifdef _DEBUG
	Safe_AddRef(m_pDebugShader);
#endif
}

HRESULT CNaviMesh::Initialize_Prototype(const wstring& strDataFilePath)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	ifstream inFile(strDataFilePath.c_str(), ios::binary | ios::in);

	if (inFile.is_open() == TRUE) 
	{
		inFile.read(reinterpret_cast<char*>(&m_iNumTriangle), sizeof _int);
		m_pTriangles.reserve(m_iNumTriangle);

		for (_int i = 0; i < m_iNumTriangle; ++i)
		{
			_float3	vPoints[3] = {};
			inFile.read(reinterpret_cast<char*>(&vPoints), sizeof _float3 * 3);

			_int	Indices[3] = { };
			inFile.read(reinterpret_cast<char*>(&Indices), sizeof _int * 3);

			CTriangle* pTriangle = CTriangle::Create(m_pDevice, m_pContext, vPoints, Indices, i);
			
			if (nullptr == pTriangle)
				return E_FAIL;

			m_pTriangles.push_back(pTriangle);
		}
	}

#ifdef _DEBUG
	m_pDebugShader = CShader::Create(m_pDevice, m_pContext, VTXPOS::VertexElements, VTXPOS::iNumElements, TEXT("../../Reference/ShaderFiles/Shader_Cell.hlsl"));
	
	if (nullptr == m_pDebugShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNaviMesh::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_iCurrentCellIndex = ((NAVIGATION_DESC*)pArg)->iCurrentCellIndex;
	}



	return S_OK;
}

void CNaviMesh::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool CNaviMesh::isMove(_fvector vPosition)
{
	/* 셀의 세개 점이 로컬위치였기 때문에 판단하고자하는 위치르 ㄹ로컬로 바꾼다. */
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = -1;

	/* 현재 존재하던 삼각형 안에서 움직였다. */
	if (true == m_pTriangles[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex))
		return true;
	/* 현재 존재하던 삼각형 밖으로 움직였다. */
	else
	{
		/* 나간 방향에 이웃이 있었다. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_pTriangles[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
					break;
			}

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		/* 나간 방향에 이웃이 없었다. */
		else
		{
			return false;
		}
	}
}

HRESULT CNaviMesh::Load_NaviMesh(const wstring& strDataFilePath)
{
	ifstream inFile(strDataFilePath.c_str(), ios::binary | ios::in);

	if (inFile.is_open() == FALSE)
		return E_FAIL;

	inFile.read(reinterpret_cast<char*>(&m_iNumTriangle), sizeof _int);
	m_pTriangles.reserve(m_iNumTriangle);

	for (_int i = 0; i < m_iNumTriangle; ++i)
	{
		_float3	vPoints[3] = {};
		inFile.read(reinterpret_cast<char*>(&vPoints), sizeof _float3 * 3);

		_int	Indices[3] = { };
		inFile.read(reinterpret_cast<char*>(&Indices), sizeof _int * 3);

		CTriangle* pTriangle = CTriangle::Create(m_pDevice, m_pContext, vPoints, Indices, i);

		if (nullptr == pTriangle)
			return E_FAIL;

		m_pTriangles.push_back(pTriangle);
	}

	return S_OK;
}

HRESULT CNaviMesh::Add_Triangle(_float3* vPoints)
{
	_float3 vPointsInfo[3];

	vPointsInfo[0] = *vPoints;
	vPointsInfo[1] = *(vPoints + 1);
	vPointsInfo[2] = *(vPoints + 2);

	CTriangle* pTriangle = CTriangle::Create(m_pDevice, m_pContext, vPointsInfo, (_int)m_pTriangles.size());

	if (nullptr == pTriangle)
		return E_FAIL;

	m_pTriangles.push_back(pTriangle);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNaviMesh::Delete_Triangle(_float3 vPoint)
{
	for (size_t i = 0; i < m_pTriangles.size(); i++)
	{
		_bool bDelete = false;

		for (_int j = 0; j < 3; j++)
		{
			_vector vTrianglePoint = m_pTriangles[i]->Get_Point((CTriangle::POINT)j);
			_vector PointDir = XMVector3Length(XMLoadFloat3(&vPoint) - vTrianglePoint);

			if (0.5f > XMVectorGetX(PointDir))
			{
				Safe_Release(m_pTriangles[i]);
				m_pTriangles.erase(m_pTriangles.begin() + i);
				bDelete = true;
				break;
			}
		}

		if (true == bDelete)
			break;
	}

	SetUp_Neighbors();

	return S_OK;
}

HRESULT CNaviMesh::Search_Point(_float3* pPoint)
{
	if (nullptr == pPoint)
		return E_FAIL;

	for (size_t i = 0; i < m_pTriangles.size(); i++)
	{
		for (size_t j = 0; j < CTriangle::POINT_END; j++)
		{
			_vector vCellPoint = m_pTriangles[i]->Get_Point((CTriangle::POINT)j);

			_vector PointDir = XMVector3Length(XMLoadFloat3(pPoint) - vCellPoint);

			if (0.5f > XMVectorGetX(PointDir))
			{
				XMStoreFloat3(pPoint, m_pTriangles[i]->Get_Point((CTriangle::POINT)j));
			}
		}
	}

	return S_OK;
}

HRESULT CNaviMesh::Save_Triangles()
{
	ofstream OutFile(TEXT("../../Reference/Resources/Data/Navigation/Navigation.dat"), ios::binary);

	if (OutFile.is_open())
	{
		_int iNumTriangle = (_int)m_pTriangles.size();

		OutFile.write(reinterpret_cast<_char*>(&iNumTriangle), sizeof(_int));

		for (_int i = 0; i < iNumTriangle; i++)
		{
			CTriangle* pTriangle = m_pTriangles[i];

			_float3* pPoints = pTriangle->Get_Points();
			_int* pNeighbors = pTriangle->Get_Neighbors();

			_float3 vPoints[CTriangle::POINT_END];
			_int iNeighbors[CTriangle::LINE_END];

			vPoints[CTriangle::POINT_A] = pPoints[CTriangle::POINT_A];
			vPoints[CTriangle::POINT_B] = pPoints[CTriangle::POINT_B];
			vPoints[CTriangle::POINT_C] = pPoints[CTriangle::POINT_C];

			iNeighbors[CTriangle::LINE_AB] = pNeighbors[CTriangle::LINE_AB];
			iNeighbors[CTriangle::LINE_BC] = pNeighbors[CTriangle::LINE_BC];
			iNeighbors[CTriangle::LINE_CA] = pNeighbors[CTriangle::LINE_CA];

			OutFile.write(reinterpret_cast<char*>(&vPoints), sizeof(_float3) * 3);
			OutFile.write(reinterpret_cast<char*>(&iNeighbors), sizeof(_int) * 3);
		}

		OutFile.close();
	}

	return S_OK;
}

#ifdef _DEBUG

HRESULT CNaviMesh::Render()
{
	if (FAILED(m_pDebugShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pDebugShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4x4 RenderMatrix = m_WorldMatrix;
	RenderMatrix._42 += 0.1f;

	if (FAILED(m_pDebugShader->Bind_Matrix("g_WorldMatrix", RenderMatrix)))
		return E_FAIL;

	m_pDebugShader->Bind_RawValue("g_vColor", Colors::Green, sizeof(_float4));

	if (FAILED(m_pDebugShader->Begin(0)))
		return E_FAIL;

	for (auto& pTriangle : m_pTriangles)
		pTriangle->Render();

	return S_OK;
}

#endif

HRESULT CNaviMesh::SetUp_Neighbors()
{
	for (auto& pSrcTriangle : m_pTriangles)
	{
		for (auto& pDstTriangle : m_pTriangles)
		{
			if (pSrcTriangle == pDstTriangle)
				continue;

			if (true == pDstTriangle->Compare_Points(pSrcTriangle->Get_Point(CTriangle::POINT_A), pSrcTriangle->Get_Point(CTriangle::POINT_B)))
			{
				pSrcTriangle->Set_Neighbor(CTriangle::LINE_AB, pDstTriangle);
			}

			if (true == pDstTriangle->Compare_Points(pSrcTriangle->Get_Point(CTriangle::POINT_B), pSrcTriangle->Get_Point(CTriangle::POINT_C)))
			{
				pSrcTriangle->Set_Neighbor(CTriangle::LINE_BC, pDstTriangle);
			}

			if (true == pDstTriangle->Compare_Points(pSrcTriangle->Get_Point(CTriangle::POINT_C), pSrcTriangle->Get_Point(CTriangle::POINT_A)))
			{
				pSrcTriangle->Set_Neighbor(CTriangle::LINE_CA, pDstTriangle);
			}
		}
	}

	return S_OK;
}

CNaviMesh* CNaviMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFilePath)
{
	CNaviMesh* pInstance = new CNaviMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strDataFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CNaviMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNaviMesh::Clone(void* pArg)
{
	CNaviMesh* pInstance = new CNaviMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNaviMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaviMesh::Free()
{
	__super::Free();

	for (auto& pTriangle : m_pTriangles)
		Safe_Release(pTriangle);

	m_pTriangles.clear();

#ifdef _DEBUG
	Safe_Release(m_pDebugShader);
#endif
}
