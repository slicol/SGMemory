@echo off
setlocal enabledelayedexpansion
set batdir="%~dp0"
cd /d %batdir%
.\vswhere.exe -latest -property productPath > tmp.vswhere.txt
set /p VS_DIR=< tmp.vswhere.txt
set VS_DIR=%VS_DIR:devenv.exe=devenv.com%
.\vswhere.exe -latest -property catalog_productLineVersion > tmp.vswhere.txt
set /p VS_YEAR=< tmp.vswhere.txt
.\vswhere.exe -latest -property catalog_buildVersion > tmp.vswhere.txt
set /p VS_VERSION=< tmp.vswhere.txt
set VS_VERSION=%VS_VERSION:~0,2%
.\vswhere.exe -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe > tmp.vswhere.txt
set /p MSBUILD_DIR=< tmp.vswhere.txt
del tmp.vswhere.txt
echo "Visual Studio Dir = %VS_DIR%"
echo "Visual Studio Product Line = %VS_YEAR%"
echo "Visual Studio Version = %VS_VERSION%"

mkdir build
cd build
cmake -G "Visual Studio %VS_VERSION% %VS_YEAR%" -A x64 -S .. -B .
rem "%VS_DIR%" SGMemory.sln /Build "Release|x64" /MP
"%MSBUILD_DIR%" ALL_BUILD.vcxproj -m:8 -property:Configuration=Release -property:Platform=x64
pause