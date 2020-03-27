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

// Static functions
static HRESULT ParseDBBind( const DBBINDING *bindings, const void *pData, DWORD &status,
                           DWORD &length, const void *&data )
{
    status=DBSTATUS_E_UNAVAILABLE;
    length=0;

    // Check what does the binding contain
    if( (bindings->eParamIO&DBPARAMIO_INPUT)==0 ) {
        // Parameter is not input - pass NULL
        status=DBSTATUS_S_ISNULL;
        length=0;
        data=NULL;

        return S_OK;
    }

    const unsigned char *buffer=reinterpret_cast<const unsigned char *>(pData);

    if( (bindings->dwPart&DBPART_STATUS)!=0 ) {
        status=*reinterpret_cast<const DWORD *>(buffer+bindings->obStatus);
    }

    if( (bindings->dwPart&DBPART_LENGTH)!=0 ) {
        length=*reinterpret_cast<const DWORD *>(buffer+bindings->obLength);
    }

    if( (bindings->dwPart&DBPART_VALUE)==0 && status!=DBSTATUS_S_ISNULL &&
        status!=DBSTATUS_S_DEFAULT && status!=DBSTATUS_S_IGNORE ) {
        // No "value", and also not null or default.
        return E_FAIL; // XXX Find a better return code here
    }

    data=buffer+bindings->obValue;

    return S_OK;
}

static size_t GetPGWidth( const typeinfo *info, const DBBINDING *bindings, const void *pData,
                         DWORD *status )
{
    DWORD stat;
    DWORD length;
    const void *data;

    if( status==NULL )
        status=&stat;

    HRESULT hr=ParseDBBind( bindings, pData, *status, length, data );

    if( FAILED(hr) || *status!=DBSTATUS_S_OK )
        return 0;

    return info->PGGetLength( info, data, length );
}

/* PGCopy wraps the actual parameter copy. It handles the NULL cases.
 * It must be passed the exact same type as it expects.
 */
