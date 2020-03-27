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
    static ATLCOLUMNINFO* GetColumnInfo( T *pT, ULONG* pcCols )
    {
        if( pcCols!=NULL )
            *pcCols=pT->m_colInfo.GetSize();

        return pT->m_colInfo.GetData();
    }
};

// CATLCOLUMNINFO
class CATLCOLUMNINFO : public ATLCOLUMNINFO {
    void copy( const ATLCOLUMNINFO &rhs ) {
        pwszName=new OLECHAR[lstrlenW(rhs.pwszName)+1];
        lstrcpyW(pwszName, rhs.pwszName);
    }
public:
    CATLCOLUMNINFO() {
        pwszName=NULL;
    }
    ~CATLCOLUMNINFO() {
        delete [] pwszName;
    }
    CATLCOLUMNINFO( const CATLCOLUMNINFO &rhs ) {
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }
    CATLCOLUMNINFO( const ATLCOLUMNINFO &rhs ) {
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }

    CATLCOLUMNINFO &operator=( const ATLCOLUMNINFO &rhs ) {
        delete [] pwszName;
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }
    CATLCOLUMNINFO &operator=( const CATLCOLUMNINFO &rhs ) {
        *this=static_cast<const ATLCOLUMNINFO &>(rhs);
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
	public IColumnsInfoImpl<CPgCommand>,
	public ICommandWithParameters
{
public:
BEGIN_COM_MAP(CPgCommand)
	COM_INTERFACE_ENTRY(ICommand)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(ICommandProperties)
	COM_INTERFACE_ENTRY2(ICommandText, ICommand)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
	COM_INTERFACE_ENTRY(ICommandWithParameters)
END_COM_MAP()
// ICommand
public:
    CPgCommand() : m_queryRes(NULL)
    {
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
						  LONG * pcRowsAffected, IUnknown ** ppRowset);
	static ATLCOLUMNINFO* GetColumnInfo(CPgCommand* pv, ULONG* pcInfo)
	{
        return CPgRemoteStorage::GetColumnInfo(pv,pcInfo);
	}
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
    // ICommandWithParameters
    STDMETHOD(GetParameterInfo) (
        ULONG         *pcParams,
        DBPARAMINFO    **prgParamInfo,
        OLECHAR        **ppNamesBuffer);
    STDMETHOD(MapParameterNames) (
        ULONG            cParamNames,
        const OLECHAR    *rgParamNames[],
        LONG __RPC_FAR   rgParamOrdinals[]);
    STDMETHOD(SetParameterInfo) (
        ULONG                   cParams,
        const ULONG __RPC_FAR   rgParamOrdinals[],
        const DBPARAMBINDINFO   rgParamBindInfo[]);
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
	PGresult *m_queryRes;  // Query results
    CSimpleArray<CATLCOLUMNINFO> m_colInfo;
    HRESULT CreateRowset(IUnknown* pUnkOuter, REFIID riid,
        DBPARAMS * pParams, LONG * pcRowsAffected,
        IUnknown** ppRowset,
        CPgRowset*& pRowsetObj);
    friend class CPgRemoteStorage;

    // CommandWithParameters related data
    struct param_info {
        param_info() : oid(0), wType(DBTYPE_EMPTY)
        {
        }
        unsigned long oid;
        int paramLength;
        ULONG ulParamSize;
        DBTYPE wType;
        BYTE bPrecision;
        BYTE bScale;
    };
    std::vector<param_info> m_params;
	HRESULT FillParams();
    HRESULT FillinValues( char *paramValues[], int paramLengths[], size_t num_params,
        DBPARAMS * pParams, class CPgSession *session, char **buffer );
    _bstr_t m_parsed_command; // The command text after parsing
};

#endif //__CPgRowset_H_
