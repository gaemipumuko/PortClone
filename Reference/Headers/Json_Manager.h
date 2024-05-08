#pragma once

#include "Base.h"

using namespace Json;

BEGIN(Engine)

class CJson_Manager final : public CBase
{
private:
	CJson_Manager();
	virtual ~CJson_Manager() = default;

private:
	map<wstring, Value> m_Jsons;

public:
	HRESULT	Load_JsonFile(const wstring& strJsonTag, const string& strJsonFilePath);

	_int		Find_JsonIntValue(const wstring& strJsonTag, vector<string> vFindTags);
	_float		Find_JsonFloatValue(const wstring& strJsonTag, vector<string> vFindTags);
	wstring		Find_JsonStringValue(const wstring& strJsonTag, vector<string> vFindTags);
	
	list<_int>		Find_JsonIntArrayValues(const wstring& strJsonTag, vector<string> vFindTags);
	list<wstring>	Find_JsonStringArrayValues(const wstring& strJsonTag, vector<string> vFindTags);

public:
	HRESULT Initialize();

public:
	static CJson_Manager*	Create();
	virtual void			Free();
};

END