static HRESULT PGCopy( const typeinfo *info, const DBBINDING *bindings, const void *pData,
                      void *buffer, size_t buflen, IDataConvert *idataconv )
{
    DWORD status;
    DWORD length;
    void *data;

    try {
        HRESULT hr=ParseDBBind( bindings, pData, status, length, data );
        if( FAILED(hr) )
            throw(hr);
        
        DWORD dst_length;
        DWORD dst_status;
        
        hr=idataconv->GetConversionSize(bindings->wType, info->wType, &length,
            &dst_length, data );
        if( FAILED(hr) )
            throw(hr);
        
        std::auto_ptr<char> dst_data(new char[dst_length]);
        if( dst_data.get()==NULL )
            throw(E_OUTOFMEMORY);
        
        hr=idataconv->DataConvert(bindings->wType, info->wType, length,
            &dst_length, data, dst_data.get(), dst_length, status, &dst_status,
            0, 0, DBDATACONVERT_DEFAULT );
        if( FAILED(hr) )
            throw(hr);
        
        if( FAILED(hr) || status==DBSTATUS_S_ISNULL || status==DBSTATUS_S_DEFAULT ||
            status==DBSTATUS_S_IGNORE ) {
            return hr;
        }
        
        return info->PGCopyData( info, dst_data.get(), dst_length, buffer, buflen );
    } catch( HRESULT hres ) {
        return hres;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CPgCommand
HRESULT CPgCommand::Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
								 LONG * pcRowsAffected, IUnknown ** ppRowset)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute\n");
    HRESULT hr;
    USES_CONVERSION;

    if( m_parsed_command.length()==0 ) {
        HRESULT hr=FillParams();

        if( FAILED(hr) )
            return hr;
    }
    
    /* Get the session object, where our connection lies */
    IPgSession *isess=NULL;
    hr=GetSite(IID_IPgSession, reinterpret_cast<void **>(&isess));
    if( FAILED(hr) ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute couldn't get session\n");
        return hr;
    }
    CPgSession *pgsess=static_cast<CPgSession *>(isess);

    // XXX Wer'e not using a cursor right now.
    //CComBSTR command=OLESTR("DECLARE oledbcursor BINARY CURSOR WITH HOLD FOR ");
    //command+=szCommand;

    size_t num_params=m_params.size();
    unsigned int *paramTypes=new unsigned int [num_params];
    char **paramValues=new char *[num_params];
    int *paramLengths=new int[num_params];
    int *paramFormats=new int[num_params];

    for( int i=0; i<num_params; ++i ) {
        paramTypes[i]=m_params[i].oid;
        paramFormats[i]=1;
    }

    char *databuff=NULL;

    hr=FillinValues( paramValues, paramLengths, num_params, pParams, pgsess, &databuff );

    if( FAILED(hr) ) {
        delete [] paramTypes;
        delete [] paramValues;
        delete [] paramLengths;
        delete [] paramFormats;

        isess->Release();

        return hr;
    }

    PGresult *res = pgsess->PQexec(OLE2CU8(m_parsed_command),
        num_params, paramTypes, paramValues, paramLengths, paramFormats );
    ExecStatusType stat=PQresultStatus(res);

    delete [] paramTypes;
    delete [] paramValues;
    delete [] paramLengths;
    delete [] paramFormats;

    AtlTrace2(atlTraceDBProvider, 0, "CPgCommand::Execute got status \"%s\" from command\n",
        PQresStatus(stat) );

    CPgRowset* pRowset;
    hr=CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRowset);

    if( FAILED(hr) ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute Error creating rowset\n");
        return hr;
    }

    pRowset->m_rgRowData.SetResult(res);
    
    switch( stat ) {
    case PGRES_COMMAND_OK:
        /* Ok, but no results */
        if( pcRowsAffected!=NULL )
            *pcRowsAffected=atol(PQcmdTuples(res));
        break;
    case PGRES_TUPLES_OK:
        {
            /* Ok, results to return */
            for( int i=0; i<PQnfields(res); ++i ) {
                ATLCOLUMNINFO info;
                
                info.pwszName=A2OLE(PQfname(res, i));
                info.iOrdinal=i+1; // XXX Only bookmark is 0
                info.cbOffset=0;
                        
                unsigned long restype=PQftype(res, i);
                const typeinfo *typinfo=pgsess->GetTypeInfo(restype);

                if( typinfo!=NULL ) {
                    typinfo->Status( typinfo, &info, res, i );
                } else {
                    // We are asked to work with unhandled data type
                    ATLASSERT(!"Unhandled type in query result");

                    typeinfo::StatUnknown( &info, res, i );
                }

                // Add the column to the array
                m_colInfo.Add(CATLCOLUMNINFO(info));
            }

            pRowset->SetColInfo(m_colInfo);    
        }
        break;
    case PGRES_EMPTY_QUERY:
        /* No command? */
        hr=DB_E_NOCOMMAND;
        break;
//    case PGRES_FATAL_ERROR:
//        hr=DB_E_ERRORSINCOMMAND;
//        break;
    default:
        AtlTrace2(atlTraceDBProvider, 0, "CPgCommand::Execute unhandled status\n%s\n",
            pgsess->PQerrorMessage() );
        /* XXX until we return error details */
        MessageBox(NULL, static_cast<CPgSession *>(isess)->PQerrorMessage(),
            "CPgCommand:Execute unhandled error", MB_ICONEXCLAMATION|MB_OK );
        ATLASSERT(FALSE);
        hr=E_FAIL;
        break;
    }
    
    /* Clear out the no longer needed session class */
    pgsess=NULL;
    isess->Release();
    isess=NULL;

    return hr;
}

