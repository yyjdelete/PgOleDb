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

#include "stdafx.h"
#include "OleDb.h"
#include "PgRS.h"
#include "PgSess.h"

#include "ErrorLookupService.h"

HRESULT CPgRowset::Execute(DBPARAMS * pParams, DBROWCOUNT* pcRowsAffected)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgRowset::Execute\n");
    return S_OK;
}

CPgVirtualArray::~CPgVirtualArray()
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::~CPgVirtualArray\n");
    if( m_res!=NULL )
        PQclear(m_res);
    if( m_session!=NULL ) {
        m_session->Release();
        m_session=NULL;
    }
}
BYTE& CPgVirtualArray::operator[] (int nIndex) const
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::operator[]\n");
    const int nfields=PQnfields(m_res);
    
    size_t offset=0;
    int i;
    for( i=0; i<nfields; ++i ) {
        int colsize=0;
        int datasize=0;
        
        if( !PQgetisnull(m_res, nIndex, i) ) {
            const typeinfo *info=m_session->GetTypeInfo(PQftype(m_res, i));

            if( info!=NULL ) {
                datasize=info->GetWidth(m_session, m_res, nIndex, i);
                if( (info->wType&DBTYPE_BYREF)==0 ) {
                    colsize=datasize;
                    // Make sure alignment is ok
                    offset+=info->alignment-1;
                    offset-=offset%info->alignment;
                } else {
                    colsize=sizeof(void *);
                    // Make sure alignment is ok
                    offset+=sizeof(void*)-1;
                    offset-=offset%sizeof(void*);
                }
            } else {
                ATLASSERT(info!=NULL);
                ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::operator[] Query resulted in "
                    "undefined type %d\n", PQftype(m_res, i));
                colsize=0;
            }
        }

        m_offsets[i]=offset;
        m_sizes[i]=datasize;
        offset+=colsize;
    }
    
    if( m_buff!=NULL )
        delete [] m_buff;
    m_buff=new BYTE[offset];
    
    for( i=0; i<nfields; ++i ) {
        if( !PQgetisnull(m_res, nIndex, i) ) {
            ATLASSERT(m_offsets[i]<offset);
            size_t colwidth=(i==nfields-1?offset:m_offsets[i+1])-m_offsets[i];
            const typeinfo *info=m_session->GetTypeInfo(PQftype(m_res, i));

            if( (info->wType&DBTYPE_BYREF)==0 ) {
                int colsize=info->GetWidth(m_session, m_res, nIndex, i);
                
                info->CopyData( m_buff+m_offsets[i], colsize, m_session, m_res, nIndex, i );
            } else {
                info->CopyData( m_buff+m_offsets[i], sizeof(void *), m_session, m_res, nIndex,
                    i );
            }
        }
    }
    
    return *m_buff;
}

DBSTATUS CPgVirtualArray::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pColInfo)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::GetDBStatus\n");
    if( PQgetisnull(m_res, pRow->m_iRowset, pColInfo->iOrdinal-1) )
        return DBSTATUS_S_ISNULL;
    else
        return DBSTATUS_S_OK;
}

void CPgVirtualArray::AttachSess( CPgSession *session )
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::AttachSess\n");
    m_session=session;
    m_session->AddRef();
}

HRESULT CPgRowset::PostConstruct( CPgSession *sess, PGresult *pRes )
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::PostConstruct\n");
    USES_CONVERSION;
    m_rgRowData.AttachSess( sess );
    m_rgRowData.SetResult( pRes );

    ExecStatusType stat=PQresultStatus(pRes);

    ATLTRACE2(atlTraceDBProvider, 0, "CPgRowset::PostConstruct got status \"%s\" from command\n",
        PQresStatus(stat) );
        
    switch( stat ) {
    case PGRES_COMMAND_OK:
        /* Ok, but no results */
        break;
    case PGRES_TUPLES_OK:
        {
            /* Ok, results to return */
            for( int i=0; i<PQnfields(pRes); ++i ) {
                ATLCOLUMNINFO info;
                
                info.pwszName=U82W(PQfname(pRes, i));
                info.iOrdinal=i+1; // XXX Only bookmark is 0. We don't support bookmarks.
                info.cbOffset=0;
                
                unsigned long pRestype=PQftype(pRes, i);
                const typeinfo *typinfo=sess->GetTypeInfo(pRestype);
                
                if( typinfo!=NULL ) {
                    typinfo->Status( typinfo, &info, pRes, i );
                } else {
                    // We are asked to work with unhandled data type
                    _bstr_t message;
                    CHAR number[13];
                    message="Query returned unhandled type ";
                    _ltot( pRestype, number, 10 );
                    message+=number;
                    ATLTRACE2(atlTraceDBProvider, 0, (const char *)message );
                    throw PgOleError(E_FAIL, message);
                }
                
                // Add the column to the array
                m_colInfo.Add(CATLCOLUMNINFO(info));
            }
        }
        break;
    case PGRES_EMPTY_QUERY:
        /* No command? */
        CErrorLookupService::ReportCustomError(sess->PQerrorMessage(), DB_E_NOCOMMAND, IID_IRowset );
        throw PgOleError(DB_E_NOCOMMAND, "Backend returned \"no command\"");
        break;
        //    case PGpRes_FATAL_ERROR:
        //        hr=DB_E_ERRORSINCOMMAND;
        //        break;
    case PGRES_FATAL_ERROR:
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute returned fatal error\n" );
        CErrorLookupService::ReportCustomError(sess->PQerrorMessage(), E_FAIL, IID_IRowset );
        throw PgOleError(E_FAIL, "Fatal error in query");
        break;
    default:
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute unhandled status\n%s\n",
            sess->PQerrorMessage() );
        CErrorLookupService::ReportCustomError(sess->PQerrorMessage(), E_FAIL, IID_IRowset );
        throw PgOleError(E_FAIL, "Unhandled status in query result");
        break;
    }

    return S_OK;
}

CPgRowset::~CPgRowset()
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgRowset::~CPgRowset\n");
    CComPtr<IPgCommand> cmd;

    // Find out whether we were derived from a CPgCommand, and if so, notify it that we are dead.
    if( SUCCEEDED(GetSite(IID_IPgCommand, reinterpret_cast<void **>(&cmd)))) {
        static_cast<CPgCommand *>(static_cast<IPgCommand *>(cmd))->ClearRowset(this);
    }
}

HRESULT STDMETHODCALLTYPE CPgRowset::GetColumnInfo(DBORDINAL *pcColumns,
                                        DBCOLUMNINFO **prgInfo,
                                        OLECHAR **ppStringsBuffer)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CPgRowset::GetColumnInfo\n");

    HRESULT hr=IColumnsInfoImpl< CPgRowset >::GetColumnInfo( pcColumns, prgInfo, ppStringsBuffer );

    // There is a bug in some versions of ATL which causes GetColumnInfo to expose internal
    // pointers. A compile time check would have been ideal here, but a runtime check will
    // have to do.
    if( SUCCEEDED(hr) && *pcColumns>0 && (*prgInfo)[0].pwszName!=*ppStringsBuffer )
    {
        ATLTRACE2(atlTraceDBProvider, 0, "Working around bug in IColumnsInfoImpl::GetColumnInfo\n");

        for( ULONG iCol=0, iOffset=0; iCol<*pcColumns; ++iCol )
        {
            if( (*prgInfo)[iCol].pwszName!=NULL ) {
                (*prgInfo)[iCol].pwszName=*ppStringsBuffer+iOffset;
                iOffset+=wcslen(*ppStringsBuffer+iOffset)+1;
            }
        }
    }

    return hr;
}