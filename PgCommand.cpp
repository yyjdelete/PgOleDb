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

// Implementation of the CPgCommand

#include "stdafx.h"

#include "OleDb.h"
#include "PgCommand.h"
#include "PgSess.h"
#include "PgMultipleResults.h"
#include "ErrorLookupService.h"

using namespace std;
class stringci : public string
{
public:
    stringci()
    {
    }
    stringci( const stringci &rhs ) : string( rhs )
    {
    }
    stringci( const std::string &str ) : string( str )
    {
    }
    stringci( const char *str ) : string( str )
    {
    }
    bool operator< ( const stringci &rhs ) const {
        return stricmp( c_str(), rhs.c_str() )<0;
    }
    bool operator<= ( const stringci &rhs ) const {
        return stricmp( c_str(), rhs.c_str() )<=0;
    }
    bool operator== ( const stringci &rhs ) const {
        return stricmp( c_str(), rhs.c_str() )==0;
    }
    bool operator!= ( const stringci &rhs ) const {
        return stricmp( c_str(), rhs.c_str() )!=0;
    }
    bool operator> ( const stringci &rhs ) const {
        return stricmp( c_str(), rhs.c_str() )>0;
    }
    bool operator>= ( const stringci &rhs ) const {
        return stricmp( c_str(), rhs.c_str() )>=0;
    }
};

// This map is used to map the string names used by such functions as "SetParameterInfo"
// into types understood by OleDB
// Yes, it's a singleton
class name_dbtype_map : public std::map<stringci,DBTYPE>
{
public:
    name_dbtype_map()
    {
        //insert(value_type("DBTYPE_I1", DBTYPE_I1 ));
        insert(value_type("DBTYPE_I2", DBTYPE_I2 ));
        insert(value_type("DBTYPE_I4", DBTYPE_I4 ));
        insert(value_type("DBTYPE_I8", DBTYPE_I8 ));
        insert(value_type("DBTYPE_UI1", DBTYPE_UI1 ));
        //insert(value_type("DBTYPE_UI2", DBTYPE_UI2 ));
        //insert(value_type("DBTYPE_UI4", DBTYPE_UI4 ));
        //insert(value_type("DBTYPE_UI8", DBTYPE_UI8 ));
        insert(value_type("DBTYPE_R4", DBTYPE_R4 ));
        insert(value_type("DBTYPE_R8", DBTYPE_R8 ));
        insert(value_type("DBTYPE_CY", DBTYPE_CY ));
        insert(value_type("DBTYPE_DECIMAL", DBTYPE_DECIMAL ));
        insert(value_type("DBTYPE_NUMERIC", DBTYPE_NUMERIC ));
        insert(value_type("DBTYPE_BOOL", DBTYPE_BOOL ));
        //insert(value_type("DBTYPE_ERROR", DBTYPE_ERROR ));
        //insert(value_type("DBTYPE_UDT", DBTYPE_UDT ));
        insert(value_type("DBTYPE_VARIANT", DBTYPE_VARIANT ));
        //insert(value_type("DBTYPE_IDISPATCH", DBTYPE_IDISPATCH ));
        //insert(value_type("DBTYPE_IUNKNOWN", DBTYPE_IUNKNOWN ));
        //insert(value_type("DBTYPE_GUID", DBTYPE_GUID ));
        insert(value_type("DBTYPE_DATE", DBTYPE_DATE ));
        insert(value_type("DBTYPE_DBDATE", DBTYPE_DBDATE ));
        insert(value_type("DBTYPE_DBTIME", DBTYPE_DBTIME ));
        insert(value_type("DBTYPE_DBTIMESTAMP", DBTYPE_DBTIMESTAMP ));
        insert(value_type("DBTYPE_BSTR", DBTYPE_WSTR ));
        insert(value_type("DBTYPE_CHAR", DBTYPE_WSTR ));
        insert(value_type("DBTYPE_VARCHAR", DBTYPE_WSTR ));
        insert(value_type("DBTYPE_LONGVARCHAR", DBTYPE_WSTR ));
        insert(value_type("DBTYPE_WCHAR", DBTYPE_WSTR ));
        insert(value_type("DBTYPE_WVARCHAR", DBTYPE_WSTR ));
        insert(value_type("DBTYPE_WLONGVARCHAR", DBTYPE_WSTR ));
        //insert(value_type("DBTYPE_BINARY", DBTYPE_BYTES ));
        //insert(value_type("DBTYPE_VARBINARY", DBTYPE_BYTES ));
        //insert(value_type("DBTYPE_LONGVARBINARY", DBTYPE_BYTES ));
        insert(value_type("DBTYPE_FILETIME", DBTYPE_FILETIME ));
        insert(value_type("DBTYPE_VARNUMERIC", DBTYPE_VARNUMERIC ));
        //insert(value_type("DBTYPE_PROPVARIANT", DBTYPE_PROPVARIANT ));
    }
} const name_dbtype_map;

