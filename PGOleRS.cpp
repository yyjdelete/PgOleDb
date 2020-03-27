// Implementation of the CPGOleCommand
#include "stdafx.h"
#include "PGOle.h"
#include "PGOleRS.h"
#include "PGOleDS.h"
#include "PGOleSess.h"

/////////////////////////////////////////////////////////////////////////////
// CPGOleCommand

HRESULT CPGOleCommand::Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
								 LONG * pcRowsAffected, IUnknown ** ppRowset)
{
	CPGOleRowset* pRowset;
	return CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRowset);
}
HRESULT CPGOleRowset::Execute(DBPARAMS * pParams, LONG* pcRowsAffected)
	{
	USES_CONVERSION;

	CPGOleRowset* pT = (CPGOleRowset*) this;
	CPGOleRowset::ObjectLock cab((CPGOleRowset*) this);
	CComPtr<IDBCreateCommand> spSession = NULL;
	CComPtr<IRowset> spRowset = NULL;
	CComPtr<IDBProperties> spProperties = NULL;
    // we must dig in to get Session Object
	HRESULT hr = pT->GetSite(IID_IDBCreateCommand, (void**) &spSession);
		if (SUCCEEDED(hr))  // The Rowset was created from an IOpenRowset::OpenRowset( )...
		{
				// Get to DBPROP_INIT_DATASOURCE property
				CComPtr<IDBCreateSession> spInit;
				CComPtr<IObjectWithSite> spCreator2 = NULL;
		
				if (FAILED(hr = spSession->QueryInterface(IID_IObjectWithSite,(void**) &spCreator2)))
				{
						ATLTRACE2(atlTraceDBProvider,0,"FATAL ERROR: Cannot get to the IObjectWithSite from ICommand...\n");
						return E_FAIL;
				}

				if (FAILED(hr = spCreator2->GetSite(IID_IDBCreateSession,(void**) &spInit)))
				{
						ATLTRACE2(atlTraceDBProvider,0,"FATAL ERROR: Cannot get to the IDBCreateSession from ICommand...\n");
						return E_FAIL;
				}

				hr = spInit->QueryInterface(IID_IDBProperties,(void**) &spProperties);
				if(FAILED(hr))
				{
					ATLTRACE2(atlTraceDBProvider,0,"FATAL ERROR: Cannot get to the IDBCreateSession'ss IDBProperties...\n");
					return hr;
				}

		}
		else // The Rowset was created from ICommand::Execute( )
		{
			CComPtr<ICommand> spCommand=NULL;
			hr = pT->GetSite(IID_ICommand,(void**) &spCommand);
			if(FAILED(hr))
			{
				ATLTRACE2(atlTraceDBProvider,0,"FATAL ERROR: Cannot get to the ICommand of the Rowset...\n");
				return E_FAIL;
			}	

			CComPtr<IObjectWithSite> spCreator = NULL;
			if (FAILED(hr = spCommand->QueryInterface(IID_IObjectWithSite,(void**) &spCreator)))
			{
					return E_FAIL;
			}
			
			if (FAILED(hr = spCreator->GetSite(IID_IDBCreateCommand,(void**) &spSession)))
			{
					return E_FAIL;
			}

			CComPtr<IPGOpenConnection> spOpenConn=NULL;
			if (FAILED(hr = spSession->QueryInterface(__uuidof(IPGOpenConnection),(void**) &spOpenConn)))
			{
					return E_FAIL;
			}
			spOpenConn->Execute(m_strCommandText);
		}		
		return S_OK;
	}



