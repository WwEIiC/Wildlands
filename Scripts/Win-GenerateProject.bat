@echo off
pushd ..\
call Externals\Premake\Premake5.exe vs2022
popd

IF %ERRORLEVEL% NEQ 0 ( PAUSE)