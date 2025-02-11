@echo off

echo.
echo.
@wcc386 /6r main.c
echo.
echo.


@wlink name test.exe file main.obj Library ddk/hid.lib

echo.
echo.

pause