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
#include "resource.h"       // main symbols

#include "typeinfo.h"

// CPgRemoteStorage
class CPgRemoteStorage  
{
public:
    template <class T>
    static ATLCOLUMNINFO* GetColumnInfo( T *pT, ULONG* pcCols )
    {
        if( pcCols!=NULL )
            *pcCols=pT->m_colInfo.GetSize();

        return pT->m_colInfo.GetData();
    }
};

class CPgSession;

// CPgVirtualArray
class CPgVirtualArray
{
    PGresult *m_res;
    mutable BYTE *m_buff; // We allocate the buffer regardless of whether the class instance is a const
    size_t *m_offsets;
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
    void AttachSess( CPgSession *session )
    {
        m_session=session;
    }
    void SetResult( PGresult *res )
    {
        RemoveAll();
        m_res=res;

        m_offsets=new size_t[PQnfields(m_res)];
    }
	void RemoveAll()
	{
        ClearInternal();
        m_res=NULL;
	}
	int GetSize() const
	{
        return PQntuples(m_res);
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
        lstrcpyW(pwszName, rhs.pwszName);
    }
public:
    CATLCOLUMNINFO() {
        pwszName=NULL;
    }
    ~CATLCOLUMNINFO() {
        delete [] pwszName;
    }
    CATLCOLUMNINFO( const CATLCOLUMNINFO &rhs ) {
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }
    CATLCOLUMNINFO( const ATLCOLUMNINFO &rhs ) {
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }

    CATLCOLUMNINFO &operator=( const ATLCOLUMNINFO &rhs ) {
        delete [] pwszName;
        *static_cast<ATLCOLUMNINFO *>(this)=rhs;
        copy(rhs);
    }
    CATLCOLUMNINFO &operator=( const CATLCOLUMNINFO &rhs ) {
        *this=static_cast<const ATLCOLUMNINFO &>(rhs);
    }
};

class CPgRowset;

// CPgCommand
class ATL_NO_VTABLE CPgCommand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IAccessorImpl<CPgCommand>,
	public ICommandTextImpl<CPgCommand>,
	public ICommandPropertiesImpl<CPgCommand>,
	public IObjectWithSiteImpl<CPgCommand>,
	public IConvertTypeImpl<CPgCommand>,
	public IColumnsInfoImpl<CPgCommand>
//	public ICommandWithPamametersImpl<CPgCommand>
{
public:
BEGIN_COM_MAP(CPgCommand)
	COM_INTERFACE_ENTRY(ICommand)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(ICommandProperties)
	COM_INTERFACE_ENTRY2(ICommandText, ICommand)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
//	COM_INTERFACE_ENTRY(ICommandWithParameters)
END_COM_MAP()
// ICommand
public:
    CPgCommand() : m_queryRes(NULL)
    {
    }
	HRESULT FinalConstruct()
	{
		HRESULT hr = CConvertHelper::FinalConstruct();
		if (FAILED (hr))
			return hr;
		hr = IAccessorImpl<CPgCommand>::FinalConstruct();
		if (FAILED(hr))
			return hr;
		return CUtlProps<CPgCommand>::FInit();
	}
	void FinalRelease();
	HRESULT WINAPI Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
						  LONG * pcRowsAffected, IUnknown ** ppRowset);
	static ATLCOLUMNINFO* GetColumnInfo(CPgCommand* pv, ULONG* pcInfo)
	{
        return CPgRemoteStorage::GetColumnInfo(pv,pcInfo);
	}
    STDMETHOD(SetCommandText)(REFGUID rguidDialect,LPCOLESTR pwszCommand)
	{
		ATLTRACE2(atlTraceDBProvider, 0, "CPgCommand::SetCommandText\n");
        Lock();
        HRESULT hr=ICommandTextImpl<CPgCommand>::SetCommandText(rguidDialect, pwszCommand);
        m_colInfo.RemoveAll();
        Unlock();

        return hr;
    }
BEGIN_PROPSET_MAP(CPgCommand)
	BEGIN_PROPERTY_SET(DBPROPSET_ROWSET)
		PROPERTY_INFO_ENTRY(IAccessor)
		PROPERTY_INFO_ENTRY(IColumnsInfo)
		PROPERTY_INFO_ENTRY(IConvertType)
		PROPERTY_INFO_ENTRY(IRowset)
		PROPERTY_INFO_ENTRY(IRowsetIdentity)
		PROPERTY_INFO_ENTRY(IRowsetInfo)
		PROPERTY_INFO_ENTRY(IRowsetLocate)
		PROPERTY_INFO_ENTRY(BOOKMARKS)
		PROPERTY_INFO_ENTRY(BOOKMARKSKIPPED)
		PROPERTY_INFO_ENTRY(BOOKMARKTYPE)
		PROPERTY_INFO_ENTRY(CANFETCHBACKWARDS)
		PROPERTY_INFO_ENTRY(CANHOLDROWS)
		PROPERTY_INFO_ENTRY(CANSCROLLBACKWARDS)
		PROPERTY_INFO_ENTRY(LITERALBOOKMARKS)
		PROPERTY_INFO_ENTRY(ORDEREDBOOKMARKS)
	END_PROPERTY_SET(DBPROPSET_ROWSET)
END_PROPSET_MAP()
private:
	PGresult *m_queryRes;  // Query results
    CSimpleArray<CATLCOLUMNINFO> m_colInfo;
    HRESULT CreateRowset(IUnknown* pUnkOuter, REFIID riid,
        DBPARAMS * pParams, LONG * pcRowsAffected,
        IUnknown** ppRowset,
        CPgRowset*& pRowsetObj);
    friend class CPgRemoteStorage;
};

class CPgRowset : public CRowsetImpl< CPgRowset, CPgRemoteStorage, CPgCommand, CPgVirtualArray >,
    public IPgRowset
{
public:
    HRESULT Execute(DBPARAMS * pParams, LONG* pcRowsAffected);
    void SetColInfo( const CSimpleArray<CATLCOLUMNINFO> &info ) {
        /* RANT - You would expect a well constructed class to either support deep copy
         * via m_colInfo=info, or to disable the copy constructor. Anyone who has ever
         * read Scott Meyers' excellent "Effective C++" should know that. I guess someone at
         * MS didn't bother with this one. *sigh*.
         */
        // Traversing backwards results in a single allocation, and no copy
        for( int i=0; i<info.GetSize(); ++i ) {
            m_colInfo.Add(info[i]);
        }
    }
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

private:
    CSimpleArray<ATLCOLUMNINFO> m_colInfo;
    friend class CPgRemoteStorage;
};

#endif //__CPgRowset_H_
