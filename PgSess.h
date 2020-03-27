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

// Session.h : Declaration of the CPgSession
#ifndef __CPgSession_H_
#define __CPgSession_H_
#include "resource.h"       // main symbols
#include "PgRS.h"

/////////////////////////////////////////////////////////////////////////////
// CPgSession
class ATL_NO_VTABLE CPgSession : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IGetDataSourceImpl<CPgSession>,
	public IOpenRowsetImpl<CPgSession>,
	public ISessionPropertiesImpl<CPgSession>,
	public IObjectWithSiteSessionImpl<CPgSession>,
	public IDBSchemaRowsetImpl<CPgSession>,
	public IDBCreateCommandImpl<CPgSession, CPgCommand>,
    public ITransactionLocal,
	public IPgSession
{
    PGconn *m_conn;
public:
    CPgSession() : m_conn(NULL), m_transaction(false), m_transid(0)
	{
	}
	HRESULT FinalConstruct()
	{
		return FInit();
	}
	STDMETHOD(OpenRowset)(IUnknown *pUnk, DBID *pTID, DBID *pInID, REFIID riid,
					   ULONG cSets, DBPROPSET rgSets[], IUnknown **ppRowset)
	{
		CPgRowset* pRowset;
		return CreateRowset(pUnk, pTID, pInID, riid, cSets, rgSets, ppRowset, pRowset);
	}
    STDMETHOD(PgConnectDB)( BSTR connectString );
    ~CPgSession()
    {
        PQfinish(m_conn);
        m_conn=NULL;
    }

    PGresult *PQexec(const char *query)
    {
        ATLTRACE2(atlTraceDBProvider, 0, "CPgSession::PQexec \"%.400s\"\n", query);

        PGresult *res=::PQexecParams( m_conn, query, 0, NULL, NULL, NULL, NULL, 1 );

        return res;
    }
    char *PQerrorMessage()
    {
        return ::PQerrorMessage( m_conn );
    }

    const typeinfo *GetTypeInfo( unsigned int oid ) const {
        types_type::const_iterator i=m_types.find(oid);

        if( i!=m_types.end() ) {
            return &(i->second);
        } else {
            return NULL;
        }
    }
private:
    // All custom types are known by name, not OID.
    // Can't use a symbolic constant due to C++ syntax.
    // Must be same number for following two statements - enforced using ASSERT
    static const char *s_typenames[1];
    static const typeinfo s_cust_types_type[1];
    // Can't use a symbolic constant due to C++ syntax.
    // Must be same number for following two statements - enforced using ASSERT
    static const unsigned long s_types_oids[10];
    static const typeinfo s_types_type[10];

public:
private:
    types_type m_types;
public:
BEGIN_PROPSET_MAP(CPgSession)
	BEGIN_PROPERTY_SET(DBPROPSET_SESSION)
		PROPERTY_INFO_ENTRY(SESS_AUTOCOMMITISOLEVELS)
	END_PROPERTY_SET(DBPROPSET_SESSION)
END_PROPSET_MAP()
BEGIN_COM_MAP(CPgSession)
	COM_INTERFACE_ENTRY(IGetDataSource)
	COM_INTERFACE_ENTRY(IOpenRowset)
	COM_INTERFACE_ENTRY(ISessionProperties)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDBCreateCommand)
	COM_INTERFACE_ENTRY(IDBSchemaRowset)
//    COM_INTERFACE_ENTRY(ITransactionLocal)
	COM_INTERFACE_ENTRY(IPgSession)
END_COM_MAP()
BEGIN_SCHEMA_MAP(CPgSession)
//	SCHEMA_ENTRY(DBSCHEMA_TABLES, CPgSessionTRSchemaRowset)
//	SCHEMA_ENTRY(DBSCHEMA_COLUMNS, CPgSessionColSchemaRowset)
//	SCHEMA_ENTRY(DBSCHEMA_PROVIDER_TYPES, CPgSessionPTSchemaRowset)
END_SCHEMA_MAP()
public:
    // ITransactionLocal methods
    virtual HRESULT STDMETHODCALLTYPE Commit( 
        /* [in] */ BOOL fRetaining,
        /* [in] */ DWORD grfTC,
        /* [in] */ DWORD grfRM);
        
    virtual HRESULT STDMETHODCALLTYPE Abort( 
        /* [unique][in] */ BOID __RPC_FAR *pboidReason,
        /* [in] */ BOOL fRetaining,
        /* [in] */ BOOL fAsync);
    
    virtual HRESULT STDMETHODCALLTYPE GetTransactionInfo( 
        /* [out] */ XACTTRANSINFO __RPC_FAR *pinfo);

    virtual HRESULT STDMETHODCALLTYPE GetOptionsObject( 
        /* [out] */ ITransactionOptions __RPC_FAR *__RPC_FAR *ppOptions);
    
    virtual HRESULT STDMETHODCALLTYPE StartTransaction( 
        /* [in] */ ISOLEVEL isoLevel,
        /* [in] */ ULONG isoFlags,
        /* [in] */ ITransactionOptions __RPC_FAR *pOtherOptions,
        /* [out] */ ULONG __RPC_FAR *pulTransactionLevel);

private:
    bool m_transaction; // Whether there is a transaction in progress.
    XACTTRANSINFO m_transactioninfo;
    unsigned long m_transid;
};

#endif //__CPgSession_H_
