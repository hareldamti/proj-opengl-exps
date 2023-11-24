set render=src\engine\render\render.c
set io=src\engine\io\io.c
set config=src\engine\config\config.c
set input=src\engine\input\input.c
set time=src\engine\time\time.c
set libs=..\vendor\lib\SDL2main.lib ..\vendor\lib\SDL2.lib
set files=src\main.c src\engine\global.c src\glad.c %render% %io% %time%
::%input% %config% 

for %%I in (.) do set dirname=%%~nxI

CL /Zi /I ..\vendor\include %files% /link %libs% /OUT:%dirname%.exe

del .\*.obj .\*.ilk .\*.pdb