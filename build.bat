@echo off

:: clear temp dir
rd /q /s build
::rd /q /s pack

:: build
cmake -S . -B build -A Win32

cmake --build build --config debug
::cmake --build build --config release 

::cmake --install build --prefix output --config debug
::cmake --install build --prefix output --config release 

@echo on

