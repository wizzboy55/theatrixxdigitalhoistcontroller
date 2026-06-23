@ECHO OFF
SETLOCAL EnableExtensions EnableDelayedExpansion

SET "ATPROGRAM_EXE="

FOR %%D IN (
    "%ProgramFiles%\Microchip\Studio"
    "%ProgramFiles(x86)%\Microchip\Studio"
    "%ProgramFiles%\Atmel\Studio"
    "%ProgramFiles(x86)%\Atmel\Studio"
) DO (
    IF EXIST "%%~D" (
        FOR /F "delims=" %%F IN ('DIR /B /S "%%~D\atprogram.exe" 2^>NUL') DO (
            SET "ATPROGRAM_EXE=%%~fF"
            GOTO :found
        )
    )
)

ECHO Could not find atprogram.exe under Microchip/Atmel Studio folders.
ECHO Install Microchip Studio first, then run this script again.
EXIT /B 1

:found
FOR %%I IN ("!ATPROGRAM_EXE!") DO SET "ATPROGRAM_DIR=%%~dpI"
IF "!ATPROGRAM_DIR:~-1!"=="\" SET "ATPROGRAM_DIR=!ATPROGRAM_DIR:~0,-1!"

SET "USER_PATH="
FOR /F "tokens=2,*" %%A IN ('REG QUERY "HKCU\Environment" /v Path 2^>NUL ^| FINDSTR /I "Path"') DO (
    SET "USER_PATH=%%B"
)

ECHO ;!USER_PATH!; | FIND /I ";!ATPROGRAM_DIR!;" >NUL
IF NOT ERRORLEVEL 1 (
    ECHO atprogram path already exists in user Path:
    ECHO   !ATPROGRAM_DIR!
    EXIT /B 0
)

IF DEFINED USER_PATH (
    SET "NEW_PATH=!USER_PATH!;!ATPROGRAM_DIR!"
) ELSE (
    SET "NEW_PATH=!ATPROGRAM_DIR!"
)

SETX Path "!NEW_PATH!" >NUL
IF ERRORLEVEL 1 (
    ECHO Failed to update user Path.
    EXIT /B 1
)

SET "PATH=!PATH!;!ATPROGRAM_DIR!"
ECHO Added to user Path:
ECHO   !ATPROGRAM_DIR!
ECHO Open a new Command Prompt and run: atprogram --version
EXIT /B 0
