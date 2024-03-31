@echo off

IF NOT EXIST .\build mkdir .\build
pushd .\build
cl -FC -Zi /nologo ..\code\win32.cpp user32.lib gdi32.lib opengl32.lib ws2_32.lib
popd

move build\win32.exe game.exe 2> nul
