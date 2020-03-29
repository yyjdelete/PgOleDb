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

/* Document the reasoning, so that we know what went wrong when the security problem comes.
 * UTF-8 encoded character can be 1, 2, 3 or 4 bytes long.
 * UTF-16 encoded character can be 1 or 2 WCHARs long.
 * lstrlenW counts the number of WCHARs in the string, not the number of characters.
 * A two WCHAR UTF-16 surrogate will always result in a 4 byte UTF-8 surrogate, and vice versa.
 *
 * As a result, if we count the number of WCHARS in the UTF-16 string (lstrlenW), and multiply
 * by 3, we will always have enough room for the new string.
 */
#ifndef UNICODE_H
#define UNICODE_H

#define W2U8(lpw) (\
    ((_lpw = lpw) == NULL) ? NULL : (\
    _convert = ((size_t)lstrlenW(_lpw)+1)*3,\
    ATLW2AHELPER((LPSTR) alloca(_convert), _lpw, _convert, CP_UTF8)))
#define U82W(lpa) (\
	((_lpa = lpa) == NULL) ? NULL : (\
		_convert = ((size_t)lstrlenA(_lpa)+1),\
		ATLA2WHELPER((LPWSTR) alloca(_convert*2), _lpa, _convert, CP_UTF8)))

#define W2CU8(lpw) ((LPCSTR)W2U8(lpw))
#define U82CW(lpa) ((LPCWSTR)U82W(lpa))

#define OLE2CU8 W2CU8
#define U82COLE U82CW

#endif // UNICODE_H