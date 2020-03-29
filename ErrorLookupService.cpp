// ErrorLookupService.cpp : Implementation of COleDbApp and DLL registration.

#include "stdafx.h"
#include "OleDb.h"
#include "ErrorLookupService.h"

static CComQIPtr<IPgErrorLookup> LookupService;

std::map<DWORD, _bstr_t> CErrorLookupService::m_error_messages;
DWORD CErrorLookupService::m_current_msgid(1);

/////////////////////////////////////////////////////////////////////////////
//

HRESULT STDMETHODCALLTYPE CErrorLookupService::GetErrorDescription( HRESULT hrError,
        DWORD dwLookupID, DISPPARAMS __RPC_FAR *pdispparams,
        LCID lcid, BSTR __RPC_FAR *pbstrSource, BSTR __RPC_FAR *pbstrDescription)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CErrorLookupService::GetErrorDescription\n");

    HRESULT hr=S_OK;

    Lock();

    std::map<DWORD, _bstr_t>::iterator i=m_error_messages.find(dwLookupID);

    *pbstrSource=SysAllocString(L"PgOleDb");

    if( i!=m_error_messages.end() ) {
        // The dynamic ID indeed exists. Hurra!
        *pbstrDescription=SysAllocString(i->second);
        if( *pbstrDescription==NULL || *pbstrSource==NULL ) 
        {
            // Allocation failed
            hr=E_OUTOFMEMORY;
        }
    } else {
        hr=DB_E_BADDYNAMICERRORID;
    }

    Unlock();
	
    return hr;
}

HRESULT STDMETHODCALLTYPE CErrorLookupService::GetHelpInfo( HRESULT hrError, DWORD dwLookupID,
        LCID lcid, BSTR __RPC_FAR *pbstrHelpFile, DWORD __RPC_FAR *pdwHelpContext )
{
    pbstrHelpFile=NULL;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CErrorLookupService::ReleaseErrors( const DWORD dwDynamicErrorID )
{
    HRESULT hr=S_OK;

    Lock();
    std::map<DWORD, _bstr_t>::iterator i=m_error_messages.find(dwDynamicErrorID);
    
    if( i!=m_error_messages.end() ) {
        // The dynamic ID indeed exists. Hurra!
        m_error_messages.erase(i);
    } else {
        hr=DB_E_BADDYNAMICERRORID;
    }
    
    Unlock();
	
    return hr;
}

STDMETHODIMP CErrorLookupService::RegisterCustomErrorMessage(BSTR Msg, DWORD *AllocatedHRes)
{
    Lock();
    m_current_msgid=(m_current_msgid%50000)+1;
    // Make sure we are not reusing an ID, unlikely though it may be.
    ATLASSERT(m_error_messages.find(m_current_msgid)==m_error_messages.end());

    m_error_messages[m_current_msgid]=Msg;

    *AllocatedHRes=m_current_msgid;
    Unlock();

	return S_OK;
}

void CErrorLookupService::ReportError(HRESULT hError, const IID &Interface)
{
    ATLTRACE2(atlTraceDBProvider, 0, "CErrorLookupService::ReportError HRES %08x:\n", hError );

    InternalReportError(hError, Interface, 0);
}

void CErrorLookupService::ReportCustomError(const char* message, HRESULT hError,
                                            const IID &Interface )
{
    HRESULT hr;

    ATLTRACE2(atlTraceDBProvider, 0, "CErrorLookupService::ReportCustomError HRES %08x:\n%s\n",
        hError, message );

    if( LookupService==NULL ) {
        hr=::CoCreateInstance(CLSID_ErrorLookupService, NULL, CLSCTX_ALL,
            IID_IPgErrorLookup, (void **) &LookupService);
    }

    if( LookupService!=NULL ) {
        DWORD ErrorDID;
        USES_CONVERSION;
        //message is UTF8, and need be convert to UTF16
        HRESULT hres=static_cast<IPgErrorLookup *>(LookupService)->
            RegisterCustomErrorMessage(U82W(message), &ErrorDID);
        
        InternalReportError(hError, Interface, ErrorDID);
    } else {
        // Couldn't get the error report module up. Report the generic error instead
        ReportError(hError, Interface );
    }
}

void CErrorLookupService::InternalReportError(HRESULT hError, const IID &Interface,
                                              DWORD dwDynamicErrorID )
{
    CComPtr<IErrorInfo> ErrorInfo;
    HRESULT hr=GetErrorInfo(0, &ErrorInfo);

    if( FAILED(hr)||ErrorInfo==NULL ) {
        hr=ErrorInfo.CoCreateInstance(CLSID_EXTENDEDERRORINFO);
    }
    CComPtr<IErrorRecords> ErrorRecords;
    hr=ErrorInfo->QueryInterface(&ErrorRecords);

    ERRORINFO errinfo;

    errinfo.hrError=hError;
    errinfo.dwMinor=0;
    errinfo.clsid=CLSID_Pg;
    errinfo.iid=Interface;
    errinfo.dispid=0;

    hr=static_cast<IErrorRecords *>(ErrorRecords)->AddErrorRecord(&errinfo,
        (dwDynamicErrorID!=0)?dwDynamicErrorID:IDENTIFIER_SDK_ERROR, NULL, NULL, dwDynamicErrorID );

    SetErrorInfo(0, ErrorInfo);
}