// Static functions
static HRESULT ParseDBBind( const DBBINDING *bindings, void *pData, DWORD &status,
                           DWORD &dst_length, auto_array<char> &dst_data,
                           IDataConvert *idataconv, DBTYPE dst_type ) throw (PgOleError)
{
    HRESULT hr;
    status=DBSTATUS_E_UNAVAILABLE;
    DWORD *plength=NULL;
    void *data;

    // Check what does the binding contain
    if( (bindings->eParamIO&DBPARAMIO_INPUT)==0 ) {
        // Parameter is not input - pass NULL
        status=DBSTATUS_S_ISNULL;
        dst_length=0;

        return S_OK;
    }

    unsigned char *buffer=reinterpret_cast<unsigned char *>(pData);

    if( (bindings->dwPart&DBPART_STATUS)!=0 ) {
        status=*reinterpret_cast<DWORD *>(buffer+bindings->obStatus);
    }

    if( (bindings->dwPart&DBPART_LENGTH)!=0 ) {
        plength=reinterpret_cast<DWORD *>(buffer+bindings->obLength);
    }

    if( (bindings->dwPart&DBPART_VALUE)==0 && status!=DBSTATUS_S_ISNULL &&
        status!=DBSTATUS_S_DEFAULT && status!=DBSTATUS_S_IGNORE ) {
        throw PgOleError(E_INVALIDARG, "No \"value\", and also not null or default");
    }

    data=buffer+bindings->obValue;

    hr=idataconv->GetConversionSize(bindings->wType, dst_type, plength, &dst_length, data);
    if( FAILED(hr) )
        throw PgOleError( hr, "Failure getting conversion destination data size" );

    dst_data=auto_array<char>(new char[dst_length]);
    if( dst_data.get()==NULL )
        throw PgOleError(E_OUTOFMEMORY, "No memory for converted data" );
    
    DWORD dst_status;
    hr=idataconv->DataConvert(bindings->wType, dst_type, *plength,
        &dst_length, data, dst_data.get(), dst_length, status, &dst_status,
        0, 0, DBDATACONVERT_DEFAULT );
    if( SUCCEEDED(hr) )
        status=dst_status;
    else
        throw PgOleError(hr, "Data conversion failed");
    
    return hr;
}

static size_t GetPGWidth( const typeinfo *info, const DBBINDING *bindings, void *pData,
                         DWORD *status, IDataConvert *idataconv )
{
    DWORD stat;
    DWORD dst_length;
    auto_array<char> dst_data;

    if( status==NULL )
        status=&stat;

    HRESULT hr=ParseDBBind( bindings, pData, *status, dst_length, dst_data, idataconv, info->wType );

    if( FAILED(hr) || *status!=DBSTATUS_S_OK )
        return 0;

    return info->PGGetLength( info, dst_data.get(), dst_length );
}

/* PGCopy wraps the actual parameter copy. It handles the NULL cases.
 * It must be passed the exact same type as it expects.
 */
