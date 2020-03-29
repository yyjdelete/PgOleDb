// ErrorLookupService.h: Definition of the CErrorLookupService class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORLOOKUPSERVICE_H__128D333A_3D8A_4F51_A81A_DC8410770426__INCLUDED_)
#define AFX_ERRORLOOKUPSERVICE_H__128D333A_3D8A_4F51_A81A_DC8410770426__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "OleDb.h"

/////////////////////////////////////////////////////////////////////////////
// CErrorLookupService

class CErrorLookupService : 
	public IPgErrorLookup,
    public IErrorLookup,
	public CComObjectRootEx<CComGlobalsThreadModel>,
	public CComCoClass<CErrorLookupService,&CLSID_ErrorLookupService>
{
private:
    static DWORD m_current_msgid;
    static std::map<DWORD, _bstr_t> m_error_messages;
public:
    CErrorLookupService()
    {
    }
	~CErrorLookupService()
    {
    }
BEGIN_COM_MAP(CErrorLookupService)
	COM_INTERFACE_ENTRY(IErrorLookup)
	COM_INTERFACE_ENTRY(IPgErrorLookup)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(CErrorLookupService) 

DECLARE_REGISTRY_RESOURCEID(IDR_ErrorLookupService)
                                              
// IPgErrorLookup
public:
	STDMETHOD(RegisterCustomErrorMessage)(/*[in]*/ BSTR Msg, /*[out,retval]*/ DWORD *DEID);
    STDMETHOD(GetErrorDescription)( 
        /* [in] */ HRESULT hrError,
        /* [in] */ DWORD dwLookupID,
        /* [in] */ DISPPARAMS __RPC_FAR *pdispparams,
        /* [in] */ LCID lcid,
        /* [out] */ BSTR __RPC_FAR *pbstrSource,
        /* [out] */ BSTR __RPC_FAR *pbstrDescription);
    STDMETHOD(GetHelpInfo)( 
        /* [in] */ HRESULT hrError,
        /* [in] */ DWORD dwLookupID,
        /* [in] */ LCID lcid,
        /* [out] */ BSTR __RPC_FAR *pbstrHelpFile,
        /* [out] */ DWORD __RPC_FAR *pdwHelpContext);

    STDMETHOD(ReleaseErrors)(const DWORD dwDynamicErrorID);

    static void ClearError() {
        ATLTRACE2(atlTraceDBProvider, 0, "CErrorLookupService::ClearError\n" );
        SetErrorInfo(0, NULL);
    }
    static void ReportError(HRESULT hres, const IID &Interface);
    static void ReportCustomError(const char* message, HRESULT hError, const IID &Interface);
private:
    static void InternalReportError(HRESULT hError, const IID &Interface, DWORD dwDynamicErrorID );
};

#endif // !defined(AFX_ERRORLOOKUPSERVICE_H__128D333A_3D8A_4F51_A81A_DC8410770426__INCLUDED_)
