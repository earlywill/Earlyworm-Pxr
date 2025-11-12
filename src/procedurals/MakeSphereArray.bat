set RMAN_VERSION="26.3"
set RMANTREE=C:\\Program Files\Pixar\RenderManProServer-%RMAN_VERSION%\

set TARGETDIR=D:\scenes\rman\custom\procedurals\

call "D:\programs\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
set PLUGIN_NAME="SphereArray"

cl -MD /EHsc /D "WIN32" /D "_WINDLL" /I"%RMANTREE%\include" -c %PLUGIN_NAME%.cpp
link  /DLL /out:%PLUGIN_NAME%.dll %PLUGIN_NAME%.obj "%RMANTREE%\lib\libprman.lib" "%RMANTREE%\lib\libpxrcore.lib"

copy %PLUGIN_NAME%.dll %TARGETDIR%\%PLUGIN_NAME%.dll
copy %PLUGIN_NAME%.args %TARGETDIR%\args\%PLUGIN_NAME%.args


pause