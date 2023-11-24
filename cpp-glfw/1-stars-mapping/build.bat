set system=win64

set libs=..\vendor\%system%\lib\glfw3_mt.lib user32.lib gdi32.lib shell32.lib opengl32.lib
for %%I in (.) do set dirname=%%~nxI

set files=src\main.cpp ..\vendor\src\glad.c

CL /Zi /I ..\vendor\%system%\include %files% /link %libs% /OUT:%dirname%.exe
del .\*.obj .\*.ilk .\*.pdb