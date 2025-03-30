@echo off
setlocal enabledelayedexpansion

echo Searching for required dependencies...

rem Try to find OpenCV installation
set "OPENCV_DIR="
if exist "C:\tools\opencv\build" (
    set "OPENCV_DIR=C:\tools\opencv\build"
) else if exist "C:\opencv\build" (
    set "OPENCV_DIR=C:\opencv\build"
) else if defined OPENCV_DIR (
    echo Found OpenCV in environment variable: %OPENCV_DIR%
) else (
    for /d %%i in ("C:\Program Files*\opencv*") do (
        if exist "%%i\build" set "OPENCV_DIR=%%i\build"
    )
)

if not defined OPENCV_DIR (
    echo WARNING: OpenCV installation not found. You may need to set paths manually.
) else (
    echo Found OpenCV at: !OPENCV_DIR!
    set "INCLUDE=%INCLUDE%;!OPENCV_DIR!\include"
    set "LIB=%LIB%;!OPENCV_DIR!\x64\vc16\lib"
    set "PATH=%PATH%;!OPENCV_DIR!\x64\vc16\bin"
)

rem Try to find ImageMagick installation
set "IMAGEMAGICK_DIR="
if exist "C:\Program Files\ImageMagick*" (
    for /d %%i in ("C:\Program Files\ImageMagick*") do (
        set "IMAGEMAGICK_DIR=%%i"
    )
) else (
    reg query "HKLM\SOFTWARE\ImageMagick" /s 2>nul | findstr /i "InstallDir" >nul
    if !errorlevel! equ 0 (
        for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\ImageMagick" /s ^| findstr /i "InstallDir"') do (
            set "IMAGEMAGICK_DIR=%%b"
        )
    )
)

if not defined IMAGEMAGICK_DIR (
    echo WARNING: ImageMagick installation not found. GIF generation may not work properly.
) else (
    echo Found ImageMagick at: !IMAGEMAGICK_DIR!
    set "PATH=%PATH%;!IMAGEMAGICK_DIR!"
)

echo Environment variables set:
echo INCLUDE = %INCLUDE%
echo LIB = %LIB%
echo PATH = %PATH%

rem Pindah ke folder bin (asumsi run.bat berada di direktori root proyek)
cd /d "%~dp0\bin"

echo.
echo Running QuadTree Image Compression...
if "%~1"=="basic" (
    echo Mode: Basic
) else if not "%~1"=="" (
    echo Mode: %~1
) else (
    echo Mode: Standard
)
echo.

rem Jalankan executable dengan semua parameter yang diberikan
quadtree_compression.exe %*
