// AdvProp.h : Declaration of the CAdvProp
/*
 *   PostgreSQL OLE DB Provider 
 *   LGPL License  
 *   initial author Marek Mosiewicz (marekmosiewicz@poczta.onet.pl)
 *                  Jotel Poland 
 *
 *   Advanced properties page
 */

#ifndef __ADVPROP_H_
#define __ADVPROP_H_

#include "resource.h"       // main symbols

EXTERN_C const CLSID CLSID_AdvProp;

/////////////////////////////////////////////////////////////////////////////
// CAdvProp
class ATL_NO_VTABLE CAdvProp :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAdvProp, &CLSID_AdvProp>,
	public IPropertyPageImpl<CAdvProp>,
	public CDialogImpl<CAdvProp>,
	public IPersistPropertyBagImpl<CAdvProp>
{
public:
	CAdvProp() 
	{
		m_dwTitleID = IDS_TITLEAdvProp;
		m_dwHelpFileID = IDS_HELPFILEAdvProp;
		m_dwDocStringID = IDS_DOCSTRINGAdvProp;
	}

	enum {IDD = IDD_ADVPROP};

DECLARE_REGISTRY_RESOURCEID(IDR_ADVPROP)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAdvProp) 
	COM_INTERFACE_ENTRY(IPropertyPage)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
END_COM_MAP()

BEGIN_MSG_MAP(CAdvProp)
	CHAIN_MSG_MAP(IPropertyPageImpl<CAdvProp>)
END_MSG_MAP()

BOOL m_bRequiresSave;
BEGIN_PROP_MAP( CAdvProp )
END_PROP_MAP( )

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	STDMETHOD(Apply)(void)
	{
		ATLTRACE(_T("CAdvProp::Apply\n"));
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
};

#endif //__ADVPROP_H_
