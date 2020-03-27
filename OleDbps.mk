
PgOleDbps.dll: dlldata.obj PgOleDb_p.obj PgOleDb_i.obj
	link /dll /out:PgOleDbps.dll /def:PgOleDbps.def /entry:DllMain dlldata.obj PgOleDb_p.obj PgOleDb_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del PgOleDbps.dll
	@del PgOleDbps.lib
	@del PgOleDbps.exp
	@del dlldata.obj
	@del PgOleDb_p.obj
	@del PgOleDb_i.obj
