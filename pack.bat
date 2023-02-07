@echo off

:: clear temp dir
rd /q /s build
rd /q /s pack

:: build
cmake -S . -B build -A Win32  -DEXAMPLE_ENABLE=OFF

cmake --build build --config debug
cmake --build build --config release 

cmake --install build --prefix output --config debug
cmake --install build --prefix output --config release 

mkdir pack
:: copy nuget config json file
xcopy /Y  /F nuget.json pack
:: copy header file
mkdir pack\include
xcopy /Y /F output\Debug\include\aes70_client\*.h pack\include\aes70_client\*.h
mkdir pack\v14
:: copy bin 
xcopy /Y /F output\Debug\bin\aes70_client*.dll pack\v14\Debug\bin\
xcopy /Y /F output\Debug\lib\aes70*.lib pack\v14\Debug\lib\
xcopy /Y /F output\Debug\pdb\aes70*.pdb pack\v14\Debug\pdb\

xcopy /Y /F output\Release\bin\aes70_client*.dll pack\v14\Release\bin\
xcopy /Y /F output\Release\lib\aes70*.lib pack\v14\Release\lib\
xcopy /Y /F output\Release\pdb\aes70*.pdb pack\v14\Release\pdb\

cd pack

create_nuget.exe nuget.json

@echo on

