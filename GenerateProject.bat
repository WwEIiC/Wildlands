@echo off
call Externals\Premake\Premake5.exe vs2022

IF %ERRORLEVEL% NEQ 0 ( PAUSE)