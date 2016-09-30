@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@echo off
setlocal EnableDelayedExpansion

REM ===============================================================================================================
REM == This batch shows how to parser the tests.lst. It will call “ExecuteTest” for each test in the list, the test 
REM == name will be in projectName, and each parameter in the list will set as a local variable. It is necessary to 
REM == use ‘!’ besides ‘%’ in these variables to avoid batch expansion problem. 
REM ===============================================================================================================

call :ParserTestLst
goto :eof


REM Parser tests.lst
:ParserTestLst
set buildlog=tests.lst
set testName=false
set projectName=

for /F "tokens=*" %%F in (%buildlog%) do (
    if /i "%%F"=="" (
        REM ignore empty line.
    ) else ( 
        set command=%%F
        if /i "!command:~0,1!"=="#" (
            echo Comment=!command:~1!
        ) else ( if /i "!command:~0,1!"=="[" (
            call :ExecuteTest
            set projectName=!command:~1,-1!
            if /i "!projectName!"=="End" goto :eof
        ) else (
            for /f "tokens=1 delims==" %%A in ("!command!") do set key=%%A
            call set value=%%command:!key!=%%
            set value=!value:~1!
            set !key!=!value!
        ) ) ) 
    ) 
)
goto :eof


REM Execute each test in the Tests.lst
:ExecuteTest
if not "!projectName!"=="" (
    echo.
    echo Execute !projectName!
    echo RelativePath=!RelativePath!
    echo RelativeWorkingDir=!RelativeWorkingDir!
    echo MaxAllowedDurationSeconds=!MaxAllowedDurationSeconds!
    echo Categories=!Categories!
    echo Hardware=!Hardware!
    echo CPUParameters=!CPUParameters!
    echo Build=!Build!
    echo Installation=!Installation!
    echo Execution=!Execution!
)
goto :eof