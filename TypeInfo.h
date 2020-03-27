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
    typedef int (*WIDTHFUNC)(IPgSession *sess, const PGresult *res, int tup_num, int field_num);
    WIDTHFUNC GetWidth;

    typedef void (*COPYFUNC)(void *dst, size_t count, IPgSession *sess, const PGresult *res,
        int tup_num, int field_num);
    COPYFUNC CopyData;

    typedef void (*STATFUNC)(const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num);
    STATFUNC Status;

    typedef size_t (*PGWIDTHFUNC)(const typeinfo *_this, const void *data, size_t length,
        IPgSession *sess );
    PGWIDTHFUNC PGGetLength;
    
    typedef HRESULT (*PGCOPYFUNC)(const typeinfo *_this, const void *data, size_t length,
        void *dst, size_t dstlen, IPgSession *sess );
    PGCOPYFUNC PGCopyData;

    typeinfo() : alignment(0), wType(DBTYPE_EMPTY), bPrecision(~0), GetWidth(StdGWwidth),
        CopyData(StdC_memcpy), Status( StdStat ), PGGetLength(StdPGWidthInvalid),
        PGCopyData(StdPGC_memcpy)
    {
    }
    
    typeinfo( DBTYPE type, BYTE precise, COPYFUNC copyfunc=StdC_memcpy,
        WIDTHFUNC widthfunc=StdGWwidth, PGCOPYFUNC pgcopyfunc=StdPGC_memcpy,
        PGWIDTHFUNC pgwidthfunc=StdPGWidthInvalid, unsigned int align=0, STATFUNC statfunc=StdStat ) :
        alignment(align), wType(type), bPrecision(precise), GetWidth(widthfunc),
        CopyData(copyfunc), Status( statfunc ), PGGetLength(pgwidthfunc), PGCopyData(pgcopyfunc)
    {
    }
    
    // Standard callback functions - GetWidth
    static int StdGWwidth( IPgSession *sess, const PGresult *res, int tup_num, int field_num )
    {
        return PQgetlength(res, tup_num, field_num);
    }
    
    static int StdGWwidth_1(IPgSession *sess, const PGresult *res, int tup_num, int field_num)
    {
        // When one extra byte is needed
        return StdGWwidth( sess, res, tup_num, field_num )+1;
    }
    
    static int StdGW_1(IPgSession *, const PGresult *, int, int )
    {
        return 1;
    }
    
    static int StdGW_2(IPgSession *, const PGresult *, int, int )
    {
        return 2;
    }
    
    static int StdGW_4(IPgSession *, const PGresult *, int, int )
    {
        return 4;
    }
    
    static int StdGW_8(IPgSession *, const PGresult *, int, int )
    {
        return 8;
    }

    // Standard callback functions - Copy
    static void StdC_memcpy( void *dst, size_t count, IPgSession *sess, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count>=PQgetlength(res, tup_num, field_num));
        memcpy(dst, PQgetvalue( res, tup_num, field_num ), count);
    }

    static void StdC_ntoh_2( void *dst, size_t count, IPgSession *sess, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count==2);
        *reinterpret_cast<short *>(dst)=ntohs(
            *reinterpret_cast<short *>(PQgetvalue( res, tup_num, field_num )));
    }

    static void StdC_ntoh_4( void *dst, size_t count, IPgSession *sess, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count==4);
        *reinterpret_cast<int *>(dst)=ntohl(
            *reinterpret_cast<int *>(PQgetvalue( res, tup_num, field_num )));
    }

    static void StdC_ntoh_8( void *dst, size_t count, IPgSession *sess, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count==8);
        int *data=reinterpret_cast<int *>(PQgetvalue( res, tup_num, field_num ));

        // XXX - This code assumes the machine is little endian
        reinterpret_cast<int *>(dst)[0]=ntohl(data[1]);
        reinterpret_cast<int *>(dst)[1]=ntohl(data[0]);
    }

    static void StdC_nullterm( void *dst, size_t count, IPgSession *sess, const PGresult *res,
        int tup_num, int field_num)
    {
        ATLASSERT(count>=(PQgetlength(res, tup_num, field_num)+1));
        StdC_memcpy( dst, count, sess, res, tup_num, field_num );
        static_cast<char *>(dst)[PQgetlength(res, tup_num, field_num)]='\0';
    }

    // Standard callback functions - Status
    static void StdStat( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num)
    {
        StatUnknown( colinfo, res, field_num );
        colinfo->wType=_this->wType;
        colinfo->bPrecision=_this->bPrecision;
    }

    static void StatUnknown(ATLCOLUMNINFO *colinfo, PGresult *res, int field_num)
    {
        colinfo->pTypeInfo=NULL;
        colinfo->ulColumnSize=PQfsize( res, field_num );
        if( colinfo->ulColumnSize<0 )
            colinfo->ulColumnSize=~0;
        colinfo->dwFlags=DBCOLUMNFLAGS_MAYBENULL|
            (colinfo->ulColumnSize==~0?0:DBCOLUMNFLAGS_ISFIXEDLENGTH);
        colinfo->wType=DBTYPE_UDT; // User Defined Type
        colinfo->bPrecision=~0;
        colinfo->bScale=~0;
        colinfo->cbOffset=0;
        
        colinfo->columnid.eKind=DBKIND_PROPID;
        colinfo->columnid.uName.ulPropid=PQftablecol(res, field_num);        
    }

    static size_t StdPGWidthInvalid(const typeinfo *_this, const void *data, size_t length,
        IPgSession *sess )
    {
        ATLASSERT(!"Invalid width function");
        return 1;
    }
    static size_t StdPGWidth1(const typeinfo *_this, const void *data, size_t length,
        IPgSession *sess)
    {
        return 1;
    }
    static size_t StdPGWidth2(const typeinfo *_this, const void *data, size_t length,
        IPgSession *sess)
    {
        return 2;
    }
    static size_t StdPGWidth4(const typeinfo *_this, const void *data, size_t length,
        IPgSession *sess)
    {
        return 4;
    }
    static size_t StdPGWidth8(const typeinfo *_this, const void *data, size_t length,
        IPgSession *sess)
    {
        return 8;
    }

    static HRESULT StdPGC_memcpy(const typeinfo *_this, const void *data, size_t length,
        void *dst, size_t dstlen, IPgSession *sess )
    {
        ATLASSERT(length>=dstlen);

        memcpy(dst, data, length );

        return S_OK;
    }

    static HRESULT StdPGC_h2n_2(const typeinfo *_this, const void *data, size_t length,
        void *dst, size_t dstlen, IPgSession *sess )
    {
        ATLASSERT(length==2 && dstlen>=2);

        *reinterpret_cast<unsigned short *>(dst)=
            htons(*reinterpret_cast<const unsigned short *>(data));
        return S_OK;
    }
    static HRESULT StdPGC_h2n_4(const typeinfo *_this, const void *data, size_t length,
        void *dst, size_t dstlen, IPgSession *sess )
    {
        ATLASSERT(length==4 && dstlen>=4);

        *reinterpret_cast<unsigned int *>(dst)=
            htonl(*reinterpret_cast<const unsigned int *>(data));
        return S_OK;
    }
    static HRESULT StdPGC_h2n_8(const typeinfo *_this, const void *data, size_t length,
        void *dst, size_t dstlen, IPgSession *sess )
    {
        ATLASSERT(length==8 && dstlen>=8);

        // XXX - This assumes little endian machine!
        reinterpret_cast<unsigned int *>(dst)[0]=
            htonl(reinterpret_cast<const unsigned int *>(data)[1]);
        reinterpret_cast<unsigned int *>(dst)[1]=
            htonl(reinterpret_cast<const unsigned int *>(data)[0]);
        return S_OK;
    }
};

