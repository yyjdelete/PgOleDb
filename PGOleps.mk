
PGOleps.dll: dlldata.obj PGOle_p.obj PGOle_i.obj
	link /dll /out:PGOleps.dll /def:PGOleps.def /entry:DllMain dlldata.obj PGOle_p.obj PGOle_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del PGOleps.dll
	@del PGOleps.lib
	@del PGOleps.exp
	@del dlldata.obj
	@del PGOle_p.obj
	@del PGOle_i.obj
