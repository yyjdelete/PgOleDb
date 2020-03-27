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

// PgRS.h : Declaration of the CPgRowset
#ifndef __CPgRowset_H_
#define __CPgRowset_H_
#include "PgCommand.h"

#include "typeinfo.h"

class CPgSession;

// CPgVirtualArray
class CPgVirtualArray
{
    PGresult *m_res;
    mutable BYTE *m_buff; // We allocate the buffer regardless of whether the class instance is a const
    size_t *m_offsets;
    mutable auto_array<size_t> m_sizes;
    CPgSession *m_session;

    void ClearInternal()
    {
        delete [] m_buff;
        m_buff=NULL;
        delete [] m_offsets;
        m_offsets=NULL;
    }
public:
    DBSTATUS GetDBStatus( CSimpleRow *pRow, ATLCOLUMNINFO *pColInfo );
    CPgVirtualArray() : m_res(NULL), m_buff(NULL), m_offsets(NULL), m_session(NULL)
    {
    }
    ~CPgVirtualArray();
    void AttachSess( CPgSession *session );
    void SetResult( PGresult *res )
    {
        RemoveAll();
        m_res=res;

        unsigned int numcols=PQnfields(m_res);
        m_offsets=new size_t[numcols];
        m_sizes=auto_array<size_t>(new size_t[numcols]);
    }
	void RemoveAll()
	{
        ClearInternal();
        if( m_res!=NULL )
            PQclear(m_res);
        m_res=NULL;
	}
    // Some versions of ATL use GetSize, others use GetCount
	int GetSize() const
	{
        return PQntuples(m_res);
	}
    int GetCount() const
    {
        return GetSize();
    }
    BYTE& operator[] (int nIndex) const;
    // Fill in the field offsets for the last retrieved row
    void FillDataOffsets( ATLCOLUMNINFO *&rpInfo )
    {
        int nfields=PQnfields(m_res);

        for( int i=0; i<nfields; ++i ) {
            rpInfo[i].cbOffset=m_offsets[i];
        }
    }
};

// CATLCOLUMNINFO
class CATLCOLUMNINFO : public ATLCOLUMNINFO {
    void copy( const ATLCOLUMNINFO &rhs ) {
        pwszName=new OLECHAR[lstrlenW(rhs.pwszName)+1];
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO copy 0x%p->0x%p\n", rhs.pwszName, pwszName );
        lstrcpyW(pwszName, rhs.pwszName);
    }
public:
    CATLCOLUMNINFO() {
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO() 0x%p\n", this);
        pwszName=NULL;
    }
    ~CATLCOLUMNINFO() {
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO delete 0x%p pwszName=0x%p\n", this,
            pwszName);
        delete [] pwszName;
    }
    CATLCOLUMNINFO( const CATLCOLUMNINFO &rhs ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO( const CATLCOLUMNINFO &rhs ) 0x%p\n", this);
       *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }
    CATLCOLUMNINFO( const ATLCOLUMNINFO &rhs ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO( const ATLCOLUMNINFO &rhs ) 0x%p\n", this);
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }

    CATLCOLUMNINFO &operator=( const ATLCOLUMNINFO &rhs ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO=( const ATLCOLUMNINFO &rhs ) 0x%p pwszName=0x%p\\n",
            this, pwszName);
        delete [] pwszName;
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }
    CATLCOLUMNINFO &operator=( const CATLCOLUMNINFO &rhs ) {
        ATLTRACE2(atlTraceDBProvider, 0, "CATLCOLUMNINFO=( const CATLCOLUMNINFO &rhs ) 0x%p\n", this);
        *this=static_cast<const ATLCOLUMNINFO &>(rhs);
    }
};

class CPgRowset :
    public CRowsetImpl< CPgRowset, CPgRemoteStorage, CPgCommand, CPgVirtualArray >,
    public IPgRowset
{
public:
    HRESULT Execute(DBPARAMS * pParams, LONG* pcRowsAffected);
    static ATLCOLUMNINFO* GetColumnInfo(CPgRowset *pv, ULONG *pcCols)
	{
        ATLCOLUMNINFO *ret;
        ret=CRowsetImpl< CPgRowset, CPgRemoteStorage, CPgCommand, CPgVirtualArray >::
            GetColumnInfo(pv,pcCols);
        pv->m_rgRowData.FillDataOffsets( ret );

        return ret;
	}
    virtual DBSTATUS GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pColInfo)
    {
        return m_rgRowData.GetDBStatus( pRow, pColInfo );
    }
BEGIN_COM_MAP(CPgRowset)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IRowsetInfo)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
	COM_INTERFACE_ENTRY(IRowsetIdentity)
	COM_INTERFACE_ENTRY(IRowset)
	COM_INTERFACE_ENTRY(IPgRowset)
END_COM_MAP()

    HRESULT PostConstruct( CPgSession *sess, PGresult *pRes );

	STDMETHOD(GetColumnInfo)(ULONG *pcColumns,
							 DBCOLUMNINFO **prgInfo,
							 OLECHAR **ppStringsBuffer);

    ~CPgRowset();
private:
    CSimpleArray<CATLCOLUMNINFO> m_colInfo;
    friend class CPgRemoteStorage;
};

#endif //__CPgRowset_H_
