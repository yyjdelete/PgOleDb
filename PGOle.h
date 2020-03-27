/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Jan 19 13:41:41 2003
 */
/* Compiler settings for C:\Documents and Settings\Marek\Moje dokumenty\Visual Studio Projects\PGOle\PGOle.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __PGOle_h__
#define __PGOle_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IPGConnProp_FWD_DEFINED__
#define __IPGConnProp_FWD_DEFINED__
typedef interface IPGConnProp IPGConnProp;
#endif 	/* __IPGConnProp_FWD_DEFINED__ */


#ifndef __IPGOpenConnection_FWD_DEFINED__
#define __IPGOpenConnection_FWD_DEFINED__
typedef interface IPGOpenConnection IPGOpenConnection;
#endif 	/* __IPGOpenConnection_FWD_DEFINED__ */


#ifndef __ITest_FWD_DEFINED__
#define __ITest_FWD_DEFINED__
typedef interface ITest ITest;
#endif 	/* __ITest_FWD_DEFINED__ */


#ifndef __PGOle_FWD_DEFINED__
#define __PGOle_FWD_DEFINED__

#ifdef __cplusplus
typedef class PGOle PGOle;
#else
typedef struct PGOle PGOle;
#endif /* __cplusplus */

#endif 	/* __PGOle_FWD_DEFINED__ */


#ifndef __ConnProp_FWD_DEFINED__
#define __ConnProp_FWD_DEFINED__

#ifdef __cplusplus
typedef class ConnProp ConnProp;
#else
typedef struct ConnProp ConnProp;
#endif /* __cplusplus */

#endif 	/* __ConnProp_FWD_DEFINED__ */


#ifndef __AdvProp_FWD_DEFINED__
#define __AdvProp_FWD_DEFINED__

#ifdef __cplusplus
typedef class AdvProp AdvProp;
#else
typedef struct AdvProp AdvProp;
#endif /* __cplusplus */

#endif 	/* __AdvProp_FWD_DEFINED__ */


#ifndef __Test_FWD_DEFINED__
#define __Test_FWD_DEFINED__

#ifdef __cplusplus
typedef class Test Test;
#else
typedef struct Test Test;
#endif /* __cplusplus */

#endif 	/* __Test_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IPGConnProp_INTERFACE_DEFINED__
#define __IPGConnProp_INTERFACE_DEFINED__

/* interface IPGConnProp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPGConnProp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("40F64D42-1809-4C53-8CE4-81695FD461B5")
    IPGConnProp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_datasource( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_datasource( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_server( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_server( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_password( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_password( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_user( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_user( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_database( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_database( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPGConnPropVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPGConnProp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPGConnProp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPGConnProp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_datasource )( 
            IPGConnProp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_datasource )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_server )( 
            IPGConnProp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_server )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_password )( 
            IPGConnProp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_password )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_user )( 
            IPGConnProp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_user )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_database )( 
            IPGConnProp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_database )( 
            IPGConnProp __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IPGConnPropVtbl;

    interface IPGConnProp
    {
        CONST_VTBL struct IPGConnPropVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPGConnProp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPGConnProp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPGConnProp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPGConnProp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPGConnProp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPGConnProp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPGConnProp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPGConnProp_get_datasource(This,pVal)	\
    (This)->lpVtbl -> get_datasource(This,pVal)

#define IPGConnProp_put_datasource(This,newVal)	\
    (This)->lpVtbl -> put_datasource(This,newVal)

#define IPGConnProp_get_server(This,pVal)	\
    (This)->lpVtbl -> get_server(This,pVal)

#define IPGConnProp_put_server(This,newVal)	\
    (This)->lpVtbl -> put_server(This,newVal)

#define IPGConnProp_get_password(This,pVal)	\
    (This)->lpVtbl -> get_password(This,pVal)

#define IPGConnProp_put_password(This,newVal)	\
    (This)->lpVtbl -> put_password(This,newVal)

#define IPGConnProp_get_user(This,pVal)	\
    (This)->lpVtbl -> get_user(This,pVal)

#define IPGConnProp_put_user(This,newVal)	\
    (This)->lpVtbl -> put_user(This,newVal)

#define IPGConnProp_get_database(This,pVal)	\
    (This)->lpVtbl -> get_database(This,pVal)

#define IPGConnProp_put_database(This,newVal)	\
    (This)->lpVtbl -> put_database(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPGConnProp_get_datasource_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IPGConnProp_get_datasource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IPGConnProp_put_datasource_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IPGConnProp_put_datasource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPGConnProp_get_server_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IPGConnProp_get_server_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IPGConnProp_put_server_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IPGConnProp_put_server_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPGConnProp_get_password_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IPGConnProp_get_password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IPGConnProp_put_password_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IPGConnProp_put_password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPGConnProp_get_user_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IPGConnProp_get_user_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IPGConnProp_put_user_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IPGConnProp_put_user_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPGConnProp_get_database_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IPGConnProp_get_database_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IPGConnProp_put_database_Proxy( 
    IPGConnProp __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IPGConnProp_put_database_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPGConnProp_INTERFACE_DEFINED__ */


