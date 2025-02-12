@echo off

set BUILD_ARGS= /6r -fp6
set BUILD_ARGS_CDECL= /6r -fp6 -ecc


echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/io.c" -i="lehidjoy" -fo="output/obj/io_wat"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/DS_CRC32.c" -i="lehidjoy" -fo="output/obj/DS_CRC32_wat"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/helpers.c" -i="lehidjoy" -fo="output/obj/helpers_wat"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/hidjoy_input.c" -i="lehidjoy" -fo="output/obj/hidjoy_input_wat"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS% "c_src/hidjoy_output.c" -i="lehidjoy" -fo="output/obj/hidjoy_output_wat"
echo.
echo.

echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/io.c" -i="lehidjoy" -fo="output/obj/io_cdecl"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/DS_CRC32.c" -i="lehidjoy" -fo="output/obj/DS_CRC32_cdecl"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/helpers.c" -i="lehidjoy" -fo="output/obj/helpers_cdecl"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/hidjoy_input.c" -i="lehidjoy" -fo="output/obj/hidjoy_input_cdecl"
echo.
echo.
echo.
echo.
@wcc386 %BUILD_ARGS_CDECL% "c_src/hidjoy_output.c" -i="lehidjoy" -fo="output/obj/hidjoy_output_cdecl"
echo.
echo.



echo.
echo.
@wlib -b -c -fo "lehidjoy.lib" ^
	output/obj/io_wat.obj            output/obj/io_cdecl.obj ^
	output/obj/DS_CRC32_wat.obj      output/obj/DS_CRC32_cdecl.obj ^
	output/obj/helpers_wat.obj       output/obj/helpers_cdecl.obj ^
	output/obj/hidjoy_input_wat.obj  output/obj/hidjoy_input_cdecl.obj ^
	output/obj/hidjoy_output_wat.obj output/obj/hidjoy_output_cdecl.obj

echo.
echo.
@wcc386 /6r -fp6 -ecc "main.c"
echo.
echo.
@wlink name test.exe file main.obj Library ddk/hid.lib , setupapi.lib , lehidjoy.lib

echo.
echo.

pause