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

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__CAB4C541_75BB_4579_9E2B_40F8DD61B3E7__INCLUDED_)
#define AFX_STDAFX_H__CAB4C541_75BB_4579_9E2B_40F8DD61B3E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS//stricmp=>_stricmp

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif

#include <windows.h>

//#define _ATL_FREE_THREADED//by default
//#define _ATL_APARTMENT_THREADED
#define ATL_TRACE_LEVEL 3
#define COM_STDMETHOD_CAN_THROW
//#define _ATL_DEBUG_PROVIDER_PROPS
//#define OLEDBVER 0x270

#ifdef TRACEOVERRIDE
//PgAtlTrace
#define ATLTRACE PgAtlTrace2
#define ATLTRACE2 PgAtlTrace2
void _cdecl PgAtlTrace2(int category, UINT level, _Printf_format_string_ LPCTSTR lpszFormat, ...);
void _cdecl PgAtlTrace2(_Printf_format_string_ LPCTSTR lpszFormat, ...);
#endif

extern int gLogLevel;

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <atldb.h>
#include <comdef.h>

// A few STL includes
#include <map>
#include <vector>
#include <list>
#include <string>

// Postgresql libpq includes
#include <libpq-fe.h>
//XXX: Disable due to too many confict
//// Allow the debugger to see into the conn abstraction
//#ifdef _DEBUG
//#include <libpq-int.h>
//#endif

#include "Unicode.h"
#include "autoarray.h"

// catalog/pg_type.h holds the type information

// Class used to report error conditions using exceptions
class PgOleError {
    HRESULT m_hr;
    _bstr_t m_message;
public:
    explicit PgOleError( HRESULT hr, const char *message="" ) : m_hr(hr), m_message(message)
    {
    }
    HRESULT hr() const {
        return m_hr;
    }
    const char *str() const {
        return m_message;
    }
};

template <class T>
void printarg(T t)
{
   std::cout << t << std::endl;
}
template <class ...Args>
void expand(Args... args)
{
   int arr[] = {(printarg(args), 0)...};
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

class ATL_NO_VTABLE ISupportErrorInfoImplAlways :
    public ISupportErrorInfo
{
public:
    STDMETHOD(InterfaceSupportsErrorInfo)(_In_ REFIID riid)
    {
        DumpInterfaceSupportsErrorInfo(riid);
        return S_OK;
    }

    static void DumpInterfaceSupportsErrorInfo(_In_ REFIID riid)
    {
        OLECHAR guidStr[39];
        StringFromGUID2(riid, guidStr, 39);
        USES_CONVERSION;
        //TODO: Try also read name from HKEY_CLASSES_ROOT/Interface/riid/
        char* guidStrA = OLE2A(guidStr);
        ATLTRACE2("InterfaceSupportsErrorInfo %s\n", guidStrA);
    }
};

template <const IID* ... piids>
class ATL_NO_VTABLE ISupportErrorInfoImplArray :
    public ISupportErrorInfo
{
public:
    STDMETHOD(InterfaceSupportsErrorInfo)(_In_ REFIID riid)
    {
        ISupportErrorInfoImplAlways::DumpInterfaceSupportsErrorInfo(riid);
        static const IID* arr[] = { const_cast<const IID*>(piids)... };//Report `E0067` but build success??

        for (int i = 0; i < sizeof...(piids); i++)
        {
            if (InlineIsEqualGUID(*arr[i], riid))
                return S_OK;
        }

        return S_FALSE;
    }
};

#endif // !defined(AFX_STDAFX_H__CAB4C541_75BB_4579_9E2B_40F8DD61B3E7__INCLUDED)
