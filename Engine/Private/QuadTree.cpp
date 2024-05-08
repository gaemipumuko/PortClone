#include "QuadTree.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenter = (iLT + iRB) >> 1;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	m_pChild[CORNER_LT] = Create(iLT, iTC, m_iCenter, iLC);
	m_pChild[CORNER_RT] = Create(iTC, iRT, iRC, m_iCenter);
	m_pChild[CORNER_RB] = Create(m_iCenter, iRC, iRB, iBC);
	m_pChild[CORNER_LB] = Create(iLC, m_iCenter, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices, _fmatrix WorldMatrix)
{
	if (nullptr == m_pChild[CORNER_LT] ||
		true == isDraw(pGameInstance, pVerticesPos, WorldMatrix))
	{
		_bool		isIn[4] = {
			pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]), 50.0f),
			pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]), 50.0f),
			pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RB]]), 50.0f),
			pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LB]]), 50.0f),
		};

		/* 현재 내 쿼드트리가 그려질 때, 내 네방향 이웃도 같은 사이즈로 그려지냐? */
		_bool		isNeighborDraw[NEIGHBOR_END] = { true, true, true, true };

		for (size_t i = 0; i < NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbors[i])
				isNeighborDraw[i] = m_pNeighbors[i]->isDraw(pGameInstance, pVerticesPos, WorldMatrix);
		}

		if (true == isNeighborDraw[NEIGHBOR_LEFT] &&
			true == isNeighborDraw[NEIGHBOR_TOP] &&
			true == isNeighborDraw[NEIGHBOR_RIGHT] &&
			true == isNeighborDraw[NEIGHBOR_BOTTOM])
		{
			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RT] ||
				true == isIn[CORNER_RB])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}

			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RB] ||
				true == isIn[CORNER_LB])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;


		if (true == isIn[CORNER_LT] ||
			true == isIn[CORNER_RB] ||
			true == isIn[CORNER_LB])
		{
			if (false == isNeighborDraw[NEIGHBOR_LEFT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			if (false == isNeighborDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}

		if (true == isIn[CORNER_LT] ||
			true == isIn[CORNER_RT] ||
			true == isIn[CORNER_RB])
		{
			if (false == isNeighborDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}

			if (false == isNeighborDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}

		return;
	}

	_float fRange = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	if (true == pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRange*10.0f))
	{
		for (size_t i = 0; i < CORNER_END; i++)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pGameInstance, pVerticesPos, pIndices, pNumIndices, WorldMatrix);
		}
	}
}

HRESULT CQuadTree::Make_Neighbors()
{
	if (nullptr == m_pChild[CORNER_LT]->m_pChild[CORNER_LT])
		return S_OK;

	m_pChild[CORNER_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChild[CORNER_RT];
	m_pChild[CORNER_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChild[CORNER_LB];

	m_pChild[CORNER_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChild[CORNER_LT];
	m_pChild[CORNER_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChild[CORNER_RB];

	m_pChild[CORNER_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChild[CORNER_RT];
	m_pChild[CORNER_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChild[CORNER_LB];

	m_pChild[CORNER_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChild[CORNER_LT];
	m_pChild[CORNER_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChild[CORNER_RB];

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChild[CORNER_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChild[CORNER_LT];
		m_pChild[CORNER_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChild[CORNER_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChild[CORNER_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChild[CORNER_LT];
		m_pChild[CORNER_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChild[CORNER_RT];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChild[CORNER_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChild[CORNER_RT];
		m_pChild[CORNER_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChild[CORNER_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChild[CORNER_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChild[CORNER_LB];
		m_pChild[CORNER_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChild[CORNER_RB];
	}

	for (size_t i = 0; i < CORNER_END; i++)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_Neighbors();
	}

	return S_OK;
}

_bool CQuadTree::isDraw(CGameInstance* pGameInstance, const _float3* pVerticesPos, _fmatrix WorldMatrix)
{
	_float4	vCamPosition = pGameInstance->Get_CamPosition();
	_float3 vPosition = _float3(vCamPosition.x, vCamPosition.y, vCamPosition.z);
	_matrix WorldInv = XMMatrixInverse(nullptr, WorldMatrix);

	XMStoreFloat3(&vPosition, XMVector3TransformCoord(XMLoadFloat3(&vPosition), WorldInv));

	vCamPosition = _float4(vPosition.x, vPosition.y, vPosition.z, 1.0f);

	_float fCamDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vCamPosition) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	if (fCamDistance * 0.1f > (m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]))
		return true;

	return false;
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX(TEXT("Failed to Created : CQuadTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pQuadTree : m_pChild)
		Safe_Release(pQuadTree);
}
