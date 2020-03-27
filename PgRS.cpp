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
#include "PgRS.h"
#include "PgSess.h"

/////////////////////////////////////////////////////////////////////////////
// CPgCommand
HRESULT CPgCommand::Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
								 LONG * pcRowsAffected, IUnknown ** ppRowset)
{
    LPOLESTR szCommand=NULL;
    ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute\n");
    HRESULT hr;
    USES_CONVERSION;
    
    if( FAILED(hr=GetCommandText( NULL, &szCommand )) )
        return hr;

    if( szCommand==NULL || *szCommand==OLESTR('\0') ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute No command\n");

        return DB_E_NOCOMMAND;
    }

    /* Get the session object, where our connection lies */
    IPgSession *isess=NULL;
    hr=GetSite(IID_IPgSession, reinterpret_cast<void **>(&isess));
    if( FAILED(hr) ) {
        CoTaskMemFree(szCommand);
        ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::Execute couldn't get session\n");
        return hr;
    }

    // XXX Wer'e not using a cursor right now.
    //CComBSTR command=OLESTR("DECLARE oledbcursor BINARY CURSOR WITH HOLD FOR ");
    //command+=szCommand;

    PGresult *res = static_cast<CPgSession *>(isess)->PQexec(OLE2CA(szCommand));
    ExecStatusType stat=PQresultStatus(res);

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
                info.pTypeInfo=NULL;
                info.iOrdinal=i+1; // XXX Only bookmark is 0
                info.ulColumnSize=PQfsize(res, i);
                if( info.ulColumnSize<0 )
                    info.ulColumnSize=~0;
                info.bScale=0;
                info.dwFlags=DBCOLUMNFLAGS_ISFIXEDLENGTH|DBCOLUMNFLAGS_ISNULLABLE|
                    DBCOLUMNFLAGS_MAYBENULL;
                info.cbOffset=0;

                info.columnid.eKind=DBKIND_PROPID;
                info.columnid.uName.ulPropid=PQftablecol(res, i);

                unsigned long restype=PQftype(res, i);
                const typeinfo *typinfo=static_cast<CPgSession *>(isess)->
                    GetTypeInfo(restype);

                info.wType=typinfo->wType;
                info.bPrecision=typinfo->bPrecision;

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
            static_cast<CPgSession *>(isess)->PQerrorMessage() );
        /* XXX until we return error details */
        MessageBox(NULL, static_cast<CPgSession *>(isess)->PQerrorMessage(),
            "CPgCommand:Execute unhandled error", MB_ICONEXCLAMATION|MB_OK );
        ATLASSERT(FALSE);
        hr=E_FAIL;
        break;
    }
    
    /* Clear out the no longer needed session class */
    isess->Release();
    isess=NULL;

    CoTaskMemFree(szCommand);

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

HRESULT CPgRowset::Execute(DBPARAMS * pParams, LONG* pcRowsAffected)
{
    return S_OK;
}

CPgVirtualArray::~CPgVirtualArray()
{
    PQclear(m_res);
    m_session->Release();
    m_session=NULL;
}
BYTE& CPgVirtualArray::operator[] (int nIndex) const
{
    const int nfields=PQnfields(m_res);
    
    size_t offset=0;
    for( int i=0; i<nfields; ++i ) {
        m_offsets[i]=offset;
        int colsize=0;
        
        if( !PQgetisnull(m_res, nIndex, i) ) {
            const typeinfo *info=m_session->GetTypeInfo(PQftype(m_res, i));

            if( info!=NULL ) {
                colsize=info->GetWidth(m_res, nIndex, i);
            } else {
                ATLASSERT(info!=NULL);
                ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::operator[] Query resulted in "
                    "undefined type %d\n", PQftype(m_res, i));
                colsize=0;
            }
        }

        // XXX Need to make sure alignment is ok
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
            int colsize=info->GetWidth(m_res, nIndex, i);

            info->CopyData( m_buff+m_offsets[i], colsize, m_res, nIndex, i );
        }
    }
    
    return *m_buff;
}

DBSTATUS CPgVirtualArray::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pColInfo)
{
    if( PQgetisnull(m_res, pRow->m_iRowset, pColInfo->iOrdinal-1) )
        return DBSTATUS_S_ISNULL;
    else
        return DBSTATUS_S_OK;
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