static HRESULT PGCopy( const typeinfo *info, const DBBINDING *bindings, void *pData,
                      void *buffer, size_t buflen, IDataConvert *idataconv )
{
    HRESULT hr=S_OK;
    try {
        DWORD status;
        DWORD dst_length;
        auto_array<char> dst_data;

        hr=ParseDBBind( bindings, pData, status, dst_length, dst_data, idataconv, info->wType );

        if( SUCCEEDED(hr) && status==DBSTATUS_S_OK )
            hr=info->PGCopyData( info, dst_data.get(), dst_length, buffer, buflen );
    } catch( const PgOleError &err ) {
        hr=err.hr();
        ATLTRACE2(atlTraceDBProvider, 0, "PGCopy: %s\n", err.str());
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPgCommand
HRESULT CPgCommand::Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
								 LONG * pcRowsAffected, IUnknown ** ppRowset)
{
    USES_CONVERSION;
    ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute\n");
    CErrorLookupService::ClearError();
    HRESULT hr=S_OK;
    // The transaction object is not exception safe, so we need to manually release it at the end
    // of the function. This requires that certain vars will be available outside of the "try" block
    bool transactioninitiated=false;
    CPgSession *pgsess=NULL;
    CComPtr<IPgSession> isess;

    try {
        if( m_parsed_command.length()==0 ) {
            HRESULT hr=FillParams();
            
            if( FAILED(hr) )
                throw PgOleError(hr, "FillParams failed at parsing the command");
        }
        
        /* Get the session object, where our connection lies */
        
        hr=GetSite(IID_IPgSession, reinterpret_cast<void **>(&isess));
        if( FAILED(hr) ) {
            throw PgOleError(hr, "couldn't get session");
        }
        
        pgsess=static_cast<CPgSession *>(static_cast<IPgSession *>(isess));
        
        // XXX We're not using a cursor right now.
        //CComBSTR command=OLESTR("DECLARE oledbcursor BINARY CURSOR WITH HOLD FOR ");
        //command+=szCommand;
        
        size_t num_params=m_params.size();
        auto_array<unsigned int> paramTypes(new unsigned int [num_params]);
        auto_array<char *> paramValues(new char *[num_params]);
        auto_array<int> paramLengths(new int[num_params]);
        auto_array<int> paramFormats(new int[num_params]);
        
        for( int i=0; i<num_params; ++i ) {
            paramTypes[i]=m_params[i].oid;
            paramFormats[i]=1;
        }
        
        auto_array<char> databuff;
        
        hr=FillinValues( paramValues.get(), paramLengths.get(), num_params, pParams, pgsess,
            databuff );
        
        if( FAILED(hr) ) {
            throw PgOleError(hr, "FillinValues failed");
        }
        
        // We need to be inside a transaction if we are to get MultipleResults answers
        // Since only "select" will ever return MultipleResults, and as "Create table" will
        // fail if executed inside a transaction, make sure this is a select
        /* XXX Horrible kludge - just check minimum required to distinguish select from all other
         *     LEGAL commands. This means that we check that the length is at least 7, the first
         *     letter is "s" and the third is "l" (to distinguish from "set").
         *     This still errounously catches "select into", but so be it for the time being.
         */
        if( m_parsed_command.length()>7 &&
            towlower(static_cast<WCHAR *>(m_parsed_command)[0])==L's' &&
            towlower(static_cast<WCHAR *>(m_parsed_command)[2])==L'l' )
        {
            DWORD transactionlevel;
            pgsess->PgTransactionLevel(&transactionlevel);

            if( riid==IID_IMultipleResults && transactionlevel==0 ) {
                hr=pgsess->StartTransaction(ISOLATIONLEVEL_READCOMMITTED, 0, NULL,
                    &transactionlevel );
                if( FAILED(hr) )
                    throw PgOleError( hr, "Failed creating an explicit transaction for the command" );

                transactioninitiated=true;
            }
        }
        PGresult *res = pgsess->PQexec(OLE2CU8(m_parsed_command),
            num_params, paramTypes.get(), paramValues.get(), paramLengths.get(), paramFormats.get() );
        
        paramTypes.reset();
        paramValues.reset();
        paramLengths.reset();
        paramFormats.reset();
        
        if( pcRowsAffected!=NULL )
            *pcRowsAffected=atol(PQcmdTuples(res));
        
        hr=CreateResult(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, res );
        
        if( FAILED(hr) ) {
            throw PgOleError(hr, "Error creating rowset");
        }
                
    } catch( const PgOleError &err ) {
        hr=err.hr();
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute error: %s\n", err.str());
        if( err.str() ) {
            CErrorLookupService::ReportCustomError(err.str(), hr, IID_ICommand );
        }
    }

    if( transactioninitiated ) {
        // Call a version of "Commit" that will not clear our previous error information
        pgsess->ErroredCommit(FALSE, 0, 0 );
    }

    return hr;
}

HRESULT CPgCommand::CreateResult(IUnknown* pUnkOuter, REFIID riid,
                                 DBPARAMS * pParams, LONG * pcRowsAffected,
                                 IUnknown** ppRowset,
                                 PGresult *pRes)
{
    HRESULT hr;

    // XXX Need to check whether DBPROP_MULTIPLERESULTS was set on the command?
    if( riid==IID_IMultipleResults ) {
        hr=CreateMultiResult(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRes );
    } else {
        hr=CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRes );
    }

    return hr;
}