HRESULT CPgCommand::CreateRowset(IUnknown* pUnkOuter, REFIID riid,
                                 DBPARAMS * pParams, LONG * pcRowsAffected,
                                 IUnknown** ppRowset,
                                 CPgRowset*& pRowsetObj)
{
    HRESULT hr=ICommandTextImpl<CPgCommand>::CreateRowset( pUnkOuter, riid, pParams,
        pcRowsAffected, ppRowset, pRowsetObj );

    if( SUCCEEDED(hr) ) {
        IPgRowset *object;

        hr=(*ppRowset)->QueryInterface(&object);
        if(SUCCEEDED(hr)) {
            IPgSession *isess=NULL;
            hr=GetSite(IID_IPgSession, reinterpret_cast<void **>(&isess));

            static_cast<CPgRowset *>(object)->m_rgRowData.AttachSess(
                static_cast<CPgSession *>(isess));
            object->Release();
            // isess is not released here. It will be released by m_rgRowData
        } else {
            (*ppRowset)->Release();
        }
    }

    return hr;
}

void CPgCommand::FinalRelease()
{
    PQclear(m_queryRes); // I believe this needs to go here
    ATLASSERT(m_spUnkSite != 0);
    IPgSession *isess;
    HRESULT hr=m_spUnkSite->QueryInterface(&isess);
    
    if( SUCCEEDED(hr) ) {
        CPgSession *pSess = static_cast<CPgSession *>(isess);
        //PGresult *res = pSess->PQexec("CLOSE oledbcursor");
        //PQclear(res);
        
        isess->Release();
    }
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
    if( cParams==0 ) {
        m_params.clear();

        return S_OK;
    }

    return E_FAIL;
}
HRESULT CPgCommand::MapParameterNames (
        ULONG            cParamNames,
        const OLECHAR    *rgParamNames[],
        LONG __RPC_FAR   rgParamOrdinals[])
{
    // But we do not support named parameters!
    return E_FAIL;
}


HRESULT CPgCommand::FillParams()
{
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

            while( *i!=0 && *i!=L'"' ) {
                if( *i==L'\\' && *(i+1)!=0 )
                    ++i;

                tmp[0]=*(i++);
                function_name+=tmp;
            }
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
            hr=E_FAIL;
            
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
                AtlTrace2(atlTraceDBProvider, 0, "CPgCommand:FillParams error running query \"%s\": \"%s\"\n",
                    static_cast<LPCSTR>(argsquery), PQresultErrorMessage(res) );
                PQclear(res);

                goto error;
            }

            if( PQntuples( res )!=1 ) {
                hr=E_FAIL;
                isess->Release();
                AtlTrace2(atlTraceDBProvider, 0, "CPgCommand::FillParams expected one row, got %d\n",
                    PQntuples(res) );
                PQclear(res);

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
            hr=E_FAIL;

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
                                 DBPARAMS * pParams, CPgSession *sess, char **buffer )
{
    if( num_params==0 && pParams==NULL )
        return S_OK;

    if( pParams==NULL )
        return DB_E_PARAMNOTOPTIONAL;

    if( pParams->cParamSets!=1 )
        return E_INVALIDARG;

    HRESULT hr=S_OK;
    DBBINDING *rgBindings=NULL;

    *buffer=NULL;
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
        offsets.resize(cBindings);

        for( int i=0; i<cBindings; ++i ) {
            ATLASSERT((i+1)==rgBindings[i].iOrdinal);

            const typeinfo *info=sess->GetTypeInfo(m_params[i].oid);

            if( info==NULL ) {
                // Major oops - we are asked to handle a type we don't know how
                throw( E_FAIL );
            }

            offsets[i]=buffsize;
            DWORD status;
            paramLengths[i]=GetPGWidth( info, &rgBindings[i], pParams->pData, &status );
            buffsize+=paramLengths[i];
            // XXX Need to handle the status
        }

        *buffer=new char [buffsize];
        for( i=0; i<cBindings; ++i ) {
            const typeinfo *info=sess->GetTypeInfo(m_params[i].oid);

            paramValues[i]=*buffer+offsets[i];

            hr=PGCopy( info, &rgBindings[i], pParams->pData, *buffer+offsets[i],
                paramLengths[i], m_spConvert );

            if( FAILED(hr) )
                throw(hr);
        }
    } catch(HRESULT res) {
        delete [] *buffer;
        *buffer=NULL;
        hr=res;
    }

    CoTaskMemFree(rgBindings);

    return hr;
}

