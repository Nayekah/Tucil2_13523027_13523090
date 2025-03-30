@echo off
setlocal

REM Tentukan direktori root proyek (lokasi file build.bat ini)
set "PROJECT_ROOT=%~dp0"
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

REM Set lokasi OpenCV (sesuaikan path ini dengan lokasi OpenCVConfig.cmake Anda)
set "OPENCV_DIR=C:\tools\opencv\build"
REM Tambahkan folder bin OpenCV ke PATH agar executable OpenCV bisa diakses
set "PATH=%OPENCV_DIR%\bin;%PATH%"

REM Periksa apakah CMakeLists.txt ada di PROJECT_ROOT
if not exist "%PROJECT_ROOT%\CMakeLists.txt" (
    echo CMakeLists.txt tidak ditemukan di %PROJECT_ROOT%.
    pause
    exit /b 1
)

REM Buat folder build khusus Windows (misalnya, bin\windows) jika belum ada
if not exist "%PROJECT_ROOT%\bin\windows" (
    mkdir "%PROJECT_ROOT%\bin\windows"
)

REM Pindah ke folder build
cd "%PROJECT_ROOT%\bin\windows"

REM Jalankan CMake dan override runtime output directory agar executable ditempatkan di %PROJECT_ROOT%\bin
cmake -DOpenCV_DIR="%OPENCV_DIR%" -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJECT_ROOT%\bin" -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE="%PROJECT_ROOT%\bin" -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG="%PROJECT_ROOT%\bin" "%PROJECT_ROOT%"

REM Lakukan build proyek dengan konfigurasi Release
cmake --build . --config Release

echo.
echo Build complete. Executable akan berada di %PROJECT_ROOT%\bin\quadtree_compression.exe
