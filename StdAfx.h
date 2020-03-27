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
#include <comdef.h>

// A few STL includes
#include <map>
#include <vector>

// Postgresql libpq includes
#include <libpq-fe.h>
// Allow the debugger to see into the conn abstraction
#ifdef _DEBUG
#include <libpq-int.h>
#endif

#include "Unicode.h"

// catalog/pg_type.h holds the type information

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CAB4C541_75BB_4579_9E2B_40F8DD61B3E7__INCLUDED)
