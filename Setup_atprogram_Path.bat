@ECHO OFF
SETLOCAL EnableExtensions EnableDelayedExpansion

SET "ATPROGRAM_EXE="
SET "EXIT_CODE=0"
SET "RESULT_MESSAGE="
SET "RESULT_DETAIL="

ECHO Searching for atprogram.exe in common Microchip/Atmel Studio install paths...

FOR %%D IN (
    "%ProgramFiles%\Microchip\Studio"
    "%ProgramFiles(x86)%\Microchip\Studio"
    "%ProgramFiles%\Atmel\Studio"
    "%ProgramFiles(x86)%\Atmel\Studio"
) DO (
    ECHO   Checking: %%~D
    IF EXIST "%%~D" (
        FOR /F "delims=" %%F IN ('DIR /B /S "%%~D\atprogram.exe" 2^>NUL') DO (
            SET "ATPROGRAM_EXE=%%~fF"
            GOTO :found
        )
    )
)

SET "EXIT_CODE=1"
SET "RESULT_MESSAGE=Failed to configure atprogram in user Path."
SET "RESULT_DETAIL=Could not find atprogram.exe under Microchip/Atmel Studio folders. Install Microchip Studio first, then run this script again."
GOTO :finish

:found
ECHO Found atprogram.exe:
ECHO   !ATPROGRAM_EXE!
FOR %%I IN ("!ATPROGRAM_EXE!") DO SET "ATPROGRAM_DIR=%%~dpI"
IF "!ATPROGRAM_DIR:~-1!"=="\" SET "ATPROGRAM_DIR=!ATPROGRAM_DIR:~0,-1!"

SET "USER_PATH="
FOR /F "tokens=2,*" %%A IN ('REG QUERY "HKCU\Environment" /v Path 2^>NUL ^| FINDSTR /I "Path"') DO (
    SET "USER_PATH=%%B"
)

ECHO ;!USER_PATH!; | FIND /I ";!ATPROGRAM_DIR!;" >NUL
IF NOT ERRORLEVEL 1 (
    SET "EXIT_CODE=0"
    SET "RESULT_MESSAGE=No changes needed."
    SET "RESULT_DETAIL=atprogram path already exists in user Path: !ATPROGRAM_DIR!"
    GOTO :finish
)

IF DEFINED USER_PATH (
    SET "NEW_PATH=!USER_PATH!;!ATPROGRAM_DIR!"
) ELSE (
    SET "NEW_PATH=!ATPROGRAM_DIR!"
)

SETX Path "!NEW_PATH!" >NUL
IF ERRORLEVEL 1 (
    SET "EXIT_CODE=1"
    SET "RESULT_MESSAGE=Failed to configure atprogram in user Path."
    SET "RESULT_DETAIL=SETX failed while updating HKCU\\Environment\\Path."
    GOTO :finish
)

SET "PATH=!PATH!;!ATPROGRAM_DIR!"
SET "EXIT_CODE=0"
SET "RESULT_MESSAGE=Successfully added atprogram directory to user Path."
SET "RESULT_DETAIL=Added: !ATPROGRAM_DIR!  (open a new Command Prompt and run: atprogram --version)"

:finish
ECHO.
IF "!EXIT_CODE!"=="0" (
    ECHO [SUCCESS] !RESULT_MESSAGE!
) ELSE (
    ECHO [ERROR] !RESULT_MESSAGE!
)
IF DEFINED RESULT_DETAIL ECHO !RESULT_DETAIL!
ECHO.
PAUSE
EXIT /B !EXIT_CODE!