#ifndef __IPGOpenConnection_INTERFACE_DEFINED__
#define __IPGOpenConnection_INTERFACE_DEFINED__

/* interface IPGOpenConnection */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IPGOpenConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("31A2F8B1-E5F9-4064-8B3A-FC5ED3F55284")
    IPGOpenConnection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR query) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPGOpenConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPGOpenConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPGOpenConnection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPGOpenConnection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            IPGOpenConnection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            IPGOpenConnection __RPC_FAR * This,
            /* [in] */ BSTR query);
        
        END_INTERFACE
    } IPGOpenConnectionVtbl;

    interface IPGOpenConnection
    {
        CONST_VTBL struct IPGOpenConnectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPGOpenConnection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPGOpenConnection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPGOpenConnection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPGOpenConnection_Init(This)	\
    (This)->lpVtbl -> Init(This)

#define IPGOpenConnection_Execute(This,query)	\
    (This)->lpVtbl -> Execute(This,query)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPGOpenConnection_Init_Proxy( 
    IPGOpenConnection __RPC_FAR * This);


void __RPC_STUB IPGOpenConnection_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPGOpenConnection_Execute_Proxy( 
    IPGOpenConnection __RPC_FAR * This,
    /* [in] */ BSTR query);


void __RPC_STUB IPGOpenConnection_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPGOpenConnection_INTERFACE_DEFINED__ */


#ifndef __ITest_INTERFACE_DEFINED__
#define __ITest_INTERFACE_DEFINED__

/* interface ITest */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56631CBD-67B9-4E8D-8E3B-65906C664F0D")
    ITest : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ITestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITest __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITest __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITest __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITest __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITest __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITest __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITest __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } ITestVtbl;

    interface ITest
    {
        CONST_VTBL struct ITestVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITest_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITest_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITest_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITest_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITest_INTERFACE_DEFINED__ */



#ifndef __PGOLELib_LIBRARY_DEFINED__
#define __PGOLELib_LIBRARY_DEFINED__

/* library PGOLELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PGOLELib;

EXTERN_C const CLSID CLSID_PGOle;

#ifdef __cplusplus

class DECLSPEC_UUID("A6FB38B9-4B5C-4409-AEA5-CBBEA6557C62")
PGOle;
#endif

EXTERN_C const CLSID CLSID_ConnProp;

#ifdef __cplusplus

class DECLSPEC_UUID("9A3F6292-0700-44A0-8982-0BB9AB27E53A")
ConnProp;
#endif

EXTERN_C const CLSID CLSID_AdvProp;

#ifdef __cplusplus

class DECLSPEC_UUID("02C9EA86-6C9F-43B2-9DB3-B914705CE5ED")
AdvProp;
#endif

EXTERN_C const CLSID CLSID_Test;

#ifdef __cplusplus

class DECLSPEC_UUID("A5455662-5CCD-49B7-ADE1-8EDED02AF8A1")
Test;
#endif
#endif /* __PGOLELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
