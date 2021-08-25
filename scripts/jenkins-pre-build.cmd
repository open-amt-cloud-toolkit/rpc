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
set VCPKG_DIR=C:\opt\vcpkg-source

cd %VCPKG_DIR%

REM build vcpkg
git clone --branch 2021.05.12 https://github.com/microsoft/vcpkg.git
cd vcpkg
cmd /c bootstrap-vcpkg.bat

REM install CPPRestSDK
cmd /c vcpkg install cpprestsdk[websockets]:x64-windows-static
