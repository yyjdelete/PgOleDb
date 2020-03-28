// PgDBSchemaRowsetImpl.h: interface for the CPgDBSchemaRowsetImpl class.
//
//////////////////////////////////////////////////////////////////////
/*
 * This file is part of OleDb, an OLE DB provider for PostgreSQL
 * Copyright (C) 2004 Shachar Shemesh for Lingnu Open Source Consulting (http://www.lignu.com)
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

#if !defined(AFX_PGDBSCHEMAROWSETIMPL_H__3FA31621_5E4C_40D0_8BC9_52AA89015914__INCLUDED_)
#define AFX_PGDBSCHEMAROWSETIMPL_H__3FA31621_5E4C_40D0_8BC9_52AA89015914__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PgRS.h"

template <class SessType>
class CPgDBSchemaRowsetImpl : public IDBSchemaRowset
{
    CComPtr<IPgCommand> cmd;

    HRESULT InitCmd()
    {
        if( cmd==NULL )
            return static_cast<SessType *>(this)->CreateCommand(NULL, IID_IPgCommand,
                reinterpret_cast<IUnknown **>(&cmd));

        return S_OK;
    }
protected:
    // Define the structure in which this class' son will define the actual schemas reported.
#define MAX_SCHEMA_CONSTRAINTS 7
    struct PGSCHEMA_INFO {
        GUID guid;
        const WCHAR *basic_query; // Basic SQL query
        struct { DBTYPE type; const WCHAR *column; } constraints[MAX_SCHEMA_CONSTRAINTS];
        unsigned int num_constraints;

        PGSCHEMA_INFO( const GUID &_guid, const WCHAR *_query ) : guid(_guid), basic_query(_query),
            num_constraints(0)
        {
            ZeroFillArray();
        }
        PGSCHEMA_INFO( const GUID &_guid, const WCHAR *_query, DBTYPE type1, const WCHAR *col1 ) :
            guid(_guid), basic_query(_query), num_constraints(1)
        {
            constraints[0].type=type1;
            constraints[0].column=col1;
            ZeroFillArray();
        }
        PGSCHEMA_INFO( const GUID &_guid, const WCHAR *_query, DBTYPE type1, const WCHAR *col1,
            DBTYPE type2, const WCHAR *col2) :
            guid(_guid), basic_query(_query), num_constraints(2)
        {
            constraints[0].type=type1;
            constraints[0].column=col1;
            constraints[1].type=type2;
            constraints[1].column=col2;
            ZeroFillArray();
        }
        PGSCHEMA_INFO( const GUID &_guid, const WCHAR *_query, DBTYPE type1, const WCHAR *col1,
            DBTYPE type2, const WCHAR *col2, DBTYPE type3, const WCHAR *col3) :
            guid(_guid), basic_query(_query), num_constraints(3)
        {
            constraints[0].type=type1;
            constraints[0].column=col1;
            constraints[1].type=type2;
            constraints[1].column=col2;
            constraints[2].type=type3;
            constraints[2].column=col3;
            ZeroFillArray();
        }
        PGSCHEMA_INFO( const GUID &_guid, const WCHAR *_query, DBTYPE type1, const WCHAR *col1,
            DBTYPE type2, const WCHAR *col2, DBTYPE type3, const WCHAR *col3,
            DBTYPE type4, const WCHAR *col4) :
            guid(_guid), basic_query(_query), num_constraints(4)
        {
            constraints[0].type=type1;
            constraints[0].column=col1;
            constraints[1].type=type2;
            constraints[1].column=col2;
            constraints[2].type=type3;
            constraints[2].column=col3;
            constraints[3].type=type4;
            constraints[3].column=col4;
            ZeroFillArray();
        }
        void ZeroFillArray( )
        {
            for( unsigned int i=num_constraints; i<MAX_SCHEMA_CONSTRAINTS; ++i ) {
                constraints[i].type=DBTYPE_EMPTY;
                constraints[i].column=NULL;
            }
        }
    };
private:
    const PGSCHEMA_INFO *FindSchema( const GUID &guid ) const
    {
        const SessType * const session=static_cast<const SessType *>(this);
        int i=0;
        const int numschemas=sizeof(session->s_schema_queries)/sizeof(*session->s_schema_queries);

        for( i=0; i<numschemas ; ++i )
            if( session->s_schema_queries[i].guid==guid )
                return &session->s_schema_queries[i];

        return NULL;
    }
public:
    CPgDBSchemaRowsetImpl() {}
    virtual ~CPgDBSchemaRowsetImpl() {}
    virtual HRESULT STDMETHODCALLTYPE GetRowset( 
        /* [in] */ IUnknown __RPC_FAR *pUnkOuter,
        /* [in] */ REFGUID rguidSchema,
        /* [in] */ ULONG cRestrictions,
        /* [size_is][in] */ const VARIANT __RPC_FAR rgRestrictions[  ],
        /* [in] */ REFIID riid,
        /* [in] */ ULONG cPropertySets,
        /* [size_is][unique][out][in] */ DBPROPSET __RPC_FAR rgPropertySets[  ],
        /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppRowset)
    {
        CErrorLookupService::ClearError();

        ATLTRACE2(atlTraceDBProvider, 0, "CPgDBSchemaRowsetImpl::GetRowset\n");
        HRESULT hr=InitCmd();
        if( FAILED(hr) )
            return hr;
        CPgCommand *pgCommand=static_cast<CPgCommand *>(static_cast<IPgCommand *>(cmd));

        const PGSCHEMA_INFO *schema=FindSchema(rguidSchema);
        if( schema==NULL || ::InlineIsEqualGUID(rguidSchema, IID_NULL) ) {
            // We don't support the requested interface
            CErrorLookupService::ReportCustomError("GetRowset: Requested schema not supported", E_INVALIDARG, IID_IDBSchemaRowset);
            return E_INVALIDARG;
        }

        // Prepare the "where" clause parameters
        if( cRestrictions>schema->num_constraints || (cRestrictions>0 && rgRestrictions==NULL))
        {
            CErrorLookupService::ReportCustomError("GetRowset: Number of arguments passed invalid", E_INVALIDARG, IID_IDBSchemaRowset);
            return E_INVALIDARG;
        }

        auto_array<DBPARAMBINDINFO> prminfo(new DBPARAMBINDINFO[cRestrictions]);
        auto_array<DB_UPARAMS> uparams(new DB_UPARAMS[cRestrictions]);

        // Create the execution parameters
        auto_array<DBBINDING> bindings(new DBBINDING[cRestrictions]);
        auto_array<unsigned char> params_data(new unsigned char[cRestrictions*(
            sizeof(VARIANT)+sizeof(DWORD)+sizeof(ULONG))]);
        ULONG *params_length=reinterpret_cast<ULONG *>(params_data.get());
        DWORD *params_status=reinterpret_cast<DWORD *>(params_length+cRestrictions);
        VARIANT *params_vars=reinterpret_cast<VARIANT *>(params_status+cRestrictions);
#define CALC_OFFSET(a, b) (reinterpret_cast<const unsigned char *>(a)-(b))

        int max_actual_restriction=0; // Maximal number of actual (non-empty) restriction

        for( unsigned int i=0; i<cRestrictions; ++i ) {
            if( rgRestrictions[i].vt!=VT_EMPTY ) {
                uparams[max_actual_restriction]=max_actual_restriction+1;
                
                bindings[max_actual_restriction].iOrdinal=max_actual_restriction+1;
                bindings[max_actual_restriction].obValue=CALC_OFFSET(params_vars+max_actual_restriction, params_data.get());
                bindings[max_actual_restriction].obLength=CALC_OFFSET(params_length+max_actual_restriction, params_data.get());
                bindings[max_actual_restriction].obStatus=CALC_OFFSET(params_status+max_actual_restriction, params_data.get());
                bindings[max_actual_restriction].pTypeInfo=NULL;
                bindings[max_actual_restriction].pObject=NULL;
                bindings[max_actual_restriction].pBindExt=NULL;
                bindings[max_actual_restriction].dwMemOwner=DBMEMOWNER_CLIENTOWNED;
                bindings[max_actual_restriction].eParamIO=DBPARAMIO_INPUT;
                bindings[max_actual_restriction].cbMaxLen=0;
                bindings[max_actual_restriction].dwFlags=0;
                bindings[max_actual_restriction].bPrecision=0;
                bindings[max_actual_restriction].bScale=0;
                
                params_length[max_actual_restriction]=sizeof(VARIANT);
                params_vars[max_actual_restriction]=rgRestrictions[i];
                
                // Set the parameter info
                prminfo[max_actual_restriction].pwszName=NULL;
                prminfo[max_actual_restriction].dwFlags=DBPARAMFLAGS_ISNULLABLE|DBPARAMFLAGS_ISINPUT;

                bindings[max_actual_restriction].dwPart=DBPART_VALUE|DBPART_LENGTH|DBPART_STATUS;
                bindings[max_actual_restriction].wType=DBTYPE_VARIANT;

                params_status[max_actual_restriction]=DBSTATUS_S_OK;

                switch( schema->constraints[i].type ) {
                case DBTYPE_WSTR:
                    prminfo[max_actual_restriction].pwszDataSourceType=L"DBTYPE_WCHAR";
                    prminfo[max_actual_restriction].ulParamSize=~0;
                    break;
                case DBTYPE_I4:
                    prminfo[max_actual_restriction].pwszDataSourceType=L"DBTYPE_I4";
                    prminfo[max_actual_restriction].ulParamSize=4;
                    prminfo[max_actual_restriction].bPrecision=10;
                    prminfo[max_actual_restriction].bScale=0;
                    break;
                default:
                    ATLASSERT(!"Internal error - unknown data type in constraint!");
                    return E_INVALIDARG;
                }
                max_actual_restriction++;
            }
        }

        // Create the new command's text
        _bstr_t command_text;
        while( schema->guid==rguidSchema )
        {
            if( command_text.length()!=0 ) {
                command_text+=" UNION ALL ";
            }

            command_text+=schema->basic_query;

            _bstr_t where_sql;
            // Prepare the actual where clause
            if( cRestrictions>schema->num_constraints ) {
                CErrorLookupService::ReportCustomError("GetRowset: Internal schema parameter inconsistency",
                    E_UNEXPECTED, IID_IDBSchemaRowset);
                return E_UNEXPECTED;
            }

            max_actual_restriction=0;
            for( unsigned int i=0; i<cRestrictions; ++i ) {
                if( rgRestrictions[i].vt!=VT_EMPTY ) {
                    if( where_sql.length()==0 )
                        where_sql=" WHERE ";
                    else
                        where_sql+=" AND ";

                    if( schema->constraints[i].column!=NULL ) {
                        where_sql+=schema->constraints[i].column;
                        where_sql+="=$";
                        char buffer[34];
                        where_sql+=itoa(++max_actual_restriction, buffer, 10);
                    } else {
                        where_sql+="$";
                        char buffer[34];
                        where_sql+=itoa(++max_actual_restriction, buffer, 10);
                        where_sql+=" IS NULL";
                    }
                }
            }

            if( where_sql.length()!=0 ) {
                command_text+=where_sql;
            }

            ++schema;
        }

        // See if we need to add "order by" to the command's end
        if( schema->guid==IID_NULL ) {
            command_text+=" ORDER BY ";
            command_text+=schema->basic_query;
        }

        hr=pgCommand->PgSetCommandText(DBGUID_DEFAULT, command_text);
        if( FAILED(hr) ) {
            CErrorLookupService::ReportCustomError("Error setting the schema query command text",
                hr, IID_IDBSchemaRowset );
            return hr;
        }
        OutputDebugString( command_text );

        hr=pgCommand->SetParameterInfo( max_actual_restriction, uparams.get(), prminfo.get() );
        if( FAILED(hr) ) {
            CErrorLookupService::ReportCustomError("Error setting the schema query parameters",
                hr, IID_IDBSchemaRowset );
            return hr;
        }

        if( max_actual_restriction>0 ) {
            // Prepare the parameters for passing to Execute
            DBPARAMS dbparams;
            dbparams.pData=params_data.get();
            dbparams.cParamSets=1;
            hr=pgCommand->CreateAccessor(DBACCESSOR_PARAMETERDATA, max_actual_restriction,
                bindings.get(), 0, &dbparams.hAccessor, NULL );

            if( FAILED(hr) ) {
                CErrorLookupService::ReportCustomError("Could not create accessor for schema filters",
                    hr, IID_IDBSchemaRowset );
                return hr;
            }

            hr=pgCommand->Execute(pUnkOuter, riid, &dbparams, NULL, ppRowset);
            pgCommand->ReleaseAccessor(dbparams.hAccessor, NULL );
        } else {
            hr=pgCommand->Execute(pUnkOuter, riid, NULL, NULL, ppRowset);
        }
        if( FAILED(hr) ) {
            CErrorLookupService::ReportCustomError("Executing schema query failed", hr,
                IID_IDBSchemaRowset );
        }

        return hr;
    }
    
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetSchemas( 
        /* [out][in] */ ULONG __RPC_FAR *pcSchemas,
        /* [size_is][size_is][out] */ GUID __RPC_FAR *__RPC_FAR *prgSchemas,
        /* [size_is][size_is][out] */ ULONG __RPC_FAR *__RPC_FAR *prgRestrictionSupport)
    {
        // XXX Untested code, but I can't seem to find the client code that reaches here
        CErrorLookupService::ClearError();
        HRESULT hr=S_OK;

        try {
            if( pcSchemas==NULL || prgSchemas==NULL ) {
                CErrorLookupService::ReportCustomError("NULL argument passed", E_INVALIDARG,
                    IID_IDBSchemaRowset );
                return E_INVALIDARG; // Error handling assumes the prgSchemas is non-NULL, so don't use it here.
            }

            *pcSchemas=0;
            *prgSchemas=NULL;
            if( prgRestrictionSupport )
                *prgRestrictionSupport=NULL;

            const SessType * const session=static_cast<const SessType *>(this);
            const int numschemas=sizeof(session->s_schema_queries)/sizeof(*session->s_schema_queries);

            int i;
            // Count how many distinct schemas we have
            for( i=0; i<numschemas; ++i ) {
                if( session->s_schema_queries[i].guid==IID_NULL )
                    (*pcSchemas)++;
            }

            *prgSchemas=static_cast<GUID *>(CoTaskMemAlloc( (*pcSchemas)*sizeof(GUID) ));
            if( *prgSchemas==NULL )
                throw PgOleError( E_OUTOFMEMORY );
            if( prgRestrictionSupport!=NULL ) {
                *prgRestrictionSupport=static_cast<ULONG *>
                    (CoTaskMemAlloc( (*pcSchemas)*sizeof(ULONG) ));

                if( *prgRestrictionSupport!=NULL )
                    throw PgOleError( E_OUTOFMEMORY );
            }

            int schema_index(0);
            GUID last_guid(IID_NULL);
            if( prgRestrictionSupport!=NULL ) {
                prgRestrictionSupport[schema_index]=0;
            }

            for( i=0; i<numschemas; ++i ) {
                const PGSCHEMA_INFO * const schema=session->s_schema_queries+i;
                if( schema->guid!=IID_NULL ) {
                    (*prgSchemas)[schema_index]=schema->guid;

                    if( prgRestrictionSupport!=NULL ) {
                        ULONG filter_mask=0;

                        for( unsigned int j=0; j<schema->num_constraints; ++j ) {
                            if( schema->constraints[j].column!=NULL )
                                filter_mask|=1ul<<j;
                        }

                        (*prgRestrictionSupport)[schema_index]|=filter_mask;
                    }
                } else {
                    schema_index++;
                    if( prgRestrictionSupport!=NULL )
                        (*prgRestrictionSupport)[schema_index]=0;
                }

                last_guid=schema->guid;
            }
        } catch( const PgOleError &err ) {
            hr=err.hr();
            ATLTRACE2(atlTraceDBProvider, 0, "GetSchemas: %s\n", err.str());
            *pcSchemas=0;
            CoTaskMemFree( *prgSchemas );
            *prgSchemas=NULL;
            if( prgRestrictionSupport!=NULL ) {
                CoTaskMemFree( *prgRestrictionSupport );
                *prgRestrictionSupport=NULL;
            }
            CErrorLookupService::ReportCustomError(err.str(), hr, IID_IDBSchemaRowset );
        }

        return hr;
    }

};

#endif // !defined(AFX_PGDBSCHEMAROWSETIMPL_H__3FA31621_5E4C_40D0_8BC9_52AA89015914__INCLUDED_)
