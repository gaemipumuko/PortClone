#pragma once

#include "Client_Defines.h"
#include "Ortho.h"


BEGIN(Client)

class CDialogue_Page : public COrtho
{
public:
	typedef struct tagBreakDesc
	{
	/*선택지 세팅시 필요한것들.
	1. 각 선택지가 흐름 어디로 가져갈건지
	  몇개 쓸지 알고 있으니 그만큼 세팅 int로
	2. 선택지에 따른 다음 브레이크 포인트 발생 위치
	 위랑 같은  것 동적할당해서 가져가
	3. 선택지에 따른 가장 가까운 종료 포인트
	위랑 같을것
	4. 선택지에 따른 다음 브레크 포인트 인덱스;
	
	4. 몇개의 박스 사용할건지
		숫자로 보관, 숫자만큼 for문의로 벡터 땡겨오기
	
	5. 각 박스의 내용물 ----다 넣어두면 됨
		vector에 아이콘, 문자
	
		자 여기서 이제 문제
	*/
	
		_int  iNumBoxes;
		_int* pNextChoiceIdx;
		_int* pNextBreakIdx;
		_int* pNexTerminatorIdx;
		_int* pScriptFlow;
		_int* pNextBreak;


	}BREAK_DESC;

private:
	CDialogue_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogue_Page(const CDialogue_Page& rhs);
	virtual ~CDialogue_Page() = default;

private:
	wstring					m_strNPC_Name = { TEXT("테스트") };
	wstring					m_strNPC_Occupation = {TEXT("모험가길드")};
	wstring					m_strNPC_Script = {};

	_float4x4				m_DialogueBoxMat;
	vector<_float4x4>		m_DecorLMat;
	vector<_float4x4>		m_DecorRMat;


	//Decor

	_float					m_fScriptStartX;
	_float					m_fScriptEndX;
	_float					m_fOccupationStartX;
	_float					m_fOccupationXEnd;
	_float					m_fOccupationLength;
	_float					m_fScriptLength;
	_float					m_fLTailX;
	_float					m_fLTail_Len;
	_float					m_fLMidX;
	_float					m_fLMid_Len;
	_float					m_fLHeadX;
	_float					m_fLHead_Len;
	_float					m_fRHeadX;
	_float					m_fRHead_Len;
	_float					m_fRMidX;
	_float					m_fRMid_Len;
	_float					m_fRTailX;
	_float					m_fRTail_Len;

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Script(wstring strScript) { m_strNPC_Script = strScript; }
	void Set_Target(wstring strNPCName, wstring strNPCDetail) {
		m_strNPC_Name = strNPCName;
		m_strNPC_Occupation = strNPCDetail;}

public:
	static CDialogue_Page*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;


};

END