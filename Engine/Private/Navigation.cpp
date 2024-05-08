#include "Navigation.h"

#include "Shader.h"
#include "Cell.h"

#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = { };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pDebugShader(rhs.m_pDebugShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pDebugShader);
#endif
}

_bool CNavigation::isMove(_fvector vPosition)
{
	_int		iNeighborIndex = -1;

	/* 플레이어의 위치 월드 좌표를 Terrain의 로컬 좌표로 변환 */
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	/* 현재 Cell 안에 위치한다면 */
	if (m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex) == TRUE)
		return TRUE;

	/* 나간 방향의 면의 NeighborIndex가 -1이 아니라면 */
	if (iNeighborIndex != -1)
	{
		while (TRUE)
		{
			/* 길이 끊긴 경우 더이상 갈 수 없음 */
			if (iNeighborIndex == -1)
				return FALSE;

			/* 무한히 반복하며 NeighborIndex의 면 안에 있을 때까지 탐색 */
			if (m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex) == TRUE)
				break;

			/* 반복 탐색이 일어난다면 이동할 수 없게 처리 */
			if (iNeighborIndex == m_iCurrentCellIndex)
				return FALSE;
		}

		m_iCurrentCellIndex = iNeighborIndex;
		return TRUE;
	}
	else
		return FALSE;
}

HRESULT CNavigation::Set_CellIndex(_int iCellIndex)
{
	if (iCellIndex < m_Cells.size())
		m_iCurrentCellIndex = iCellIndex;

	return S_OK;
}

HRESULT CNavigation::Set_CellIndex(_fvector vPosition)
{
	_int		iNeighborIndex = -1;

	/* 플레이어의 위치 월드 좌표를 Terrain의 로컬 좌표로 변환 */
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	for (auto& Cell : m_Cells)
	{
		if (Cell->isIn(vLocalPos, &iNeighborIndex))
		{
			m_iCurrentCellIndex = Cell->Get_Index();

			return S_OK;
		}
	}

	return E_FAIL;
}

void CNavigation::Invalidate_Navigation(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_vector CNavigation::Get_CurrCellCenter()
{ 
	return Get_CellCenter(m_iCurrentCellIndex); 
}

_vector CNavigation::Get_CellCenter(_int iCellIndex)
{
	_vector vPointA = m_Cells[iCellIndex]->Get_Point(CCell::POINT::POINT_A);
	_vector vPointB = m_Cells[iCellIndex]->Get_Point(CCell::POINT::POINT_B);
	_vector vPointC = m_Cells[iCellIndex]->Get_Point(CCell::POINT::POINT_C);

	_vector vCenter = {
		(vPointA.m128_f32[0] + vPointB.m128_f32[0] + vPointC.m128_f32[0]) / 3.f,
		(vPointA.m128_f32[1] + vPointB.m128_f32[1] + vPointC.m128_f32[1]) / 3.f,
		(vPointA.m128_f32[2] + vPointB.m128_f32[2] + vPointC.m128_f32[2]) / 3.f,
		1.f
	};

	return vCenter;
}

_float CNavigation::Get_CurrCellHeight(_fvector vPosition)
{
	_vector vPointA = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_A);
	_vector vPointB = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_B);
	_vector vPointC = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_C);

	_vector vPlane = XMPlaneFromPoints(vPointA, vPointB, vPointC);

	_float fHeight = 
		-(vPlane.m128_f32[0] * vPosition.m128_f32[0] + vPlane.m128_f32[2] * vPosition.m128_f32[2] + vPlane.m128_f32[3]) / vPlane.m128_f32[1];

	return fHeight;
}

_vector CNavigation::Get_SlidingVector(_fvector vPosition, _fvector vMoveDir)
{
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	return m_Cells[m_iCurrentCellIndex]->Get_SlidingVector(vLocalPos, vMoveDir);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strDataFilePath)
{
	ifstream inFile(strDataFilePath.c_str(), ios::binary | ios::in);
	if (inFile.is_open() == FALSE) {
		return E_FAIL;
	}

	/* Total Cell Count */
	inFile.read(reinterpret_cast<char*>(&m_iNumCell), sizeof _int);
	m_Cells.reserve(m_iNumCell);

	/* Create Cell */
	for (_int iCell = 0; iCell < m_iNumCell; ++iCell)
	{
		_float3	vPoints[3] = { };
		inFile.read(reinterpret_cast<char*>(&vPoints), sizeof _float3 * 3);

		_int	Indices[3] = { };
		inFile.read(reinterpret_cast<char*>(&Indices), sizeof _int * 3);

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, Indices, iCell);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

#ifdef _DEBUG
	m_pDebugShader = CShader::Create(m_pDevice, m_pContext, VTXPOS::VertexElements, VTXPOS::iNumElements, 
		TEXT("../../Reference/ShaderFiles/Shader_Cell.hlsl"));
	if (m_pDebugShader == nullptr)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Clone(void* pArg)
{
	if (pArg != nullptr)
		m_iCurrentCellIndex = static_cast<NAVIGATION_DESC*>(pArg)->iCurrentCellIndex;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (FAILED(m_pDebugShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pDebugShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4x4		RenderMatrix = m_WorldMatrix;

	if (-1 != m_iCurrentCellIndex)
	{
		RenderMatrix._42 += 0.05f;

		if (FAILED(m_pDebugShader->Bind_Matrix("g_WorldMatrix", RenderMatrix)))
			return E_FAIL;

		m_pDebugShader->Bind_RawValue("g_vColor", Colors::Green, sizeof(_float4));

		if (FAILED(m_pDebugShader->Begin(0)))
			return E_FAIL;

		m_Cells[m_iCurrentCellIndex]->Render();
	}
	else
	{
		if (FAILED(m_pDebugShader->Bind_Matrix("g_WorldMatrix", RenderMatrix)))
			return E_FAIL;

		m_pDebugShader->Bind_RawValue("g_vColor", Colors::Red, sizeof(_float4));

		if (FAILED(m_pDebugShader->Begin(0)))
			return E_FAIL;

		for (auto& pCell : m_Cells)
			pCell->Render();
	}

	return S_OK;
}
#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strDataFilePath)))
	{
		MSG_BOX(TEXT("Failed to Create : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pDebugShader);
#endif
}