HRESULT CPgCommand::CreateMultiResult(IUnknown* pUnkOuter, REFIID riid,
                                 DBPARAMS * pParams, LONG * pcRowsAffected,
                                 IUnknown** ppRowset,
                                 PGresult *pRes)
{
    HRESULT hr;

    // Create an IMultipleResults object

    if ((pUnkOuter != NULL) && !InlineIsEqualUnknown(riid))
        throw PgOleError(DB_E_NOAGGREGATION, "No aggregate support" );

    CComPolyObject<CPgMultipleResults> *pMultiResults;
    hr=CComPolyObject<CPgMultipleResults>::CreateInstance(pUnkOuter, &pMultiResults);
    if( FAILED(hr) )
        throw PgOleError(hr, "Failed when creating IMultipleResults");

	CComPtr<IUnknown> spUnk;
	hr = pMultiResults->QueryInterface(&spUnk);
	if (FAILED(hr))
	{
		delete pMultiResults; // must hand delete as it is not ref'd
		throw PgOleError(hr, "Failed querying IMultipleResults object's IUnknown");
	}

    CComPtr<IMultipleResults> spMultiRes;
    hr=spUnk->QueryInterface(&spMultiRes);
    if( FAILED(hr) )
        throw PgOleError(hr, "Internal error");

    CPgMultipleResults *pgMultiRes=static_cast<CPgMultipleResults *>(
        static_cast<IMultipleResults *>(spMultiRes));
	pgMultiRes->SetSite(GetUnknown());

	if (InlineIsEqualGUID(riid, IID_NULL) || ppRowset == NULL)
	{
		if (ppRowset != NULL)
			*ppRowset = NULL;
		return hr;
	}

    hr=pMultiResults->QueryInterface(riid, reinterpret_cast<void **>(ppRowset));
    if( FAILED(hr) )
        throw PgOleError(hr, "Last QueryInterface on IMultipleResults failed");

    // Check whether this is a single row of type "refcursor"
    if( PQnfields(pRes)!=1 || PQftype( pRes, 0 )!=1790 ) {
        // A single result - create a rowset.
        CComPtr<IPgRowset> pRowset;
        hr=CreateRowset(pUnkOuter, IID_IPgRowset, pParams, pcRowsAffected,
            reinterpret_cast<IUnknown **>(&pRowset), pRes );
        if( FAILED(hr) )
            throw( hr, "Couldn't create result rowset" );

        CPgRowset *pgRow=static_cast<CPgRowset *>(static_cast<IPgRowset *>(pRowset));

        pgMultiRes->AddRowset(pgRow);
    } else {
        // Multiple results
        CComPtr<IPgSession> isess=NULL;
        hr=GetDBSession(IID_IPgSession, reinterpret_cast<IUnknown **>(&isess));
        
        CPgSession *pgsess=static_cast<CPgSession *>(static_cast<IPgSession *>(isess));

        for( int i=0; i<PQntuples(pRes); ++i ) {
            size_t cursorlength=PQgetlength( pRes, i, 0 );

            char *cursorname=PQgetvalue( pRes, i, 0 );

            // Prepare a new query to fetch the results
            _bstr_t fetchquery="fetch all from \"";
            fetchquery+=cursorname;
            fetchquery+="\"";

            PGresult *res=pgsess->PQexec( fetchquery );

            // And now create a Rowset from this newly executed query
            CComPtr<IPgRowset> pRowset;
            hr=CreateRowset(pUnkOuter, IID_IPgRowset, pParams, pcRowsAffected,
                reinterpret_cast<IUnknown **>(&pRowset), res );

            fetchquery="close \"";
            fetchquery+=cursorname;
            fetchquery+="\"";

            PGresult *res2=pgsess->PQexec( fetchquery );
            PQclear(res2);

            if( FAILED(hr) )
            {
                PQclear(res);
                throw( hr, "Couldn't create some of the result rowsets" );
            }
            
            CPgRowset *pgRow=static_cast<CPgRowset *>(static_cast<IPgRowset *>(pRowset));
            
            pgMultiRes->AddRowset(pgRow);
        }
    }

    return hr;
}

