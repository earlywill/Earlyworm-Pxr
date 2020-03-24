set RMAN_VERSION=23.2
set RMANTREE=C:\\Program Files\Pixar\RenderManProServer-%RMAN_VERSION%\

"%RMANTREE%\bin\prman" -progress fisheye_debug.rib
"%RMANTREE%\bin\prman" -progress fisheye_art.rib

@pause