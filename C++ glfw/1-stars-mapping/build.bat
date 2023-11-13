set system=win64

::
set libs=..\vendor\%system%\lib\glfw3.lib ..\vendor\%system%\lib\glfw3_mt.lib
for %%I in (.) do set dirname=%%~nxI

set files=src\main.cpp

CL /Zi /NODEFAULTLIB /I ..\vendor\%system%\include %files% /link %libs% /OUT:%dirname%.exe
del .\*.obj .\*.ilk .\*.pdb