HRESULT CPgCommand::CreateRowset(IUnknown* pUnkOuter, REFIID riid,
                                 DBPARAMS * pParams, LONG * pcRowsAffected,
                                 IUnknown** ppRowset,
                                 PGresult *pRes)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::CreateRowset\n");
    HRESULT hr;

    // XXX Standard violating behaviour. If backend command returned a table of a single
    // column of refcursors, we would normally treat it as different results for
    // IMultipleResults. However, if IMultipleResults was not requested on the rowset,
    // specs say we should return the first row. Instead, we are reutrning the refcursors
    // themselves.
    CPgRowset *pRowsetObj;

    // This variable is used in case IID_NULL was the interface asked.
    CComPtr<IUnknown> pRowset;
    IID iid;

    if( riid!=IID_NULL ) {
        iid=riid;
    } else {
        // Create a rowset, and have the "pRowset" destructor return it when this
        // function is over.
        iid=IID_IUnknown;
        ppRowset=&pRowset;
    }

    hr=ICommandTextImpl<CPgCommand>::CreateRowset( pUnkOuter, iid, pParams,
        pcRowsAffected, ppRowset, pRowsetObj );

    if( SUCCEEDED(hr) ) {
        CComPtr<IUnknown> rowset;
        rowset.Attach(*ppRowset); // Don't increment refcount, unless all tests succeed

        CComPtr<IPgRowset> object;

        hr=(*ppRowset)->QueryInterface(&object);
        if(SUCCEEDED(hr)) {
            // Ok, we're sure it's a valid rowset - make sure our CComPtr doesn't release it
            (*ppRowset)->AddRef();

            CComPtr<IPgSession> isess=NULL;
            hr=GetDBSession(IID_IPgSession, reinterpret_cast<IUnknown **>(&isess));

            CPgRowset *prs=static_cast<CPgRowset *>(static_cast<IPgRowset *>(object));
            hr=prs->PostConstruct( static_cast<CPgSession *>(static_cast<IPgSession *>(isess)),
                pRes );
        } else {
            throw PgOleError( hr, "Couldn't get desired interface for created rowset");
        }
    }

    return hr;
}

void CPgCommand::FinalRelease()
{
    ATLASSERT(m_spUnkSite != 0);

    IAccessorImpl<CPgCommand>::FinalRelease();
}
HRESULT CPgCommand::GetParameterInfo (
                                      ULONG         *pcParams,
                                      DBPARAMINFO    **prgParamInfo,
                                      OLECHAR        **ppNamesBuffer)
{
    if( pcParams==NULL || prgParamInfo==NULL ) {
        return E_INVALIDARG;
    }

    if( m_parsed_command.length()==0 ) {
        HRESULT hr=FillParams();

        if( FAILED(hr) )
            return hr;
    }

    if( ppNamesBuffer!=NULL )
        *ppNamesBuffer=NULL;

    *pcParams=m_params.size();
    *prgParamInfo=reinterpret_cast<DBPARAMINFO *>
        (CoTaskMemAlloc(sizeof(**prgParamInfo)*(*pcParams)));

    if( *prgParamInfo==NULL ) {
        *pcParams=0;
        *prgParamInfo=NULL;
        return E_OUTOFMEMORY;
    }

    for( int i=0; i<*pcParams; ++i ) {
        (*prgParamInfo)[i].dwFlags=DBPARAMFLAGS_ISINPUT|DBPARAMFLAGS_ISNULLABLE;
        (*prgParamInfo)[i].iOrdinal=i+1;
        (*prgParamInfo)[i].pwszName=NULL;
        (*prgParamInfo)[i].pTypeInfo=NULL;
        (*prgParamInfo)[i].ulParamSize=m_params[i].ulParamSize;
        (*prgParamInfo)[i].wType=m_params[i].wType;
        (*prgParamInfo)[i].bPrecision=m_params[i].bPrecision;
        (*prgParamInfo)[i].bScale=m_params[i].bScale;
    }

    return S_OK;
}
HRESULT CPgCommand::SetParameterInfo (
        ULONG cParams,
        const ULONG __RPC_FAR   rgParamOrdinals[],
        const DBPARAMBINDINFO   rgParamBindInfo[])
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::SetParameterInfo\n");
    CErrorLookupService::ClearError();

    if( cParams==0 ) {
        m_params.clear();

        return S_OK;
    }

    // XXX - We need to never fail inside this loop. Specs say that if we fail, no change is
    // done at all.
    for( int i=0; i<cParams; ++i ) {
        // Make sure we support the types
        if( (rgParamBindInfo[i].dwFlags&~(DBPARAMFLAGS_ISINPUT|DBPARAMFLAGS_ISNULLABLE))!=0 ) {
            ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::SetParameterInfo error: Not supported parameter flag %08x\n",
                rgParamBindInfo[i].dwFlags);
            return E_INVALIDARG;
        }

        if( rgParamOrdinals[i]==0 )
            return E_INVALIDARG;
        
        if( m_params.size()<rgParamOrdinals[i] )
            m_params.resize(rgParamOrdinals[i]);

        int ordinal=rgParamOrdinals[i]-1;
        CComPtr<IPgSession> isess;

        HRESULT hr=GetSite(IID_IPgSession, reinterpret_cast<void **>(&isess));
        if( FAILED(hr) ) {
            throw PgOleError(hr, "couldn't get session");
        }
        const CPgSession *pgsess=static_cast<CPgSession *>(static_cast<IPgSession *>(isess));

        if( rgParamBindInfo[i].pwszDataSourceType!=NULL ) {
            USES_CONVERSION;
            name_dbtype_map::const_iterator type=
                name_dbtype_map.find(stringci(OLE2CU8(rgParamBindInfo[i].pwszDataSourceType)));
            if( type!=name_dbtype_map.end() ) {
                m_params[ordinal].wType=type->second;
            } else {
                ATLTRACE2(atlTraceDBProvider, 3, "CPgCommand::SetParameterInfo DBTYPE name lookup failed\n");
                return DB_E_BADTYPENAME;
            }

            m_params[ordinal].oid=pgsess->GetOIDType(m_params[ordinal].wType);
            if( m_params[ordinal].oid==0 ) {
                ATLTRACE2(atlTraceDBProvider, 3, "CPgCommand::SetParameterInfo DBTYPE oid lookup failed\n");
                return DB_E_BADTYPENAME;
            }

            m_params[ordinal].ulParamSize=rgParamBindInfo[i].ulParamSize;
            m_params[ordinal].bPrecision=rgParamBindInfo[i].bPrecision;
            m_params[ordinal].bScale=rgParamBindInfo[i].bScale;
        }
    }

    // XXX Need to return DB_S_TYPEINFOOVERRIDDEN where appropriate
    return S_OK;
}
HRESULT CPgCommand::MapParameterNames (
        ULONG            cParamNames,
        const OLECHAR    *rgParamNames[],
        LONG __RPC_FAR   rgParamOrdinals[])
{
    // But we do not support named parameters!
    CErrorLookupService::ClearError();
    CErrorLookupService::ReportCustomError("Named parameters are not supported", E_NOTIMPL,
        IID_ICommandWithParameters );
    return E_NOTIMPL;
}


