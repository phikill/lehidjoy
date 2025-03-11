@echo off

if not exist "output" (
    mkdir "output"
    mkdir "output/obj"
) else (
    echo.
)

if not exist "output/obj" (
    mkdir "output/obj"
) else (
    echo.
)

set BUILD_ARGS= /6r -fp6
set BUILD_ARGS_CDECL= /6r -fp6 -ecc

echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/io.c" -i="lehidjoy" -fo="output/obj/io_wat" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/DS_CRC32.c" -i="lehidjoy" -fo="output/obj/DS_CRC32_wat" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/helpers.c" -i="lehidjoy" -fo="output/obj/helpers_wat" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/hidjoy_input.c" -i="lehidjoy" -fo="output/obj/hidjoy_input_wat" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/hidjoy_output.c" -i="lehidjoy" -fo="output/obj/hidjoy_output_wat" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.

echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/io.c" -i="lehidjoy" -fo="output/obj/io_cdecl" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/DS_CRC32.c" -i="lehidjoy" -fo="output/obj/DS_CRC32_cdecl" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/helpers.c" -i="lehidjoy" -fo="output/obj/helpers_cdecl" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/hidjoy_input.c" -i="lehidjoy" -fo="output/obj/hidjoy_input_cdecl" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/hidjoy_output.c" -i="lehidjoy" -fo="output/obj/hidjoy_output_cdecl" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.

REM Check if the library already exists, delete it to avoid conflicts.

if exist lehidjoy.lib (
    echo.
    del lehidjoy.lib
) else (
    echo.
)


echo.
echo.
echo ========== LIB CREATION ===========
@wlib -b -c -fo "lehidjoy.lib" ^
	output/obj/io_wat.obj            output/obj/io_cdecl.obj ^
	output/obj/DS_CRC32_wat.obj      output/obj/DS_CRC32_cdecl.obj ^
	output/obj/helpers_wat.obj       output/obj/helpers_cdecl.obj ^
	output/obj/hidjoy_input_wat.obj  output/obj/hidjoy_input_cdecl.obj ^
	output/obj/hidjoy_output_wat.obj output/obj/hidjoy_output_cdecl.obj ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo ========== ------------ ===========


echo.
echo.
@wcc386 /6r -fp6 -ecc "main.c" ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"
echo.
echo.
echo ======= LINKER =========
@wlink name test_wat.exe file main.obj Library ddk/hid.lib , setupapi.lib , lehidjoy.lib ^
     2>&1 | findstr /V /C:"Open Watcom" /C:"Portions" /C:"See"

echo.
echo.

pause