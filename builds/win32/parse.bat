@echo off

@echo.

@call setenvvar.bat %*
@if errorlevel 1 (goto :END)

@echo Generating parse.cpp and dsql.tab.h

%FB_ROOT_PATH%\temp\%FB_OBJ_DIR%\btyacc\btyacc -l -d -c %FB_ROOT_PATH%\src\dsql\parse.cpp -h %FB_ROOT_PATH%\src\include\gen\parse.h -S %FB_ROOT_PATH%\src\dsql\btyacc_fb.ske %FB_ROOT_PATH%\src\dsql\parse.y 2>y.txt
@if errorlevel 1 (type y.txt && exit /B 1)
@type y.txt

:END