HRESULT CPgCommand::FillParams()
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::FillParams\n");
    CErrorLookupService::ClearError();

    /* Parse the command line, looking for question marks and such.
     * If the command format is "{ call procname(foo) }", translate to
     * "select * from procname(foo)".
     * If the command format is "{ call procname }", look up the proper parameters, and
     * fill them in as question marks.
     */
    USES_CONVERSION;
    LPOLESTR szCommand=NULL;
    LPOLESTR szWorkCommand=NULL;
    HRESULT hr=S_OK;
    GUID guidDialect;
    _bstr_t query;

    if( FAILED(hr=GetCommandText( &guidDialect, &szCommand )) )
        return hr;

    szWorkCommand=szCommand;

    if( szCommand==NULL || *szCommand==OLESTR('\0') ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::FillParams No command\n");
        hr=DB_E_NOCOMMAND;
        goto error;
    }

    if( szCommand[0]==L'{' ) { // Special command syntax
        LPOLESTR i=szCommand+1;

        // XXX - must be a better way of doing this. We'll use the dumb way for now
        // Skip whitespaces
        while( *i!=0 && iswspace(*i) )
            ++i;

        // Is it a "call"?
        static const WCHAR CALL_CMD[]={'c','a','l','l'};
        bool match=true;
        for( int j=0; match && j<(sizeof(CALL_CMD)/sizeof(*CALL_CMD)); ++j, ++i )
            match=CALL_CMD[j]==*i;

        if( !match || !iswspace(*i) ) { // Parse error - unknown command other than "call"
            ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::FillParams Unsupported call type %s\n",
                OLE2CA(szCommand));
            CErrorLookupService::ReportCustomError("Unsupported call type", E_FAIL,
                IID_ICommand );

            hr=E_FAIL;

            goto error;
        }

        // Skip whitespaces
        while( *i!=0 && iswspace(*i) )
            ++i;

        _bstr_t function_name;
        bool func_quoted;
        // Get the function name to call
        if( *i==L'"' ) { // identifier is quoted, looking for a " char to end identifier
            func_quoted=true;
            WCHAR tmp[2];
            tmp[1]=L'\0';

            ++i;

            while( *i!=0 && *i!=L'"' ) {
                if( *i==L'\\' && *(i+1)!=0 )
                    ++i;

                tmp[0]=*(i++);
                function_name+=tmp;
            }

            if( *i==L'"' )
                ++i;
        } else { // identifier is unquoted - scan until whitespace
            func_quoted=false;
            WCHAR tmp[2];
            tmp[1]=L'\0';

            while( *i!=0 && !iswspace(*i) ) {
                tmp[0]=*(i++);
                function_name+=tmp;
            }

        }
        if( *i==0 ) {
            ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::FillParams Error parsing %s\n",
                OLE2CA(szCommand));
            CErrorLookupService::ReportCustomError("Parse error trying in { call } type command", DB_E_ERRORSINCOMMAND,
                IID_ICommand );
            hr=DB_E_ERRORSINCOMMAND;
            
            goto error;
        }

        // Skip whitespaces
        while( *i!=0 && iswspace(*i) )
            ++i;

        // Start constructing the new query
        query=L"select * from ";
        if( func_quoted )
            query+=L"\"";
        query+=function_name;
        if( func_quoted )
            query+=L"\"";

        if( *i==L'}' ) {
            // No parameters - need to find out what they are
            _bstr_t argsquery="select pronargs, proargtypes from pg_proc where proname=";
            if( !func_quoted )
                argsquery+="lower(";
            argsquery+="'";
            argsquery+=function_name;
            argsquery+="'";
            if( !func_quoted )
                argsquery+=")";

            IPgSession *isess=NULL;
            hr=GetSite(IID_IPgSession, reinterpret_cast<void **>(&isess));
            CPgSession *psess=static_cast<CPgSession *>(isess);

            if( FAILED(hr) ) {
                ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::FillParams couldn't get session\n");

                goto error;
            }

            PGresult *res = psess->PQexec(OLE2CU8(
                static_cast<LPCOLESTR>(argsquery)));
            if( PQresultStatus(res)!=PGRES_TUPLES_OK ) {
                hr=E_FAIL;
                isess->Release();
                ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand:FillParams error running query \"%s\": \"%s\"\n",
                    static_cast<LPCSTR>(argsquery), PQresultErrorMessage(res) );
                CErrorLookupService::ReportCustomError(PQresultErrorMessage(res), E_FAIL,
                    IID_ICommand );
                CErrorLookupService::ReportCustomError("Error finding out procedure's parameters",
                    E_FAIL, IID_ICommand );

                PQclear(res);

                goto error;
            }

            if( PQntuples( res )!=1 ) {
                hr=E_FAIL;
                isess->Release();
                ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::FillParams expected one row, got %d\n",
                    PQntuples(res) );
                PQclear(res);
                CErrorLookupService::ReportCustomError("Procedure name for automatic arguments is not unique", E_FAIL,
                    IID_ICommand );

                goto error;
            }

            // Fill in the types vector
            short num_params=ntohs(*reinterpret_cast<signed short *>(PQgetvalue(res, 0, 0)));

            // Record the parameters for the new command
            m_params.resize(num_params);
            query+="(";

            unsigned long *types=reinterpret_cast<unsigned long *>(PQgetvalue(res, 0, 1));
            for( int i=0; i<num_params; ++i ) {
                unsigned long typeoid=ntohl(types[i]);
                const typeinfo *type=psess->GetTypeInfo(typeoid);

                ATLASSERT(type!=NULL);

                // XXX need to figure out what to do if we don't know the OID

                m_params[i].oid=typeoid;
                m_params[i].wType=type->wType;
                m_params[i].bPrecision=type->bPrecision;

                if( i!=0 )
                    query+=",";
                query+=" ?";
            }
            query+=")";

            szWorkCommand=query;
        } else { // Copy parameters
            // XXX - Not implemented yet!
            ATLASSERT(!"Parametered calling of stored procedures not yet implemented");
            hr=E_NOTIMPL;
            CErrorLookupService::ReportCustomError("Parameterized calls of stored procedures is not implemented",
                E_NOTIMPL, IID_ICommand );

            goto error;
        }
    }

    // Check szWorkCommand out for "?", and convert them to the proper syntax.
    m_parsed_command="";

    {
        int paramnum=1;
        enum { ST_REG, ST_SINGLEQUOTE, ST_SQ_ESC, ST_DOUBLEQUOTE, ST_DQ_ESC } state=ST_REG;
        
        for( const WCHAR *i=szWorkCommand; *i!=L'\0'; ++i ) {
            WCHAR buffer[30];
            
            buffer[0]=*i;
            buffer[1]=0;
            
            switch(state) {
            case ST_REG:
                switch(*i) {
                case L'\'':
                    state=ST_SINGLEQUOTE;
                    break;
                case L'"':
                    state=ST_DOUBLEQUOTE;
                    break;
                case L'?':
                    _snwprintf(buffer, sizeof(buffer)/sizeof(*buffer), L"$%d", paramnum++);
                    break;
                }
                break;
            case ST_SINGLEQUOTE:
                switch(*i) {
                case L'\'':
                    state=ST_REG;
                    break;
                case L'\\':
                    state=ST_SQ_ESC;
                    break;
                }
                break;
            case ST_SQ_ESC:
                state=ST_SINGLEQUOTE;
                break;
            case ST_DOUBLEQUOTE:
                switch(*i) {
                case L'"':
                    state=ST_REG;
                    break;
                case L'\\':
                    state=ST_DQ_ESC;
                    break;
                }
                break;
            case ST_DQ_ESC:
                state=ST_DOUBLEQUOTE;
                break;
            }
            
            m_parsed_command+=buffer;
        }
    }
