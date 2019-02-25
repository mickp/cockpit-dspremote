cl /c /EHsc /I include C67lib.cpp
link /DLL C67lib.obj M62DLL.lib /OUT:C67.dll /DEF:C67.def
copy C67.dll ..\
copy IIDrvx40.dll ..\