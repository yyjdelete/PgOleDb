// Session.h : Declaration of the CPGOleSession
/*
 *   PostgreSQL OLE DB Provider 
 *   LGPL License  
 *   initial author Marek Mosiewicz (marekmosiewicz@poczta.onet.pl)
 *                  Jotel Poland 
 *
 *   OLE DB Session object
 *	 Schema information objects  CPGOleSessionPTSchemaRowset;CPGOleSessionColSchemaRowset
 */


#ifndef __CPGOleSession_H_
#define __CPGOleSession_H_
#include "resource.h"       // main symbols
#include "PGOleRS.h"
class CPGOleSessionTRSchemaRowset;
class CPGOleSessionColSchemaRowset;
class CPGOleSessionPTSchemaRowset;
#include "pg_props.h"
#include "libpq-fe.h"

/////////////////////////////////////////////////////////////////////////////
// CPGOleSession
class ATL_NO_VTABLE CPGOleSession : 
	public IPGOpenConnection,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IGetDataSourceImpl<CPGOleSession>,
	public IOpenRowsetImpl<CPGOleSession>,
	public ISessionPropertiesImpl<CPGOleSession>,
	public IObjectWithSiteSessionImpl<CPGOleSession>,
	public IDBSchemaRowsetImpl<CPGOleSession>,
	public IDBCreateCommandImpl<CPGOleSession, CPGOleCommand>
{
public:
	PGconn* conn;

	CPGOleSession()
	{
		m_bInitialized = FALSE;

	}
	STDMETHOD(Init)(){
		if(m_bInitialized)
			return S_OK;
		HRESULT hr;	
		USES_CONVERSION;

		CComQIPtr<IGetDataSource> spGetSource = this;
		CComPtr<IDBProperties> spProps;
		DBPROPSET* pPropSet = NULL;
		ULONG ulPropSet = 0;
		CDBPropIDSet set(DBPROPSET_DBINIT);
		set.AddPropertyID(DBPROP_PG_DATABASE);
		set.AddPropertyID(DBPROP_PG_SERVER_PORT);
		set.AddPropertyID(DBPROP_AUTH_PASSWORD);
		set.AddPropertyID(DBPROP_AUTH_USERID);
		set.AddPropertyID(DBPROP_INIT_LOCATION);
		// Now get a pointer to the IDBProperties interface on the data source
		hr = spGetSource->GetDataSource(__uuidof(IDBProperties), (IUnknown**)&spProps);
		if (FAILED(hr))
			return hr;

		hr = spProps->GetProperties(1, &set, &ulPropSet, &pPropSet);
		if (FAILED(hr))
			return hr;
		//copy properties to PGSocket (we could avoid this)
		if (pPropSet != NULL)
		{
			CComVariant varDatabase = pPropSet->rgProperties[0].vValue;
			CComVariant varPort = pPropSet->rgProperties[1].vValue;
			varPort.ChangeType(VT_BSTR);
			CComVariant varPassword = pPropSet->rgProperties[2].vValue;
			CComVariant varUser = pPropSet->rgProperties[3].vValue;
			CComVariant varLocation = pPropSet->rgProperties[4].vValue;
			conn = PQsetdbLogin(OLE2A(varLocation.bstrVal),
							OLE2A(varPort.bstrVal),
							NULL,
							NULL,
							OLE2A(varDatabase.bstrVal),
							OLE2A(varUser.bstrVal),
							OLE2A(varPassword.bstrVal));
			if (PQstatus(conn) == CONNECTION_BAD)
			{
				return S_FALSE;
			}
			//now we support unicode only as we can't deal with other conversions
			PQsetClientEncoding(conn,"UNICODE");
			if (PQstatus(conn) == CONNECTION_BAD)
			{
				return S_FALSE;
			}

		
			m_bInitialized = TRUE;

			::CoTaskMemFree(pPropSet->rgProperties);
			::CoTaskMemFree(pPropSet);
			return S_OK;
		}	
		return S_FALSE;
	}
	STDMETHOD(Execute)(BSTR query){
		USES_CONVERSION;
		if(FAILED(Init())){
			return S_FALSE;
		};
		PGresult *res;
//		BOOL allConverted;
//		CComBSTR q;
//		q=query;
//		LPSAFEARRAY chars;
//		q.BS
//		WideCharToMultiByte(CP_UTF8,0,&chars,q.Length()+sizeof(BCHAR),result,NULL,&allConverted);

//		res = PQexec(conn, W2A(query,CP_UTF8));
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
		{
			fprintf(stderr, "BEGIN command failed\n");
			PQclear(res);
			//exit_nicely(conn);
		}
		return S_OK;
	}
	HRESULT FinalConstruct()
	{
		return FInit();
	}
	STDMETHOD(OpenRowset)(IUnknown *pUnk, DBID *pTID, DBID *pInID, REFIID riid,
					   ULONG cSets, DBPROPSET rgSets[], IUnknown **ppRowset)
	{
		CPGOleRowset* pRowset;
		return CreateRowset(pUnk, pTID, pInID, riid, cSets, rgSets, ppRowset, pRowset);
	}
BEGIN_PROPSET_MAP(CPGOleSession)
	BEGIN_PROPERTY_SET(DBPROPSET_SESSION)
		PROPERTY_INFO_ENTRY(SESS_AUTOCOMMITISOLEVELS)
	END_PROPERTY_SET(DBPROPSET_SESSION)
END_PROPSET_MAP()
BEGIN_COM_MAP(CPGOleSession)
	COM_INTERFACE_ENTRY(IPGOpenConnection)
	COM_INTERFACE_ENTRY(IGetDataSource)
	COM_INTERFACE_ENTRY(IOpenRowset)
	COM_INTERFACE_ENTRY(ISessionProperties)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDBCreateCommand)
	COM_INTERFACE_ENTRY(IDBSchemaRowset)
