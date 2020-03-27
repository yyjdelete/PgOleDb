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

// PgDS.h : Declaration of the CPgSource
#ifndef __CPgSource_H_
#define __CPgSource_H_
#include "resource.h"       // main symbols
#include "PgSess.h"
#include "PgRS.h"
/////////////////////////////////////////////////////////////////////////////
// CDataSource
class ATL_NO_VTABLE CPgSource : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPgSource, &CLSID_Pg>,
	public IDBCreateSessionImpl<CPgSource, CPgSession>,
	public IDBInitializeImpl<CPgSource>,
	public IDBPropertiesImpl<CPgSource>,
	public IPersistImpl<CPgSource>,
	public IInternalConnectionImpl<CPgSource>
{
private:
    _bstr_t m_strConnect;

    HRESULT RefreshConnectString();
    static void ReleaseProperties( DBPROPSET *properties, ULONG numproperties )
    {
        for( ULONG i=0; i<numproperties; ++i ) {
            for( ULONG j=0; j<properties[i].cProperties; ++j ) {
                VariantClear(&properties[i].rgProperties[j].vValue );
            }
            CoTaskMemFree(properties[i].rgProperties);
        }
        CoTaskMemFree(properties);
    }
    // Format a value for inclusion in the connection string
    void FormatParameter( _bstr_t &param, const VARIANT &val );
public:
	HRESULT FinalConstruct()
	{
		return FInit();
	}

	STDMETHOD(CreateSession)(IUnknown *pUnkOuter,
							 REFIID riid,
							 IUnknown **ppDBSession);

	STDMETHOD(SetProperties)(ULONG cPropertySets,
							 DBPROPSET rgPropertySets[]) {
        HRESULT hr=IDBPropertiesImpl<CPgSource>::SetProperties(cPropertySets, rgPropertySets );

        if( SUCCEEDED(hr) )
            // Invalidate the connection string cache
            m_strConnect=OLESTR("");

        return hr;
    }
DECLARE_REGISTRY_RESOURCEID(IDR_PG)
BEGIN_PROPSET_MAP(CPgSource)
	BEGIN_PROPERTY_SET(DBPROPSET_DATASOURCEINFO)
		PROPERTY_INFO_ENTRY_VALUE(ACTIVESESSIONS, 0)
		PROPERTY_INFO_ENTRY_VALUE(DATASOURCEREADONLY, VARIANT_TRUE)
		PROPERTY_INFO_ENTRY(BYREFACCESSORS)
		PROPERTY_INFO_ENTRY(OUTPUTPARAMETERAVAILABILITY)
		PROPERTY_INFO_ENTRY(PROVIDEROLEDBVER)
		PROPERTY_INFO_ENTRY(DSOTHREADMODEL)
		PROPERTY_INFO_ENTRY(SUPPORTEDTXNISOLEVELS)
		PROPERTY_INFO_ENTRY(USERNAME)
        PROPERTY_INFO_ENTRY_VALUE(PROCEDURETERM, L"function")
	END_PROPERTY_SET(DBPROPSET_DATASOURCEINFO)
	BEGIN_PROPERTY_SET(DBPROPSET_DBINIT)
		PROPERTY_INFO_ENTRY(AUTH_PASSWORD) // password=
		//PROPERTY_INFO_ENTRY(AUTH_PERSIST_SENSITIVE_AUTHINFO)
		PROPERTY_INFO_ENTRY(AUTH_USERID) // user=
		PROPERTY_INFO_ENTRY(INIT_DATASOURCE) // host=
		//PROPERTY_INFO_ENTRY(INIT_HWND)
		//PROPERTY_INFO_ENTRY(INIT_LCID)
		PROPERTY_INFO_ENTRY(INIT_LOCATION) // dbname=
		//PROPERTY_INFO_ENTRY(INIT_MODE)
		//PROPERTY_INFO_ENTRY(INIT_PROMPT)
		PROPERTY_INFO_ENTRY(INIT_PROVIDERSTRING) // options=
		PROPERTY_INFO_ENTRY(INIT_TIMEOUT) // connect_timeout=
        // Not here - sslmode, hostaddr, port, service
	END_PROPERTY_SET(DBPROPSET_DBINIT)
	CHAIN_PROPERTY_SET(CPgCommand)
END_PROPSET_MAP()
BEGIN_COM_MAP(CPgSource)
	COM_INTERFACE_ENTRY(IDBCreateSession)
	COM_INTERFACE_ENTRY(IDBInitialize)
	COM_INTERFACE_ENTRY(IDBProperties)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IInternalConnection)
END_COM_MAP()
public:
};
#endif //__CPgSource_H_