typedef std::map<unsigned int, typeinfo> types_type;

// Other types, not standard enough to be put inside the struct above
int GetWidth_timestamp( IPgSession *sess, const PGresult *res, int tup_num, int field_num );
void COPY_timestamp( void *dst, size_t count, IPgSession *sess, const PGresult *res,
                    int tup_num, int field_num);
void COPY_timestampTZ( void *dst, size_t count, IPgSession *sess, const PGresult *res,
                    int tup_num, int field_num);
void GetStatus_timestamp( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num);
int GetWidth_date( IPgSession *sess, const PGresult *res, int tup_num, int field_num );
void COPY_date( void *dst, size_t count, IPgSession *sess, const PGresult *res,
                    int tup_num, int field_num);
void GetStatus_date( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num);
int GetWidth_time( IPgSession *sess, const PGresult *res, int tup_num, int field_num );
void COPY_time( void *dst, size_t count, IPgSession *sess, const PGresult *res,
                    int tup_num, int field_num);
void GetStatus_time( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num);

int GetWidth_string( IPgSession *sess, const PGresult *res, int tup_num, int field_num );
void COPY_string( void *dst, size_t count, IPgSession *sess, const PGresult *res,
                    int tup_num, int field_num);
size_t PGWidthString(const typeinfo *_this, const void *data, size_t length, IPgSession *sess);
HRESULT PGC_string(const typeinfo *_this, const void *data, size_t length, void *dst,
                   size_t dstlen, IPgSession *sess );
int GetWidth_numeric( IPgSession *sess, const PGresult *res, int tup_num, int field_num );
void COPY_numeric( void *dst, size_t count, IPgSession *sess, const PGresult *res,
                    int tup_num, int field_num);
void GetStatus_numeric( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num);

void COPY_money( void *dst, size_t count, IPgSession *sess, const PGresult *res, int tup_num, int field_num );
void GetStatus_money( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num);
HRESULT PGC_money(const typeinfo *_this, const void *data, size_t length, void *dst,
                   size_t dstlen, IPgSession *sess );

#endif // __type_info_H_