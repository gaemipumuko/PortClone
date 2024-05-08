#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Item.h"
#include "SelRect.h"

BEGIN(Client)
class CItem_Weapon;
class CItem_Artifact;
class CItem_Material;
class CItem_Consumable;
END

BEGIN(Client)

class CInventory final : public COrtho
{
public:
	enum RIV_TYPE { RIV_WEAPON = 1, RIV_ARTIFACT, RIV_MATERIAL, RIV_COOKING, RIV_END };	/* RenderInven Type */

private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

private:
	CTexture*			m_pButtonTex = {nullptr};

	CSelRect*			m_pSelect_Box = { nullptr };

private:
	_bool				m_bDisplayButton = { false };
	_bool				m_bOver = { false };

	_int				m_iMOverIndex = {-1};
	_int				m_iMOverSlot = {};
	_int				m_iNumRows = {};
	_int				m_iNumGems = {0};
	_int				m_iNumMora = {0};

	vector<_float4x4>	m_IconMatrix = { };
	vector<_float4x4>	m_SlotMatrix = {};
	vector<_float4x4>	m_DetailBoxMatrix = { };
	_float4x4			m_ButtonMat = {};
	_float4x4			m_ButtonIconMat = {};


private:
	_uint			m_iSelectedIndex = { };
	_uint			m_iSelectedGrade = {};
	_float			m_fScrollView = { };
	_float2			m_vStripUVPos;
	_float2			m_vStripUVScale;
	_float2			m_vItemBgUVPos;
	_float2			m_vItemBgUVScale;

	RIV_TYPE		m_eRenderInvenType = { RIV_END };

	/* Weapon Storage */
	_uint			m_iNumWeaponStorage = { };
	list<CItem_Weapon*>	m_WeaponStorage = { };

	/* Artifact Storage */
	_uint				m_iNumArtifactStorage = { };
	list<CItem_Artifact*>	m_ArtifactStorage = { };

	/* Material Storage */
	_uint				m_iNumMaterialStorage = { };
	list<CItem_Material*>	m_MaterialStorage = { };

	/* Cooking Storage */
	_uint				m_iNumCookingStorage = { };
	list<CItem_Consumable*>	m_CookingStorage = { };

	_uint				m_iNumCurrentStorage = {0};

private:
	/*For. SelRectBox*/
	_bool				m_bShowSelectBox = { false };

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
	void	Open_Inventory();
	_int	Search_Inventory(wstring strItemName);
	_int	Search_Inventory(_int iItemID);
public:
	void	Add_Item(CItem* pItem);
	void	Use_Item(wstring strItemName, _int iQty);
	void	Use_Item(_int iItemID, _int iQty);
	_int	Get_NumGems() { return m_iNumGems; }
	_int	Get_CurrentMora();

public:
	static CInventory*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END