error:
    CoTaskMemFree(szCommand);

    return hr;
}

HRESULT CPgCommand::FillinValues( char *paramValues[], int paramLengths[], size_t num_params,
                                 DBPARAMS * pParams, CPgSession *sess, auto_array<char> &buffer )
{
    if( num_params==0 && pParams==NULL )
        return S_OK;

    if( pParams==NULL )
        return DB_E_PARAMNOTOPTIONAL;

    if( pParams->cParamSets!=1 )
        return E_INVALIDARG;

    HRESULT hr=S_OK;
    DBBINDING *rgBindings=NULL;

    try {
        DBACCESSORFLAGS dwAccessorFlags;
        ULONG cBindings;
        
        hr=GetBindings( pParams->hAccessor, &dwAccessorFlags, &cBindings, &rgBindings );
        
        if( FAILED(hr) )
            throw(hr);
        
        if( cBindings!=num_params )
            throw(DB_E_PARAMNOTOPTIONAL);
        
        size_t buffsize=0;
        std::vector<size_t> offsets;
        std::vector<DWORD> statuses;
        offsets.resize(cBindings);
        statuses.resize(cBindings);

        for( int i=0; i<cBindings; ++i ) {
            ATLASSERT((i+1)==rgBindings[i].iOrdinal);

            const typeinfo *info=sess->GetTypeInfo(m_params[i].oid);

            if( info==NULL ) {
                char error_msg[250];
                _snprintf(error_msg, sizeof(error_msg),
                    "Cannot convert type from PostgreSQL oid type %d to DBTYPE.", m_params[i].oid );
                ATLTRACE2(atlTraceDBProvider, 0, error_msg);

                // Major oops - we are asked to handle a type we don't know how
                throw( PgOleError(E_FAIL, error_msg) );
            }

            offsets[i]=buffsize;
            paramLengths[i]=GetPGWidth( info, &rgBindings[i], pParams->pData, &(statuses[i]),
                m_spConvert );
            buffsize+=paramLengths[i];

            if( statuses[i]==DBSTATUS_S_IGNORE ) {
                // Status is not valid for parameters
                // XXX Need to set the status to DBSTATUS_E_BADSTATUS
                throw PgOleError( DB_S_ERRORSOCCURRED, "DBSTATUS_S_IGNORE used for command parameter" );
            }
        }

        buffer=auto_array<char>(new char [buffsize]);
        for( i=0; i<cBindings; ++i ) {
            if( statuses[i]==DBSTATUS_S_OK ) {
                const typeinfo *info=sess->GetTypeInfo(m_params[i].oid);
                
                paramValues[i]=buffer.get()+offsets[i];
                
                hr=PGCopy( info, &rgBindings[i], pParams->pData, buffer.get()+offsets[i],
                    paramLengths[i], m_spConvert );
                
                if( FAILED(hr) )
                    throw(hr);
            } else {
                // If the status is non-fatal (or an exception would be raised at the getwidth
                // stage), but also not ok. Pass a NULL as parameter.
                paramValues[i]=NULL;
            }
        }
    } catch(HRESULT res) {
        buffer.reset();
        hr=res;
    }

    CoTaskMemFree(rgBindings);

    return hr;
}

ATLCOLUMNINFO* CPgCommand::GetColumnInfo(CPgCommand* pv, ULONG* pcInfo)
{
    return CPgRowset::GetColumnInfo(pv->m_rowset,pcInfo);
}

HRESULT CPgCommand::GetColumnInfo ( ULONG        *pcColumns,
                                   DBCOLUMNINFO **prgInfo,
                                   OLECHAR      **ppStringsBuffer)
{
    if( m_rowset==NULL )
        return DB_E_NOCOMMAND;
    // XXX Strictly speaking, we are returning DB_E_NOCOMMAND even when we should have returned
    // actual results. However, as everybody understands that we cannot return these results
    // without further execution, they don't call us when we don't have them.

    return static_cast<IColumnsInfo *>(m_rowset)->GetColumnInfo( pcColumns, prgInfo, ppStringsBuffer );
}

HRESULT CPgCommand::MapColumnIDs ( ULONG        cColumnIDs,
                                  const DBID   rgColumnIDs[],
                                  ULONG        rgColumns[])
{
    if( m_rowset==NULL )
        return DB_E_NOCOMMAND;
    // See XXX comment for GetColumnInfo above.

    return m_rowset->MapColumnIDs (cColumnIDs, rgColumnIDs, rgColumns);
}
