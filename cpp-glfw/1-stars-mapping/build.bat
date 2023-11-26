set system=win64

set libs=..\vendor\%system%\lib\glfw3_mt.lib user32.lib gdi32.lib shell32.lib opengl32.lib
for %%I in (.) do set dirname=%%~nxI

set files=src\*.cpp ..\vendor\src\glad.c

CL /Zi /EHsc /I include /I ..\vendor\%system%\include %files% /link %libs% /OUT:%dirname%.exe
del .\*.obj .\*.ilk .\*.pdb