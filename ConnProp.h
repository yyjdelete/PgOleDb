// ConnProp.h : Declaration of the CConnProp
/*
 *   PostgreSQL OLE DB Provider 
 *   LGPL License  
 *   initial author Marek Mosiewicz (marekmosiewicz@poczta.onet.pl)
 *                  Jotel Poland 
 *
 *   Connection properties page
 */

#ifndef __CONNPROP_H_
#define __CONNPROP_H_

#include "resource.h"       // main symbols

EXTERN_C const CLSID CLSID_ConnProp;

/////////////////////////////////////////////////////////////////////////////
// CConnProp
class ATL_NO_VTABLE CConnProp :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnProp, &CLSID_ConnProp>,
	public IPropertyPageImpl<CConnProp>,
	public CDialogImpl<CConnProp>,
	public IPersistPropertyBagImpl<CConnProp>,
	public IDispatchImpl<IPGConnProp, &IID_IPGConnProp, &LIBID_PGOLELib>
{
public:
	CConnProp() 
	{
		m_dwTitleID = IDS_TITLEConnProp;
		m_dwHelpFileID = IDS_HELPFILEConnProp;
		m_dwDocStringID = IDS_DOCSTRINGConnProp;
	}

	enum {IDD = IDD_CONNPROP};

DECLARE_REGISTRY_RESOURCEID(IDR_CONNPROP)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CConnProp) 
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPGConnProp)
	COM_INTERFACE_ENTRY(IPropertyPage)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
END_COM_MAP()

BEGIN_MSG_MAP(CConnProp)
	CHAIN_MSG_MAP(IPropertyPageImpl<CConnProp>)
END_MSG_MAP()

	BOOL m_bRequiresSave;
	BEGIN_PROP_MAP( CConnProp )
		PROP_DATA_ENTRY("Port Number", m_iPort, VT_INT)
		PROP_ENTRY("Data Source",1,CLSID_ConnProp)
		PROP_ENTRY("Location",2,CLSID_ConnProp)
		PROP_ENTRY("Password",3,CLSID_ConnProp)
		PROP_ENTRY("User ID",4,CLSID_ConnProp)	
		PROP_ENTRY("Database",5,CLSID_ConnProp)
	END_PROP_MAP( )

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	STDMETHOD(Apply)(void)
	{
		ATLTRACE(_T("CConnProp::Apply\n"));
		for (UINT i = 0; i < m_nObjects; i++)
		{
			// Do something interesting here
			// ICircCtl* pCirc;
			// m_ppUnk[i]->QueryInterface(IID_ICircCtl, (void**)&pCirc);
			// pCirc->put_Caption(CComBSTR("something special"));
			// pCirc->Release();
		}
		m_bDirty = FALSE;
		return S_OK;
	}
	char Password;
	VARIANT m_tPassword;
	VARIANT m_tServer;
	VARIANT m_tDatabase;
	int m_iPort;


	CComBSTR m_bstrDatasource;
	STDMETHOD(get_datasource)(BSTR* pVal){
		m_bstrDatasource.CopyTo(pVal);
		return S_OK;
	};
	STDMETHOD(put_datasource)(BSTR newVal){
		m_bstrDatasource = newVal;
		return S_OK;
	};
	CComBSTR m_bstrDatabase;
	STDMETHOD(get_database)(BSTR* pVal){
		m_bstrDatabase.CopyTo(pVal);
		return S_OK;
	};
	STDMETHOD(put_database)(BSTR newVal){
		m_bstrDatabase = newVal;
		return S_OK;
	};

	CComBSTR m_bstrPassword;
	STDMETHOD(get_password)(BSTR* pVal){
		m_bstrPassword.CopyTo(pVal);
		return S_OK;
	};
	STDMETHOD(put_password)(BSTR newVal){
		m_bstrPassword = newVal;
		return S_OK;
	};
	CComBSTR m_bstrUser;
	STDMETHOD(get_user)(BSTR* pVal){
		m_bstrUser.CopyTo(pVal);
		return S_OK;
	};
	STDMETHOD(put_user)(BSTR newVal){
		m_bstrUser = newVal;
		return S_OK;
	};

	CComBSTR m_bstrServer;
	STDMETHOD(get_server)(BSTR* pVal){
		m_bstrServer.CopyTo(pVal);
		return S_OK;
	};
	STDMETHOD(put_server)(BSTR newVal){
		m_bstrServer = newVal;
		return S_OK;
	};
	void getDialogValues(){

			GetDlgItemText(IDC_DATABASE,m_bstrDatabase.m_str);
			GetDlgItemText(IDC_SERVER,m_bstrServer.m_str);
			m_iPort = GetDlgItemInt(IDC_PORT);

			GetDlgItemText(IDC_PASSWORD,m_bstrPassword.m_str);
			GetDlgItemText(IDC_USER,m_bstrUser.m_str);
	}
	void setDialogValues(){
			USES_CONVERSION;
			SetDlgItemText(IDC_DATABASE,OLE2T(m_bstrDatabase.m_str));
			SetDlgItemText(IDC_SERVER,OLE2T(m_bstrServer.m_str));
			SetDlgItemInt(IDC_PORT,m_iPort);
			SetDlgItemText(IDC_USER,OLE2T(m_bstrUser.m_str));
			SetDlgItemText(IDC_PASSWORD,OLE2T(m_bstrPassword.m_str));

	}


	STDMETHOD(Load)(LPPROPERTYBAG pPropBag,LPERRORLOG pErrorLog ){
		HRESULT ret = IPersistPropertyBagImpl<CConnProp>::Load(pPropBag,pErrorLog);
		SetDirty(TRUE);
		m_bRequiresSave = TRUE;
		setDialogValues();
		return ret;
	}
	STDMETHOD(Save)(LPPROPERTYBAG pPropBag,BOOL fClearDirty,BOOL fSaveAllProperties){
		getDialogValues();
		return IPersistPropertyBagImpl<CConnProp>::Save(pPropBag,fClearDirty,/*fSaveAllProperties*/true);
	}


};

#endif //__CONNPROP_H_
