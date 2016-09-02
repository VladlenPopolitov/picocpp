for %%a in (*.c) do ..\msvc\picoc\Debug\picoc.exe %%a >output\%%~na.expect 
