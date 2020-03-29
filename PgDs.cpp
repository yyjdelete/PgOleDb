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

// Implementation of the CPgSource
#include "stdafx.h"
#include "OleDb.h"
#include "PgDS.h"

/* A Macro for inlining GUIDs into static struct initializations */
#define GUID_EMBED(g)  g.Data1, g.Data2, g.Data3, \
    g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], \
    g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]

/////////////////////////////////////////////////////////////////////////////
// CPgSource
HRESULT CPgSource::RefreshConnectString()
{
    static const DBPROPID properties[]={
        DBPROP_AUTH_PASSWORD, // password=
        DBPROP_AUTH_USERID, // user=
        DBPROP_INIT_DATASOURCE, // host=
        DBPROP_INIT_LOCATION, // dbname=
        DBPROP_INIT_PROVIDERSTRING, // options=
        DBPROP_INIT_TIMEOUT // connect_timeout=
    };
    static const size_t NUMPROPERTIES=sizeof(properties)/
        sizeof(properties[0]);
    static const DBPROPIDSET propset[1]={ const_cast<DBPROPID*>(properties),
        NUMPROPERTIES, GUID_EMBED(DBPROPSET_DBINIT)
    };
    HRESULT hr;
    ULONG numproperties=0;
    DBPROPSET *results;

    m_strConnect=OLESTR("");
    
    hr=GetProperties(1, propset, &numproperties, &results );

    if( SUCCEEDED(hr) ) {
        for( ULONG i=0; i<results[0].cProperties; ++i ) {
            bool bParam=true;

            switch( results[0].rgProperties[i].dwPropertyID ) {
            case DBPROP_AUTH_PASSWORD:
                m_strConnect+=OLESTR("password=");
                break;
            case DBPROP_AUTH_USERID:
                m_strConnect+=OLESTR("user=");
                break;
            case DBPROP_INIT_DATASOURCE:
                m_strConnect+=OLESTR("host=");
                break;
            case DBPROP_INIT_LOCATION:
                m_strConnect+=OLESTR("dbname=");
                break;
            case DBPROP_INIT_PROVIDERSTRING:
                m_strConnect+=OLESTR("options=");
                break;
            case DBPROP_INIT_TIMEOUT:
                m_strConnect+=OLESTR("connect_timeout=");
                break;
            default:
                ATLASSERT(FALSE);
                bParam=false;
                break;
            }

            if( bParam ) {
                _bstr_t param;

                FormatParameter( param, results[0].rgProperties[i].vValue );
                m_strConnect+=param;
                m_strConnect+=OLESTR(" ");
            }
        }
        //For 9.1+, always use UTF8, even for login failed message
        m_strConnect+=OLESTR("client_encoding=\'UNICODE\' ");

        // Clear the memory
        ReleaseProperties( results, numproperties );
    }
    return hr;
}

// Format a value for inclusion in the connection string
void CPgSource::FormatParameter( _bstr_t &param, const VARIANT &val )
{
    param=OLESTR("");

    switch( val.vt ) {
    case VT_BSTR:
        {
            OLECHAR tmpchr[2];

            tmpchr[1]=OLESTR('\0');
            param+=OLESTR("\'");

            for( const OLECHAR *ch=val.bstrVal; *ch!=OLESTR('\0'); ch++ ) {
                if( *ch==OLESTR('\'') || *ch==OLESTR('\\') ) {
                    param+=OLESTR("\\");
                }

                tmpchr[0]=*ch;
                param+=tmpchr;
            }

            param+=OLESTR("\'");
        }
        break;
    case VT_I4:
        param=val;
        break;
    default:
        ATLASSERT(FALSE);
        break;
    }
}

HRESULT CPgSource::CreateSession(IUnknown *pUnkOuter,
                                 REFIID riid,
                                 IUnknown **ppDBSession)
{
    HRESULT hr=IDBCreateSessionImpl<CPgSource, CPgSession>::
        CreateSession( pUnkOuter, riid, ppDBSession );
    
    if(SUCCEEDED(hr)) {
        IPgSession *object;

        hr=(*ppDBSession)->QueryInterface(&object);
        if(SUCCEEDED(hr)) {
            if( m_strConnect.length()==0 )
                // string cache is dirty
                hr=RefreshConnectString();

            hr=object->PgConnectDB(m_strConnect);
            object->Release();
        } else {
            (*ppDBSession)->Release();
        }
    }
    
    return hr;
}

//Not used
_bstr_t CPgSource::EscapeString( PGconn* conn, const WCHAR *str ) // Escape a string inclusion in a string
{
    USES_CONVERSION;
    _bstr_t ret;
    size_t strl=wcslen(str);
    auto_array<char> to(new char[strl*4+1]);
    const char *src_str=W2U8(str);
    int error;
    PQescapeStringConn(conn, to.get(), src_str, strlen(src_str), &error);

    ret=U82W(to.get());

    return ret;
}

_bstr_t CPgSource::EscapeID( const WCHAR *str ) // Escape an identifier
{
    _bstr_t ret;

    // Doesn't need quote if all characters are lowercase ASCII english, digits or '_'.
    // Will also need quote if first character is a digit
    bool need_quote=(iswdigit(str[0])!=false);

    // Use array to construct NULL terminated string
    WCHAR charr[2];
    charr[1]=L'\0';

    for( const WCHAR *ch=str; *ch!=L'\0'; ++ch ) {
        if( !iswdigit(*ch) && *ch!=L'_' && !iswlower(*ch) )
            need_quote=true;
        charr[0]=*ch;

        if( *ch==L'"' )
            // Double the quote so it won't break the identifier
            ret+=charr;

        ret+=charr;
    }

    if( need_quote ) {
        ret=L"\""+ret+L"\"";
    }

    return ret;
}