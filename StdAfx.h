/*
 *   PostgreSQL OLE DB Provider 
 *   LGPL License  
 *   initial author Marek Mosiewicz (marekmosiewicz@poczta.onet.pl)
 *                  Jotel Poland 
 *
 *	Standard header files
 */
#if !defined(AFX_STDAFX_H__89D63496_AA36_420E_A2B5_7CFB76EDE80B__INCLUDED_)
#define AFX_STDAFX_H__89D63496_AA36_420E_A2B5_7CFB76EDE80B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <atldb.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__89D63496_AA36_420E_A2B5_7CFB76EDE80B__INCLUDED)
