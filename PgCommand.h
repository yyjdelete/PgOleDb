/*
 * This file is part of OleDb, an OLE DB provider for PostgreSQL
 * Copyright (C) 2004 Shachar Shemesh for Lingnu Open Systems Consulting (http://www.lignu.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// PgCommand.h : Declaration of the CPgCommand
#ifndef __CPgCommand_H_
#define __CPgCommand_H_

#include "resource.h"       // main symbols

// CPgRemoteStorage
class CPgRemoteStorage  
{
public:
    template <class T>
    static ATLCOLUMNINFO* GetColumnInfo( T *pT, DBORDINAL* pcCols )
    {
        if( pcCols!=NULL )
            *pcCols=pT->m_colInfo.GetSize();

        return pT->m_colInfo.GetData();
    }
};

class CPgRowset;

// CPgCommand
class ATL_NO_VTABLE CPgCommand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IAccessorImpl<CPgCommand>,
	public ICommandTextImpl<CPgCommand>,
	public ICommandPropertiesImpl<CPgCommand>,
	public IObjectWithSiteImpl<CPgCommand>,
	public IConvertTypeImpl<CPgCommand>,
	public ICommandWithParameters,
    public IPgCommand,
    public IPgCommandText,
	public IColumnsInfo
{
//COM_INTERFACE_ENTRY2: https://social.msdn.microsoft.com/Forums/en-US/b18a0650-0cbe-4cd9-ba64-27a1b6a53e14/custom-atl-ole-db-provider-not-running-under-sql-server-2012-x64?forum=sqldataaccess
public:
BEGIN_COM_MAP(CPgCommand)
	COM_INTERFACE_ENTRY2(ICommand, ICommandText)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(ICommandProperties)
	COM_INTERFACE_ENTRY(ICommandText)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
	COM_INTERFACE_ENTRY(ICommandWithParameters)
    COM_INTERFACE_ENTRY(IPgCommand)
END_COM_MAP()
// ICommand
public:
    CPgCommand() : m_rowset(NULL)
    {
    }
    ~CPgCommand()
    {
        // Make sure that the rowset has already been freed when we are freed
        ATLASSERT(m_rowset==NULL);
    }

	HRESULT FinalConstruct()
	{
		HRESULT hr = CConvertHelper::FinalConstruct();
		if (FAILED (hr))
			return hr;
		hr = IAccessorImpl<CPgCommand>::FinalConstruct();
		if (FAILED(hr))
			return hr;

		return CUtlProps<CPgCommand>::FInit();
	}
	void FinalRelease();
	HRESULT WINAPI Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
						  DBROWCOUNT * pcRowsAffected, IUnknown ** ppRowset);
	static ATLCOLUMNINFO* GetColumnInfo(CPgCommand* pv, DBORDINAL* pcInfo);
    STDMETHOD(SetCommandText)(REFGUID rguidDialect,LPCOLESTR pwszCommand)
	{
        USES_CONVERSION;
		ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::SetCommandText \"%.300s\"\n", OLE2CA(pwszCommand));
        Lock();
        HRESULT hr=ICommandTextImpl<CPgCommand>::SetCommandText(rguidDialect, pwszCommand);
        SetParameterInfo(0, NULL, NULL );
        m_parsed_command="";
        Unlock();

        return hr;
    }
    STDMETHOD(PgSetCommandText)( REFGUID rguidDialect, LPCOLESTR command)
    {
        HRESULT hr=SetCommandText(rguidDialect, command);
        if(SUCCEEDED(hr))
            m_parsed_command=command;

        return hr;
    }

    // ICommandWithParameters
    STDMETHOD(GetParameterInfo) (
        DB_UPARAMS         *pcParams,
        DBPARAMINFO    **prgParamInfo,
        OLECHAR        **ppNamesBuffer);
    STDMETHOD(MapParameterNames) (
        DB_UPARAMS            cParamNames,
        LPCWSTR    rgParamNames[],
        DB_LPARAMS   rgParamOrdinals[]);
    STDMETHOD(SetParameterInfo) (
        DB_UPARAMS                   cParams,
        const DB_UPARAMS   rgParamOrdinals[],
        const DBPARAMBINDINFO   rgParamBindInfo[]);

    // IColumnsInfo
    STDMETHOD(GetColumnInfo) (
        DBORDINAL    *pcColumns,
        DBCOLUMNINFO **prgInfo,
        OLECHAR      **ppStringsBuffer);
    STDMETHOD(MapColumnIDs) (
        DBORDINAL        cColumnIDs,
        const DBID   rgColumnIDs[],
        DBORDINAL        rgColumns[]);
BEGIN_PROPSET_MAP(CPgCommand)
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
private:
    HRESULT CreateResult(IUnknown* pUnkOuter, REFIID riid,
        DBPARAMS * pParams, DBROWCOUNT * pcRowsAffected,
        IUnknown** ppRowset,
        PGresult *pRes);
    HRESULT CreateMultiResult(IUnknown* pUnkOuter, REFIID riid,
        DBPARAMS * pParams, DBROWCOUNT * pcRowsAffected,
        IUnknown** ppRowset,
        PGresult *pRes);
    HRESULT CreateRowset(IUnknown* pUnkOuter, REFIID riid,
        DBPARAMS * pParams, DBROWCOUNT * pcRowsAffected,
        IUnknown** ppRowset,
        PGresult *pRes);

    // CommandWithParameters related data
    struct param_info {
        param_info() : oid(0), wType(DBTYPE_EMPTY)
        {
        }
        unsigned long oid;
        int paramLength;
        DBLENGTH ulParamSize;
        DBTYPE wType;
        BYTE bPrecision;
        BYTE bScale;
    };
    std::vector<param_info> m_params;
	HRESULT FillParams();
    HRESULT FillinValues( char *paramValues[], int paramLengths[], size_t num_params,
        DBPARAMS * pParams, class CPgSession *session, auto_array<char> &buffer );
    _bstr_t m_parsed_command; // The command text after parsing

    CPgRowset *m_rowset;
public:
    // A function for CPgRowset to notify us that it's dieing
    void ClearRowset( const CPgRowset *rs ) {
        if( m_rowset==rs )
            m_rowset=NULL;
    }
};

#endif //__CPgRowset_H_
