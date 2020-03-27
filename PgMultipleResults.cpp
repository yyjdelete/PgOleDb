// PgMultipleResults.cpp: implementation of the CPgMultipleResults class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "oledb.h"
#include "PgMultipleResults.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPgMultipleResults::CPgMultipleResults()
{

}

CPgMultipleResults::~CPgMultipleResults()
{
}

HRESULT CPgMultipleResults::GetResult(
                                      IUnknown     *pUnkOuter,
                                      LONG         lResultFlag,
                                      REFIID        riid,
                                      LONG   *pcRowsAffected,
                                      IUnknown    **ppRowset)
{
    HRESULT hr=S_OK;

    try {
        std::list<refrow>::iterator head=m_rowsets.begin();
        
        if( head==m_rowsets.end() ) {
            ATLTRACE2(atlTraceDBProvider, 0, "CPgMultipleResults::GetResult no more rowsets\n");

            if( ppRowset!=NULL )
                *ppRowset=NULL;
            if( pcRowsAffected!=NULL )
                *pcRowsAffected=-1;

            return DB_S_NORESULT;
        }

#if 0
        // It would appear that I have been looking in too recent a version of the docs.
        // The VC6 SDK still regards lResultFlag as reserved, and DBRESULTFLAG_* are not
        // defined.
        // Disable this too-advanced flag.
        if( lResultFlag!=DBRESULTFLAG_DEFAULT && lResultFlag!=DBRESULTFLAG_ROWSET )
            throw PgOleError( E_NOINTERFACE, "lResultFlag unsupported" );
#endif

        if( pcRowsAffected!=NULL )
            // We don't support indevidual rows count
            *pcRowsAffected=DB_COUNTUNAVAILABLE;

        // Get the next record, and remove it from the list
        refrow curr_row=*head;
        m_rowsets.pop_front();

        if( riid==IID_NULL ) {
            // No results were asked for.
            return S_OK;
        }

        hr=curr_row->QueryInterface( riid, reinterpret_cast<void **>(ppRowset) );
        if( FAILED(hr) )
            throw PgOleError( hr, "Couldn't get requested interface" );

    } catch( const PgOleError &err ) {
        hr=err.hr();
        ATLTRACE2(atlTraceDBProvider, 0, "CPgMultipleResults: %s\n", err.str());
        if( ppRowset!=NULL )
            *ppRowset=NULL;
    }

    return hr;
}
