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

// Implementation of the CPgSession
#include "stdafx.h"
#include "OleDb.h"
#include "PgSess.h"

// When changing, please make sure that the definition here is the same as when declared
const char *CPgSession::s_typenames[]={"utinyint"};
const typeinfo CPgSession::s_cust_types_type[]={
    typeinfo( DBTYPE_UI1, 3 )
};
const unsigned long CPgSession::s_types_oids[]={
    16 /* bool */, 21 /* int2 */, 28 /* xid - transaction ID */, 26 /* oid */,
    23 /* int4 */, 20 /* int8 */, 25 /* text */,
    19 /* name */, 1043 /* varchar */, 1009 /* text[] */, 1114, /* timestamp */
    1700 /* numeric */ };
const typeinfo CPgSession::s_types_type[]={
    typeinfo( DBTYPE_BOOL, 1, typeinfo::StdC_memcpy, typeinfo::StdGW_1, typeinfo::StdPGC_memcpy, typeinfo::StdPGWidth1, 1 ), // bool
    typeinfo( DBTYPE_I2, 5, typeinfo::StdC_ntoh_2, typeinfo::StdGW_2, typeinfo::StdPGC_h2n_2, typeinfo::StdPGWidth2, 2 ), // int2
    typeinfo( DBTYPE_UI4, 10, typeinfo::StdC_ntoh_4, typeinfo::StdGW_4, typeinfo::StdPGC_h2n_4, typeinfo::StdPGWidth4, 4 ), // xid
    typeinfo( DBTYPE_UI4, 10, typeinfo::StdC_ntoh_4, typeinfo::StdGW_4, typeinfo::StdPGC_h2n_4, typeinfo::StdPGWidth4, 4 ), // oid
    typeinfo( DBTYPE_I4, 10, typeinfo::StdC_ntoh_4, typeinfo::StdGW_4, typeinfo::StdPGC_h2n_4, typeinfo::StdPGWidth4, 4 ), // int4
    typeinfo( DBTYPE_I8, 20, typeinfo::StdC_ntoh_8, typeinfo::StdGW_8, typeinfo::StdPGC_h2n_8, typeinfo::StdPGWidth8, 8 ), // int8
    typeinfo( DBTYPE_WSTR, ~0, COPY_string, GetWidth_string, typeinfo::StdPGC_memcpy, typeinfo::StdPGWidthInvalid, 2 ), // text - var length string, no limit
    typeinfo( DBTYPE_WSTR, ~0, COPY_string, GetWidth_string, typeinfo::StdPGC_memcpy, typeinfo::StdPGWidthInvalid, 2 ), // name - 63-char type for storing system identifiers
    typeinfo( DBTYPE_WSTR, ~0, COPY_string, GetWidth_string, typeinfo::StdPGC_memcpy, typeinfo::StdPGWidthInvalid, 2 ), // varchar
    typeinfo( DBTYPE_ARRAY|DBTYPE_STR, ~0 ), // text[]. XXX - Is this the right way to handle this? Should consider DBTYPE_VECTOR
    typeinfo( DBTYPE_DBTIMESTAMP, ~0, COPY_timestamp, GetWidth_timestamp, typeinfo::StdPGC_memcpy, typeinfo::StdPGWidthInvalid, 4, GetStatus_timestamp ), // timestamp
    typeinfo( DBTYPE_NUMERIC, 39, COPY_numeric, GetWidth_numeric, typeinfo::StdPGC_memcpy, typeinfo::StdPGWidthInvalid, 4, GetStatus_numeric ), // numeric
};

