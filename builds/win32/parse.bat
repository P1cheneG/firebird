@echo off

@echo.

@call setenvvar.bat %*
@if errorlevel 1 (goto :END)

@echo Generating parse.cpp and dsql.tab.h

%FB_ROOT_PATH%\temp\%FB_OBJ_DIR%\btyacc\btyacc -l -d -S %FB_ROOT_PATH%\src\dsql\btyacc_fb.ske y.y 2>y.txt
@if errorlevel 1 (type y.txt && exit /B 1)
@type y.txt

@sed -n -e "s/.*btyacc: \(.*conflicts.*\)/\1/p" y.txt > %FB_ROOT_PATH%\src\dsql\parse-conflicts.txt

@copy y_tab.h %FB_ROOT_PATH%\src\include\gen\parse.h > nul
@copy y_tab.c %FB_ROOT_PATH%\src\dsql\parse.cpp > nul
@del y.y
@del y.txt
@del types.y
@del y_tab.h
@del y_tab.c
@del sed*

:END
