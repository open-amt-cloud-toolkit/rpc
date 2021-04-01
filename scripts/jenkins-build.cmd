:: Build script
::   - Windows 10
::   - Visual Studio 2019
::   - Git
::
::  IMPORTANT!!!
::    Open "x64 Native Command Tool Prompt for VS 2019" as Administrator.
::

CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

REM current dir - RPC source directory
set BASE_DIR=%cd%
set VCPKG_DIR=C:\opt\vcpkg-source

REM build RPC


if exist "build" rd /q /s "build"

mkdir build
cd build
echo %VCPKG_DIR%\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\vcpkg\scripts\buildsystems\vcpkg.cmake ..
cmake --build . --config Release
dir %BASE_DIR%\build