HRESULT STDMETHODCALLTYPE CPgSession::PgConnectDB( BSTR connectString )
{
    USES_CONVERSION;
    HRESULT hr=S_OK;
    
    m_conn=PQconnectdb(OLE2CA(connectString));
    if( PQstatus(m_conn)!=CONNECTION_OK ) {
        hr=E_FAIL;
        const char *error=PQerrorMessage(); // XXX - need to pass this string to the caller
        AtlTrace2(atlTraceDBProvider, 0, "PgSource::PgConnectDB failed to open database connection:\n%s",
            error);
        
        MessageBox( NULL, error, "CPgSession::PgConnectDB error", MB_ICONEXCLAMATION|MB_OK );
    } else {
        {
            // Set the client encoding to UTF-8
            // XXX - Should, ideally, check return status. I'm not sure what we are supposed to do on failure here, though.
            PGresult *res=PQexec( "SET client_encoding TO \"Unicode\"" );

            PQclear( res );
        }
        /* Find out if our database has any custom types, and if so, which, and what is
         * their OID.
         */

        // If this assert fails, it means that the two arrays were defined with
        // different dimensions
        ATLASSERT(sizeof(s_typenames)/sizeof(s_typenames[0])==
            sizeof(s_cust_types_type)/sizeof(s_cust_types_type[0]));

        for( int i=0; i<(sizeof(s_typenames)/sizeof(s_typenames[0])); ++i ) {
            // If this assert fails, it means that the number at the declaration of
            // s_typenames doesn't match the number of actual definitions
            ATLASSERT(s_typenames!=NULL && s_cust_types_type[i].wType!=DBTYPE_EMPTY);

            _bstr_t query("select oid from pg_type where typname='");
            query+=s_typenames[i];
            query+="'";

            PGresult *res=PQexec( query );

            if( PQresultStatus( res )==PGRES_TUPLES_OK && PQntuples(res)==1 ) {
                // There is such a type. Get it's OID and put it in the map
                unsigned int oid=
                    ntohl(*reinterpret_cast<unsigned long *>(PQgetvalue( res, 0, 0 )));
                m_types[oid]=s_cust_types_type[i];

                std::map<DBTYPE, unsigned int>::iterator j=m_ole_oid_map.find(s_cust_types_type[i].wType);

                if( j==m_ole_oid_map.end() ) {
                    // No mapping to this DBTYPE
                    m_ole_oid_map[s_cust_types_type[i].wType]=oid;
                } else {
                    // Take the one that has a higher accuracy
                    if( m_types[j->second].bPrecision>m_types[oid].bPrecision ) {
                        j->second=oid;
                    }
                }
            }

            PQclear( res );
        }

        // Add the rest (non-custom) types we know how to handle

        // If this assert fails, it means that the two arrays were defined with
        // different dimensions
        ATLASSERT(sizeof(s_types_oids)/sizeof(s_types_oids[0])==
            sizeof(s_types_type)/sizeof(s_types_type[0]));

        for( /* int */ i=0; i<(sizeof(s_types_oids)/sizeof(s_types_oids[0])); ++i ) {
            // If this assert fails, it means that the number at the declaration of
            // s_types_oids doesn't match the number of actual definitions
            ATLASSERT(s_types_oids!=0 && s_types_type[i].wType!=DBTYPE_EMPTY);

            m_types[s_types_oids[i]]=s_types_type[i];

            std::map<DBTYPE, unsigned int>::iterator j=
                m_ole_oid_map.find(s_types_type[i].wType);
            
            if( j==m_ole_oid_map.end() ) {
                // No mapping to this DBTYPE
                m_ole_oid_map[s_types_type[i].wType]=s_types_oids[i];
            } else {
                // Take the one that has a higher accuracy
                if( m_types[j->second].bPrecision>m_types[s_types_oids[i]].bPrecision ) {
                    j->second=s_types_oids[i];
                }
            }
        }

        // Some information is only available through the pg_type catalog table
        PGresult *res=PQexec("SELECT oid, typalign FROM pg_type");

        if( PQresultStatus( res )==PGRES_TUPLES_OK ) {
            for( i=0; i<PQntuples(res); ++i ) {
                unsigned long oid=ntohl(*reinterpret_cast<unsigned long *>(PQgetvalue( res, i, 0 )));
                std::map<unsigned int, typeinfo>::iterator j=m_types.find(oid);
                if( j!=m_types.end() && j->second.alignment==0 ) {
                    // We have the type this row refers to in our map,
                    // and there is no alignment override for it
                    switch( *reinterpret_cast<const char *>(PQgetvalue( res, i, 1)) ) {
                    case 'c':
                        j->second.alignment=1;
                        break;
                    case 's':
                        j->second.alignment=2;
                        break;
                    case 'i':
                        j->second.alignment=4;
                        break;
                    case 'd':
                        j->second.alignment=8;
                        break;
                    default:
                        ATLASSERT(!"Unknown alignment char!");
                        j->second.alignment=1;
                        break;
                    }
                }
            }
        } else {
            // Pretty serious condition - catalog table not queryable!
            ATLASSERT( !"pg_type catalog table not queryable!" );
            hr=E_FAIL;
        }

        PQclear(res);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CPgSession::GetOptionsObject( 
        /* [out] */ ITransactionOptions __RPC_FAR *__RPC_FAR *ppOptions)
{
    return DB_E_NOTSUPPORTED;
}

HRESULT STDMETHODCALLTYPE CPgSession::StartTransaction( /* [in] */ ISOLEVEL isoLevel,
        /* [in] */ ULONG isoFlags, /* [in] */ ITransactionOptions __RPC_FAR *pOtherOptions,
        /* [out] */ ULONG __RPC_FAR *pulTransactionLevel)
{
    // Sanity call and params
    if( isoFlags!=0 )
        return XACT_E_NOISORETAIN;

    if( m_transaction )
        return XACT_E_XTIONEXISTS;

    ZeroMemory( &m_transactioninfo, sizeof( m_transactioninfo ) );
    
    *reinterpret_cast<unsigned long *>(&m_transactioninfo.uow.rgb[0])=++m_transid;
    m_transactioninfo.isoFlags=isoFlags;
    // Commented out for performance
    // m_transactioninfo.grfTCSupported=0;

    _bstr_t query="START TRANSACTION ";

    switch( isoLevel ) {
    case ISOLATIONLEVEL_UNSPECIFIED:
    case ISOLATIONLEVEL_CHAOS:
        // All unsupported modes go here
        return XACT_E_ISOLATIONLEVEL;
        break;
    case ISOLATIONLEVEL_READUNCOMMITTED:
        // Unsupported - stengthen to Read Commited
    case ISOLATIONLEVEL_READCOMMITTED:
        m_transactioninfo.isoLevel=ISOLATIONLEVEL_READCOMMITTED;
        query+="ISOLATION LEVEL READ COMMITTED";
        break;
    case ISOLATIONLEVEL_REPEATABLEREAD:
        // Unsupported - strengthen to serializable
    case ISOLATIONLEVEL_SERIALIZABLE:
        m_transactioninfo.isoLevel=ISOLATIONLEVEL_SERIALIZABLE;
        query+="ISOLATION LEVEL SERIALIZABLE";
        break;
    default:
        return XACT_E_ISOLATIONLEVEL;
        break;
    }

    PGresult *query_res=PQexec( query );

    HRESULT res=S_OK;

    if( PQresultStatus( query_res )==PGRES_COMMAND_OK ) {
        m_transaction=true;
        if( pulTransactionLevel!=NULL )
            *pulTransactionLevel=1;
    } else {
        res=E_FAIL;
        const char *error=PQerrorMessage(); // XXX - need to pass this string to the caller
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::StartTransacion failed:\n%s",
            error);
        
        MessageBox( NULL, error, "CPgSession::StartTransacion failed", MB_ICONEXCLAMATION|MB_OK );
    }

    PQclear( query_res );

    return res;
}

HRESULT STDMETHODCALLTYPE CPgSession::Commit( /* [in] */ BOOL fRetaining, /* [in] */ DWORD grfTC,
        /* [in] */ DWORD grfRM)
{
    if( !m_transaction ) {
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::Commit called not inside a transaction\n");
        return XACT_E_NOTRANSACTION;
    }

    if( grfTC!=0 || grfRM!=0 ) {
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::Commit called with unsupported flags\n");
        return XACT_E_NOTSUPPORTED;
    }

    PGresult *query_res=PQexec("COMMIT");

    HRESULT res=S_OK;

    if( PQresultStatus( query_res )==PGRES_COMMAND_OK ) {
        m_transaction=false;

        if( fRetaining )
            // We do not support retaining
            res=XACT_E_CANTRETAIN;
    } else {
        res=E_FAIL;
        const char *error=PQerrorMessage(); // XXX - need to pass this string to the caller
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::Commit failed:\n%s",
            error);
        
        MessageBox( NULL, error, "CPgSession::StartTransacion failed", MB_ICONEXCLAMATION|MB_OK );
    }

    PQclear( query_res );

    return res;
}

