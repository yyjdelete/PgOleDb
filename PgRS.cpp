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

