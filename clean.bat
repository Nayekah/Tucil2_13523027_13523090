@echo off
setlocal

rem Tentukan direktori root proyek (lokasi file clean.bat berada)
set "PROJECT_ROOT=%~dp0"
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

rem Definisikan path folder "windows" di dalam bin (jika ada)
set "WINDOWS_BUILD_DIR=%PROJECT_ROOT%\bin\windows"

rem Definisikan path temporary build folder (jika digunakan pada build.bat)
set "BUILD_DIR=%PROJECT_ROOT%\build"

rem Definisikan path executable di dalam folder bin
set "EXECUTABLE=%PROJECT_ROOT%\bin\quadtree_compression.exe"

echo Cleaning up...

if exist "%WINDOWS_BUILD_DIR%" (
    rmdir /s /q "%WINDOWS_BUILD_DIR%"
    echo Deleted folder: %WINDOWS_BUILD_DIR%
) else (
    echo Folder %WINDOWS_BUILD_DIR% does not exist.
)

if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo Deleted folder: %BUILD_DIR%
) else (
    echo Folder %BUILD_DIR% does not exist.
)

if exist "%EXECUTABLE%" (
    del /q "%EXECUTABLE%"
    echo Deleted executable: %EXECUTABLE%
) else (
    echo Executable %EXECUTABLE% not found.
)

echo Clean complete.