HRESULT STDMETHODCALLTYPE CPgSession::Abort( /* [unique][in] */ BOID __RPC_FAR *pboidReason,
        /* [in] */ BOOL fRetaining, /* [in] */ BOOL fAsync)
{
    if( !m_transaction ) {
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::Abort called not inside a transaction\n");
        return XACT_E_NOTRANSACTION;
    }

    if( fAsync ) {
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::Abort called with unsupported fAsync=TRUE\n");
        return XACT_E_NOTSUPPORTED;
    }

    PGresult *query_res=PQexec("ROLLBACK");

    HRESULT res=S_OK;

    if( PQresultStatus( query_res )==PGRES_COMMAND_OK ) {
        m_transaction=false;

        if( fRetaining )
            // We do not support retaining
            res=XACT_E_CANTRETAIN;
    } else {
        res=E_FAIL;
        const char *error=PQerrorMessage(); // XXX - need to pass this string to the caller
        AtlTrace2(atlTraceDBProvider, 0, "CPgSession::Abort failed:\n%s",
            error);
        
        MessageBox( NULL, error, "CPgSession::StartTransacion failed", MB_ICONEXCLAMATION|MB_OK );
    }

    PQclear( query_res );

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CPgSession::GetTransactionInfo(
        /* [out] */ XACTTRANSINFO __RPC_FAR *pinfo)
{
    if( m_transaction ) {
        *pinfo=m_transactioninfo;
    } else {
        return XACT_E_NOTRANSACTION;
    }

    return S_OK;
}
