#pragma once

#include "Component.h"

/* HLSL, FX 확장자 파일을 DX를 통해 읽어서 빌드 */
/* 작성한 셰이더 파일을 빌드하여 객체화 */

BEGIN(Engine)

class ENGINE_DLL CFiniteStateMachine final : public CComponent
{
private:
	CFiniteStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFiniteStateMachine(const CFiniteStateMachine& rhs);
	virtual ~CFiniteStateMachine() = default;

private:
	_int			m_iNumStates = { };
	_int			m_iIdleStateIndex = { };

	_int			m_iCurrentState = { };
	_int			m_iPrevState = { };
	list<_int>		m_GeneralStateList;
	list<_int>		m_RegulateStateList;

public:
	_int			Get_CurrentState() { return m_iCurrentState; }
	_int			Get_PrevState() { return m_iPrevState; }

	void			Set_IdleState() { m_iCurrentState = m_iIdleStateIndex; }
	void			Bind_IdleState(_int iState) { m_iIdleStateIndex = iState; }

	void			Append_GeneralState(vector<_int> iStates);
	void			Append_RegulateState(vector<_int> iStates);

	_int GetGeneralStateListSize() const { return static_cast<_int>(m_GeneralStateList.size()); }
	_int GetRegulateStateListSize() const {	return static_cast<_int>(m_RegulateStateList.size()); }

	//_bool			Get_RenderState() { return m_bRenderState; }
	//void			Set_RenderState(_bool bRenderState) { m_bRenderState = bRenderState; }

public:
	_int			Change_State(_int iState);
	_int			Change_ForceState(_int iState);
	_int			Change_RegulateState(_int iState);

public:
	virtual HRESULT Initialize_Prototype(_int iNumStates);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CFiniteStateMachine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNumStates = 1);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

END