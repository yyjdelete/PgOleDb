/*
 * This file is part of OleDb, an OLE DB provider for PostgreSQL
 * Copyright (C) 2004 Shachar Shemesh for Lingnu Open Systems Consulting (http://www.lignu.com)
 * Portions Copyright (c) 1996-2003, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
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

#include "stdafx.h"
#include "typeinfo.h"

#define POSTGRES_EPOCH_JDATE 2451545 // date2j(2000,1,1)

static inline void time_modulo( __int64 &t, int &q, const __int64 u )
{
    q=(t/u);
    if( q!=0 )
        t-= (q*u);
}

static void j2date(int jd, short *year, unsigned short *month, unsigned short *day)
{
	unsigned int julian;
	unsigned int quad;
	unsigned int extra;
	int			y;

	julian = jd;
	julian += 32044;
	quad = julian / 146097;
	extra = (julian - quad * 146097) * 4 + 3;
	julian += 60 + quad * 3 + extra / 146097;
	quad = julian / 1461;
	julian -= quad * 1461;
	y = julian * 4 / 1461;
	julian = ((y != 0) ? ((julian + 305) % 365) : ((julian + 306) % 366))
		+ 123;
	y += quad * 4;
	*year = y - 4800;
	quad = julian * 2141 / 65536;
	*day = julian - 7834 * quad / 256;
	*month = (quad + 10) % 12 + 1;

	return;
}	/* j2date() */

static void dt2time(__int64 jd, unsigned short *hour, unsigned short *min, unsigned short *sec,
                    ULONG *fsec)
{
	__int64		time;

	time = jd;

	*hour = (time / 3600000000i64);
	time -= ((*hour) * 3600000000i64);
	*min = (time / 60000000i64);
	time -= ((*min) * 60000000i64);
	*sec = (time / 1000000i64);
	*fsec = (time - (*sec * 1000000i64));

	return;
}	/* dt2time() */

int GetWidth_timestamp( const PGresult *res, int tup_num, int field_num )
{
    return sizeof( DBTIMESTAMP ); 
}

void COPY_timestamp( void *dst, size_t count, const PGresult *res,
                    int tup_num, int field_num)
{
    DBTIMESTAMP * const ts=reinterpret_cast<DBTIMESTAMP *>(dst);

    ATLASSERT( count==sizeof(DBTIMESTAMP) );

    // Adapted from the Postgresql "timestamp2tm" function
    int date, date0;
    __int64 time;
    const __int64 YEAR=86400000000i64;

    date0=POSTGRES_EPOCH_JDATE;

    typeinfo::StdC_ntoh_8( &time, sizeof(time), res, tup_num, field_num );
    time_modulo( time, date, YEAR );

    date+=date0;

    j2date( date, &(ts->year), &(ts->month), &(ts->day) );
    dt2time( time, &(ts->hour), &(ts->minute), &(ts->second), &(ts->fraction) );
}

void GetStatus_timestamp( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num)
{
    typeinfo::StdStat( _this, colinfo, res, field_num );

    colinfo->bScale=6; // 6 digits of scale
    colinfo->ulColumnSize=sizeof( DBTIMESTAMP );
}

int GetWidth_string( const PGresult *res, int tup_num, int field_num )
{
    // Assume all characters are going to turn into two
    return (PQgetlength( res, tup_num, field_num )+1)*2; 
}

void COPY_string( void *dst, size_t count, const PGresult *res,
                    int tup_num, int field_num)
{
    ATLASSERT(count>=GetWidth_string( res, tup_num, field_num ));
    int reslen=PQgetlength( res, tup_num, field_num );
    WCHAR *dst_str=reinterpret_cast<WCHAR *>(dst);

    const char *utf=PQgetvalue( res, tup_num, field_num );
    int newlen=MultiByteToWideChar( CP_UTF8, 0, utf, reslen,
        dst_str, count/sizeof(WCHAR) );

    if( newlen!=0 || reslen==0 ) {
        // Make sure we did not, in retrospect, overwrite the buffer.
        ATLASSERT( GetWidth_string( res, tup_num, field_num )>=(newlen+1)*2 );
        // Success
        dst_str[newlen]=L'\0';
    } else {
        // XXX What am I supposed to do if this fails??
        ATLASSERT(!"UTF8 -> UTF16 conversion failed" );
    }
}

struct numeric_transfer {
    signed short ndigits;
    signed short weight;
    signed short sign;
    signed short dscale;
};
// Number of decimal digits per unsigned short digit
#define DEC_DIGITS 4

template <int numshorts>
class bignum {
    unsigned short vals[numshorts];

public:
    bignum( unsigned long l=0 )
    {
        ZeroMemory( &(vals[0]), sizeof(vals) );
        vals[0]=l&0xffff;
        vals[1]=l>>16;
    }
    bignum &operator *=( unsigned short multi )
    {
        unsigned long carry=0;

        for( int i=0; i<numshorts; ++i ) {
            unsigned long res=vals[i];
            res*=multi;
            res+=carry;
            vals[i]=res&0xffff;
            carry=res>>16;
        }
        return *this;
    }
    bignum &operator +=( unsigned short added )
    {
        unsigned long carry=added;

        for( int i=0; carry!=0 && i<numshorts; ++i ) {
            carry+=vals[i];
            vals[i]=carry&0xffff;
            carry>>=16;
        }

        return *this;
    }

    void dump( BYTE *mem, size_t num ) {
        int i;
        for( i=0; i<num && i<(numshorts*2); ++i )
            mem[i]=reinterpret_cast<const char *>(vals)[i];

        while(i<num)
            mem[i++]=0;
    }
};

int GetWidth_numeric( const PGresult *res, int tup_num, int field_num )
{
    ATLTRACE2(atlTraceDBProvider, 0, "GetWidth_numeric var length %d %d %d\n",
        PQgetlength( res, tup_num, field_num), sizeof(numeric_transfer), sizeof( DB_VARNUMERIC) );
    return sizeof(DB_NUMERIC);
}

void COPY_numeric( void *dst, size_t count, const PGresult *res,
                    int tup_num, int field_num)
{
    numeric_transfer *orig=reinterpret_cast<numeric_transfer *>
        (PQgetvalue( res, tup_num, field_num ));
    DB_NUMERIC *dbnum=reinterpret_cast<DB_NUMERIC *>(dst);
    unsigned short *origval=reinterpret_cast<unsigned short *>
        (reinterpret_cast<char *>(orig)+sizeof(numeric_transfer));

    ZeroMemory( dbnum, count );
    dbnum->precision=DEC_DIGITS*ntohs(orig->ndigits);
    dbnum->scale=-(ntohs(orig->weight)-ntohs(orig->ndigits)+1)*DEC_DIGITS;
    dbnum->sign=!ntohs(orig->sign);

    int numdig=ntohs(orig->ndigits);
    bignum<8> accumolator=0;
    for(int i=0; i<numdig; ++i ) {
        accumolator*=10000; // 10^DEC_DIGITS
        accumolator+=ntohs(origval[i]);
    }

    accumolator.dump(dbnum->val, 15);
}

void GetStatus_numeric( const typeinfo *_this, ATLCOLUMNINFO *colinfo, PGresult *res,
        int field_num)
{
    typeinfo::StdStat( _this, colinfo, res, field_num );
    colinfo->bScale=0;
}
