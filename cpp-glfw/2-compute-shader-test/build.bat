set system=win64

set libs=..\vendor\lib\%system%\glfw3_mt.lib user32.lib gdi32.lib shell32.lib opengl32.lib
for %%I in (.) do set dirname=%%~nxI

set files=src\*.cpp ..\vendor\src\*.c ..\vendor\src\*.cpp

CL /Zi /EHsc /I include /I ..\vendor\include /I ..\vendor\include\%system% %files% /link %libs% /OUT:%dirname%.exe
del .\*.obj .\*.ilk .\*.pdb