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

HRESULT CPgRowset::Execute(DBPARAMS * pParams, LONG* pcRowsAffected)
{
    return S_OK;
}

CPgVirtualArray::~CPgVirtualArray()
{
    if( m_res!=NULL )
        PQclear(m_res);
    m_session->Release();
    m_session=NULL;
}
BYTE& CPgVirtualArray::operator[] (int nIndex) const
{
    const int nfields=PQnfields(m_res);
    
    size_t offset=0;
    for( int i=0; i<nfields; ++i ) {
        int colsize=0;
        
        if( !PQgetisnull(m_res, nIndex, i) ) {
            const typeinfo *info=m_session->GetTypeInfo(PQftype(m_res, i));

            if( info!=NULL ) {
                colsize=info->GetWidth(m_res, nIndex, i);
                // Make sure alignment is ok
                offset+=info->alignment-1;
                offset-=offset%info->alignment;
            } else {
                ATLASSERT(info!=NULL);
                ATLTRACE2(atlTraceDBProvider, 0, "CPgVirtualArray::operator[] Query resulted in "
                    "undefined type %d\n", PQftype(m_res, i));
                colsize=0;
            }
        }

        m_offsets[i]=offset;
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

void CPgVirtualArray::AttachSess( CPgSession *session )
{
    m_session=session;
    m_session->AddRef();
}

HRESULT CPgRowset::PostConstruct( CPgSession *sess, PGresult *pRes )
{
    USES_CONVERSION;
    m_rgRowData.AttachSess( sess );
    m_rgRowData.SetResult( pRes );

    ExecStatusType stat=PQresultStatus(pRes);

    AtlTrace2(atlTraceDBProvider, 0, "CPgRowset::PostConstruct got status \"%s\" from command\n",
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
                
                OLECHAR *colname=U82W(PQfname(pRes, i));
                info.pwszName=new OLECHAR[lstrlenW(colname)+1];
                lstrcpyW(info.pwszName, colname );
                //info.pwszName=U82W(PQfname(pRes, i));
                info.iOrdinal=i+1; // XXX Only bookmark is 0. We don't support bookmarks.
                info.cbOffset=0;
                
                unsigned long pRestype=PQftype(pRes, i);
                const typeinfo *typinfo=sess->GetTypeInfo(pRestype);
                
                if( typinfo!=NULL ) {
                    typinfo->Status( typinfo, &info, pRes, i );
                } else {
                    // We are asked to work with unhandled data type
                    ATLASSERT(!"Unhandled type in query pResult");
                    
                    typeinfo::StatUnknown( &info, pRes, i );
                }
                
                // Add the column to the array
                m_colInfo.Add(info);
            }
        }
        break;
    case PGRES_EMPTY_QUERY:
        /* No command? */
        throw PgOleError(DB_E_NOCOMMAND, "Backend returned \"no command\"");
        break;
        //    case PGpRes_FATAL_ERROR:
        //        hr=DB_E_ERRORSINCOMMAND;
        //        break;
    default:
        AtlTrace2(atlTraceDBProvider, 0, "CPgCommand::Execute unhandled status\n%s\n",
            sess->PQerrorMessage() );
        /* XXX until we return error details */
        MessageBox(NULL, sess->PQerrorMessage(),
            "CPgCommand:Execute unhandled error", MB_ICONEXCLAMATION|MB_OK );
        ATLASSERT(FALSE);
        throw PgOleError(E_FAIL, sess->PQerrorMessage());
        break;
    }

    return S_OK;
}

CPgRowset::~CPgRowset()
{
    CComPtr<IPgCommand> cmd;

    // Find out whether we were derived from a CPgCommand, and if so, notify it that we are dead.
    if( SUCCEEDED(GetSite(IID_IPgCommand, reinterpret_cast<void **>(&cmd)))) {
        static_cast<CPgCommand *>(static_cast<IPgCommand *>(cmd))->ClearRowset(this);
    }
}