# Earlyworm-Pxr

Plugin(s?) for Pixar's RenderMan.

Provided largely as-is for the moment. This is a collection of various plugins I've tried to create over the years. Install is largely focused on Windows environment.

Most of these were started in R23 and R24, they will likely need updates to work under newer versions of RenderMan.

## Install

1. Copy the plugin (dll/so) to a plugin location that RenderMan knows about.
2. Copy the args files (arg) to the args sub-directory within that location.
3. Done.

## Compiling

### Windows

1. Open x64 Native Tools Command Prompt for VS 2017
2. Run following two lines. 
   - `cl -MD /EHsc /D "WIN32" /D "_WINDLL" /I"%RMANTREE%\include" -c %PLUGIN_NAME%.cpp`
   - `link  /DLL /out:%PLUGIN_NAME%.dll %PLUGIN_NAME%.obj "%RMANTREE%\lib\libprman.lib" "%RMANTREE%\lib\libpxrcore.lib"`

3. Follow install instructions above for copying to the right location.

## RIB

RIB files are provided to test out the plugin works. For the RIB to work you'll need to modify the following line to the location where you installed the plugin. 

On Linux/Mac. If you've installed your plugin in a folder called /scenes/rman/custom/plugins then the search path should look like this.

`Option "searchpath" "string rixplugin" ["/scenes/rman/custom/plugins:@"]`

On Windows. If you've installed your plugin in a folder called D:/scenes/rman/custom/plugins then the search path should look like this.

`Option "searchpath" "string rixplugin" ["D:\\scenes\\rman\\custom\\plugins:@"]`

## TODO

* Cleanup folder structure. 
* Update to latest R26 API


