#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CElement_OHD final : public COrtho
{
public:
	enum ELEMENTS_OVERHEAD { OVERHEAD_LIGHTNING, OVERHEAD_FIRE, OVERHEAD_GRASS, OVERHEAD_ICE, OVERHEAD_EARTH, OVERHEAD_WATER, OVERHEAD_WIND, OVERHEAD_END};

private:
	CElement_OHD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElement_OHD(const CElement_OHD& rhs);
	virtual ~CElement_OHD() = default;

private:
	_bool			m_bElementStatus[OVERHEAD_END] = {};
	_bool			m_bElementReactStatus[OVERHEAD_END] = {};

	_int			m_iNumElements = { };
	_int			m_iElement = { };

	_float			m_fTimeAcc = {};



	vector<ELEMENTS_OVERHEAD>		m_vecElements;

private:
	HRESULT Add_Component(void* pArg);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_Matrix(_vector vWorldPos, _float fSizeX, _float fSizeY);
	void Update_Matrix(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void Set_Element(ELEMENTS_OVERHEAD eElements,_bool bSwitch);
	void React_Elements(ELEMENTS_OVERHEAD eElement1, ELEMENTS_OVERHEAD eElement2);
	void Zero_Elements();
	
public:
	static CElement_OHD*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END