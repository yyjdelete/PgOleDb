// PGOleRS.h : Declaration of the CPGOleRowset
/*
 *   PostgreSQL OLE DB Provider 
 *   LGPL License  
 *   initial author Marek Mosiewicz (marekmosiewicz@poczta.onet.pl)
 *                  Jotel Poland 
 *
 *   RowSet object
 */
#ifndef __CPGOleRowset_H_
#define __CPGOleRowset_H_
#include "resource.h"       // main symbols

//temporary information about columns
class CPGOleWindowsFile: 
	public WIN32_FIND_DATA
{
public:
BEGIN_PROVIDER_COLUMN_MAP(CPGOleWindowsFile)
	PROVIDER_COLUMN_ENTRY("FileAttributes", 1, dwFileAttributes)
	PROVIDER_COLUMN_ENTRY("FileSizeHigh", 2, nFileSizeHigh)
	PROVIDER_COLUMN_ENTRY("FileSizeLow", 3, nFileSizeLow)
	PROVIDER_COLUMN_ENTRY("FileName", 4, cFileName)
	PROVIDER_COLUMN_ENTRY("AltFileName", 5, cAlternateFileName)
END_PROVIDER_COLUMN_MAP()
};
// CPGOleCommand
class ATL_NO_VTABLE CPGOleCommand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IAccessorImpl<CPGOleCommand>,
	public ICommandTextImpl<CPGOleCommand>,
	public ICommandPropertiesImpl<CPGOleCommand>,
	public IObjectWithSiteImpl<CPGOleCommand>,
	public IConvertTypeImpl<CPGOleCommand>,
	public IColumnsInfoImpl<CPGOleCommand>
{
public:
BEGIN_COM_MAP(CPGOleCommand)
	COM_INTERFACE_ENTRY(ICommand)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(ICommandProperties)
	COM_INTERFACE_ENTRY2(ICommandText, ICommand)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
END_COM_MAP()
// ICommand
public:
	HRESULT FinalConstruct()
	{
		HRESULT hr = CConvertHelper::FinalConstruct();
		if (FAILED (hr))
			return hr;
		hr = IAccessorImpl<CPGOleCommand>::FinalConstruct();
		if (FAILED(hr))
			return hr;
		return CUtlProps<CPGOleCommand>::FInit();
	}
	void FinalRelease()
	{
		IAccessorImpl<CPGOleCommand>::FinalRelease();
	}
	HRESULT WINAPI Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
						  LONG * pcRowsAffected, IUnknown ** ppRowset);
	static ATLCOLUMNINFO* GetColumnInfo(CPGOleCommand* pv, ULONG* pcInfo)
	{
		return CPGOleWindowsFile::GetColumnInfo(pv,pcInfo);
	}

BEGIN_PROPSET_MAP(CPGOleCommand)
	BEGIN_PROPERTY_SET(DBPROPSET_ROWSET)
		PROPERTY_INFO_ENTRY(IAccessor)
		PROPERTY_INFO_ENTRY(IColumnsInfo)
		PROPERTY_INFO_ENTRY(IConvertType)
		PROPERTY_INFO_ENTRY(IRowset)
		PROPERTY_INFO_ENTRY(IRowsetIdentity)
		PROPERTY_INFO_ENTRY(IRowsetInfo)
		PROPERTY_INFO_ENTRY(IRowsetLocate)
		PROPERTY_INFO_ENTRY(BOOKMARKS)
		PROPERTY_INFO_ENTRY(BOOKMARKSKIPPED)
		PROPERTY_INFO_ENTRY(BOOKMARKTYPE)
		PROPERTY_INFO_ENTRY(CANFETCHBACKWARDS)
		PROPERTY_INFO_ENTRY(CANHOLDROWS)
		PROPERTY_INFO_ENTRY(CANSCROLLBACKWARDS)
		PROPERTY_INFO_ENTRY(LITERALBOOKMARKS)
		PROPERTY_INFO_ENTRY(ORDEREDBOOKMARKS)
	END_PROPERTY_SET(DBPROPSET_ROWSET)
END_PROPSET_MAP()
};
class CPGOleRowset : public CRowsetImpl< CPGOleRowset, CPGOleWindowsFile, CPGOleCommand>
{
public:
	HRESULT Execute(DBPARAMS * pParams, LONG* pcRowsAffected);
//	ATLCOLUMNINFO* GetColumnInfo(CPGOleRowset* pv, ULONG* pcInfo		);
};
#endif //__CPGOleRowset_H_
