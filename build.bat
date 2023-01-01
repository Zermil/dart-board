@echo off

REM Change this to your visual studio's 'vcvars64.bat' script path
set MSVC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
set CXXFLAGS=/std:c++17 /EHsc /W4 /WX /FC /wd4996 /nologo %*
set INCLUDES=/I"deps\include"
set LIBS="deps\lib\SDL2.lib" "deps\lib\SDL2main.lib" shell32.lib gdi32.lib Advapi32.lib	Ole32.lib	OleAut32.lib

call %MSVC_PATH%\vcvars64.bat

pushd %~dp0
if not exist .\build mkdir build
if not exist .\build\.gitkeep type nul > .\build\.gitkeep
cl %CXXFLAGS% %INCLUDES% code\main.cpp /Fo:build\ /Fe:build\dart-board.exe %LIBS% /link /SUBSYSTEM:CONSOLE
popd
