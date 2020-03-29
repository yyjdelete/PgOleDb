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

// OleDb.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f PgOleDbps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "OleDb.h"

#include "OleDb_i.c"
#include "PgSess.h"
#include "PgDS.h"
#include "ErrorLookupService.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Pg, CPgSource)
OBJECT_ENTRY(CLSID_ErrorLookupService, CErrorLookupService)
END_OBJECT_MAP()

static void TraceInit();
static void TraceDestroy();

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_PGOLEDBLib);
        DisableThreadLibraryCalls(hInstance);
        TraceInit();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        _Module.Term();
        TraceDestroy();
    }
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

static HANDLE hLogFile=INVALID_HANDLE_VALUE;
// 0 means only highest precedance messages are logged.
int gLogLevel=0;

static void TraceInit()
{
    HKEY hKey;

    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PostgreSQL\\OLE DB Provider"), 0,
        KEY_QUERY_VALUE, &hKey )==ERROR_SUCCESS )
    {
        DWORD dwValueSize;
        DWORD res;

        // If we're asked to override the default debug output, open the log file.
#ifdef TRACEOVERRIDE
        TCHAR filename[MAX_PATH];

        dwValueSize=sizeof(filename);
        res=RegQueryValueEx(hKey, _T("LogFile"), NULL, NULL, (LPBYTE)filename, &dwValueSize );

        if( res==ERROR_SUCCESS && dwValueSize>1 )
        {
            hLogFile=CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, // Allow reading the log while it's written
                NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
        }

        if( hLogFile!=INVALID_HANDLE_VALUE ) {
            res=SetFilePointer(hLogFile, 0, NULL, FILE_END );

            PgAtlTrace2(atlTraceDBProvider, 0, "Starting logging\n");
        }
#endif

        // Read the desired log level
        dwValueSize=sizeof(gLogLevel);
        res=RegQueryValueEx(hKey, _T("LogLevel"), NULL, NULL, (LPBYTE)&gLogLevel, &dwValueSize );
        if( res!=ERROR_SUCCESS )
            gLogLevel=0;

        RegCloseKey( hKey );
    }
}

static void TraceDestroy()
{
    if( hLogFile!=INVALID_HANDLE_VALUE ) {
        CloseHandle(hLogFile);
        hLogFile=INVALID_HANDLE_VALUE;
    }
}

void _cdecl PgAtlTrace2(int category, UINT level, _Printf_format_string_ LPCTSTR lpszFormat, va_list args)
{
    if (hLogFile!=INVALID_HANDLE_VALUE && level <= ATL_TRACE_LEVEL)
    {
        int nBuf;
        TCHAR szBuffer[4096];//Must be longer than IDR_PG

        nBuf = _vsntprintf(szBuffer, sizeof(szBuffer)/sizeof(*szBuffer), lpszFormat, args);
        ATLASSERT(nBuf < sizeof(szBuffer)/sizeof(*szBuffer));

        DWORD nWritten;

        WriteFile(hLogFile, szBuffer, nBuf, &nWritten, NULL );
    }
}

void _cdecl PgAtlTrace2(int category, UINT level, _Printf_format_string_ LPCTSTR lpszFormat, ...)
{
    va_list args;
    va_start(args, lpszFormat);
    PgAtlTrace2(category, level, lpszFormat, args);
    va_end(args);
}

void _cdecl PgAtlTrace2(_Printf_format_string_ LPCTSTR lpszFormat, ...)
{
    va_list args;
    va_start(args, lpszFormat);
    PgAtlTrace2(0, 0, lpszFormat, args);
    va_end(args);
}
