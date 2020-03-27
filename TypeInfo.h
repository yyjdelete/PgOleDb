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

#ifndef __type_info_H_
#define __type_info_H_

struct typeinfo {
    unsigned int alignment;
    DBTYPE wType;
    BYTE bPrecision;
    // Callback functions
    typedef int (*WIDTHFUNC)(const PGresult *res, int tup_num, int field_num);
    WIDTHFUNC GetWidth;

    typedef void (*COPYFUNC)(void *dst, size_t count, const PGresult *res,
        int tup_num, int field_num);
    COPYFUNC CopyData;
    
    typeinfo() : alignment(0), wType(DBTYPE_EMPTY), bPrecision(~0), GetWidth(StdGWwidth),
        CopyData(StdC_memcpy)
    {
    }
    
    typeinfo( DBTYPE type, BYTE precise, COPYFUNC copyfunc=StdC_memcpy,
        WIDTHFUNC widthfunc=StdGWwidth ) :
        alignment(1), wType(type), bPrecision(precise), GetWidth(widthfunc), CopyData(copyfunc)
    {
    }
    
    // Standard callback functions - GetWidth
    static int StdGWwidth(const PGresult *res, int tup_num, int field_num)
    {
        return PQgetlength(res, tup_num, field_num);
    }
    
    static int StdGWwidth_1(const PGresult *res, int tup_num, int field_num)
    {
        // When one extra byte is needed
        return StdGWwidth( res, tup_num, field_num )+1;
    }
    
    static int StdGW_1(const PGresult, int, int )
    {
        return 1;
    }
    
    static int StdGW_2(const PGresult, int, int )
    {
        return 2;
    }
    
    static int StdGW_4(const PGresult, int, int )
    {
        return 4;
    }
    
    static int StdGW_8(const PGresult, int, int )
    {
        return 8;
    }

    // Standard callback functions - Copy
    static void StdC_memcpy( void *dst, size_t count, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count<=PQgetlength(res, tup_num, field_num));
        memcpy(dst, PQgetvalue( res, tup_num, field_num ), count);
    }

    static void StdC_ntoh_2( void *dst, size_t count, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count==2);
        *reinterpret_cast<int2 *>(dst)=ntohs(
            *reinterpret_cast<int2 *>(PQgetvalue( res, tup_num, field_num )));
    }

    static void StdC_ntoh_4( void *dst, size_t count, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count==4);
        *reinterpret_cast<int4 *>(dst)=ntohl(
            *reinterpret_cast<int4 *>(PQgetvalue( res, tup_num, field_num )));
    }

    static void StdC_ntoh_8( void *dst, size_t count, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count==8);
        int4 *data=reinterpret_cast<int4 *>(PQgetvalue( res, tup_num, field_num ));

        // XXX - This code assumes the machine is little endian
        reinterpret_cast<int4 *>(dst)[0]=ntohl(data[1]);
        reinterpret_cast<int4 *>(dst)[1]=ntohl(data[0]);
    }

    static void StdC_nullterm( void *dst, size_t count, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count<=(PQgetlength(res, tup_num, field_num)+1));
        StdC_memcpy( dst, count, res, tup_num, field_num );
        static_cast<char *>(dst)[PQgetlength(res, tup_num, field_num)]='\0';
    }
};

typedef std::map<unsigned int, typeinfo> types_type;

#endif // __type_info_H_