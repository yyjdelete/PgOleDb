// PgMultipleResults.h: interface for the CPgMultipleResults class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PGMULTIPLERESULTS_H__DA00D01B_2DBF_4D87_9A42_F618A8EEF180__INCLUDED_)
#define AFX_PGMULTIPLERESULTS_H__DA00D01B_2DBF_4D87_9A42_F618A8EEF180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PgRS.h"

template <class T> class refcount {
    T *pT;
    void add()
    {
        if( pT!=NULL )
            pT->AddRef();
    }
    void rel()
    {
        if( pT!=NULL )
            pT->Release();
    }
public:
    refcount() : pT(NULL)
    {
        add();
    }
    refcount( T *src ) : pT(src)
    {
        add();
    }
    refcount( const refcount<T> &rhs ) : pT(rhs.pT)
    {
        add();
    }
    ~refcount()
    {
        rel();
    }
    operator T *() {
        return pT;
    }
    operator const T *() const {
        return pT;
    }
    refcount<T> & operator= (const refcount &rhs) {
        rel();
        pT=rhs.pT;
        add();
        
        return *this;
    }
    T * operator-> () {
        return pT;
    }
    const T * operator-> () const {
        return pT;
    }
};

class CPgMultipleResults :
    public CComObjectRoot,
    public IMultipleResults,
    public IObjectWithSiteImpl<CPgMultipleResults>
{
public:
BEGIN_COM_MAP(CPgMultipleResults)
    COM_INTERFACE_ENTRY(IMultipleResults)
    COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()
	CPgMultipleResults();
	virtual ~CPgMultipleResults();
    STDMETHOD(GetResult)(
       IUnknown     *pUnkOuter,
       LONG         lResultFlag,
       REFIID        riid,
       LONG   *pcRowsAffected,
       IUnknown    **ppRowset);
    void AddRowset( CPgRowset *rowset ) throw( PgOleError ) {
        m_rowsets.push_back(rowset);
    }
private:
    // Why using a typedef, it is ok to say "list<refcount<CPgRowset>>" but without it is not is beyond me.
    typedef refcount<CPgRowset> refrow;
    std::list<refrow> m_rowsets;
};

#endif // !defined(AFX_PGMULTIPLERESULTS_H__DA00D01B_2DBF_4D87_9A42_F618A8EEF180__INCLUDED_)
