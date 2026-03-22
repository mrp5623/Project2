@echo off

setlocal

if defined VCPKG_ROOT (
    set TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
) else if exist "C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake" (
    set TOOLCHAIN=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake
) else (
    if not exist vcpkg (
        git clone https://github.com/microsoft/vcpkg.git
        call vcpkg\bootstrap-vcpkg.bat
    )
    set TOOLCHAIN=%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake
)

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN%
cmake --build build --config Release

endlocal