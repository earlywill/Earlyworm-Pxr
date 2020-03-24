rem set to custom plugin location
set CUSTOM_PLUGIN_PATH="D:\scenes\rman\custom\plugins"
echo %CUSTOM_PLUGIN_PATH%
rem uncomment the line below to specify custom RMANTREE
set RMAN_VERSION="23.2"
set RMANTREE=C:\\Program Files\Pixar\RenderManProServer-%RMAN_VERSION%\

rem set path to version builds
set BUILD_PLUGIN_PATH="D:\tools\github\Earlyworm-Pxr\plugin\r%RMAN_VERSION%\windows\"
set PLUGIN_NAME="WillFisheye"

mkdir %BUILD_PLUGIN_PATH%\args

rem set this to the your MSVC install
call "D:\programs\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"


rem WillActionCam
rem cl -c -I"%RMANTREE%\include"   /D "WIN32" /D "_WINDLL"  /EHsc  WillActionCam.cpp
rem link /out:WillActionCam.dll /dll /libpath:"%RMANTREE%\lib" "WillActionCam.obj" libprman.lib
cl -MD /EHsc /D "WIN32" /D "_WINDLL" /I"%RMANTREE%\include" -c %PLUGIN_NAME%.cpp
link  /DLL /out:%PLUGIN_NAME%.dll %PLUGIN_NAME%.obj "%RMANTREE%\lib\libprman.lib" "%RMANTREE%\lib\libpxrcore.lib"
rem cleanup and install
del %PLUGIN_NAME%.exp %PLUGIN_NAME%.lib %PLUGIN_NAME%.obj
copy %PLUGIN_NAME%.dll "%BUILD_PLUGIN_PATH%\%PLUGIN_NAME%.dll"
move %PLUGIN_NAME%.dll "%CUSTOM_PLUGIN_PATH%\%PLUGIN_NAME%.dll"
copy ".\args\%PLUGIN_NAME%.args" %CUSTOM_PLUGIN_PATH%\args\%PLUGIN_NAME%.args
copy ".\args\%PLUGIN_NAME%.args" %BUILD_PLUGIN_PATH%\args\%PLUGIN_NAME%.args

pause