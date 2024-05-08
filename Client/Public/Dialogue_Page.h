#pragma once

#include "Client_Defines.h"
#include "Ortho.h"


BEGIN(Client)

class CDialogue_Page : public COrtho
{
public:
	typedef struct tagBreakDesc
	{
	/*������ ���ý� �ʿ��Ѱ͵�.
	1. �� �������� �帧 ���� ����������
	  � ���� �˰� ������ �׸�ŭ ���� int��
	2. �������� ���� ���� �극��ũ ����Ʈ �߻� ��ġ
	 ���� ����  �� �����Ҵ��ؼ� ������
	3. �������� ���� ���� ����� ���� ����Ʈ
	���� ������
	4. �������� ���� ���� �극ũ ����Ʈ �ε���;
	
	4. ��� �ڽ� ����Ұ���
		���ڷ� ����, ���ڸ�ŭ for���Ƿ� ���� ���ܿ���
	
	5. �� �ڽ��� ���빰 ----�� �־�θ� ��
		vector�� ������, ����
	
		�� ���⼭ ���� ����
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
	wstring					m_strNPC_Name = { TEXT("�׽�Ʈ") };
	wstring					m_strNPC_Occupation = {TEXT("���谡���")};
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