END_COM_MAP()
BEGIN_SCHEMA_MAP(CPGOleSession)
	SCHEMA_ENTRY(DBSCHEMA_TABLES, CPGOleSessionTRSchemaRowset)
	SCHEMA_ENTRY(DBSCHEMA_COLUMNS, CPGOleSessionColSchemaRowset)
	SCHEMA_ENTRY(DBSCHEMA_PROVIDER_TYPES, CPGOleSessionPTSchemaRowset)
END_SCHEMA_MAP()
protected:
	BOOL m_bInitialized;
};
class CPGOleSessionTRSchemaRowset : 
	public CRowsetImpl< CPGOleSessionTRSchemaRowset, CTABLESRow, CPGOleSession>
{
public:

	HRESULT Execute(LONG* pcRowsAffected, ULONG, const VARIANT*)
	{
		USES_CONVERSION;
		CPGOleWindowsFile wf;
		CTABLESRow trData;
		lstrcpyW(trData.m_szType, OLESTR("TABLE"));
		lstrcpyW(trData.m_szDesc, OLESTR("The Directory Table"));
		HANDLE hFile = INVALID_HANDLE_VALUE;
		TCHAR szDir[MAX_PATH + 1];
		DWORD cbCurDir = GetCurrentDirectory(MAX_PATH, szDir);
		lstrcat(szDir, _T("\\*.*"));
		hFile = FindFirstFile(szDir, &wf);
		if (hFile == INVALID_HANDLE_VALUE)
			return E_FAIL; // User doesn't have a c:\ drive
		FindClose(hFile);
		lstrcpynW(trData.m_szTable, T2OLE(szDir), SIZEOF_MEMBER(CTABLESRow, m_szTable));
		if (!m_rgRowData.Add(trData))
			return E_OUTOFMEMORY;
		*pcRowsAffected = 1;
		return S_OK;
	}
};
class CPGOleSessionColSchemaRowset : 
	public CRowsetImpl< CPGOleSessionColSchemaRowset, CCOLUMNSRow, CPGOleSession>
{
public:
	HRESULT Execute(LONG* pcRowsAffected, ULONG, const VARIANT*)
	{
		USES_CONVERSION;
		CPGOleWindowsFile wf;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		TCHAR szDir[MAX_PATH + 1];
		DWORD cbCurDir = GetCurrentDirectory(MAX_PATH, szDir);
		lstrcat(szDir, _T("\\*.*"));
		hFile = FindFirstFile(szDir, &wf);
		if (hFile == INVALID_HANDLE_VALUE)
			return E_FAIL; // User doesn't have a c:\ drive
		FindClose(hFile);// szDir has got the tablename
		DBID dbid;
		memset(&dbid, 0, sizeof(DBID));
		dbid.uName.pwszName = T2OLE(szDir);
		dbid.eKind = DBKIND_NAME;
		return InitFromRowset < _RowsetArrayType > (m_rgRowData, &dbid, NULL, m_spUnkSite, pcRowsAffected);
	}
};
class CPGOleSessionPTSchemaRowset : 
	public CRowsetImpl< CPGOleSessionPTSchemaRowset, CPROVIDER_TYPERow, CPGOleSession>
{
public:
	HRESULT Execute(LONG* pcRowsAffected, ULONG, const VARIANT*)
	{
		return S_OK;
	}
};
#endif //__CPGOleSession_H_
