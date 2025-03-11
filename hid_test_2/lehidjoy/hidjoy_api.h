#ifndef HIDJOY_API_H
#define HIDJOY_API_H

/*
	DSW_Api.h is part of DualSenseWindows
	https://github.com/Ohjurot/DualSense-Windows

	Contributors of this file:
	11.2020 Ludwig Füchsl

	Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/


#if defined(DS5W_BUILD_DLL)
	#define DS5W_API __declspec(dllexport)
#elif defined(DS5W_BUILD_LIB)
	#define DS5W_API
#elif defined(DS5W_USE_LIB)
	#define DS5W_API
#else
	#define DS5W_API __declspec(dllimport)
#endif

#define DS5W_SUCCESS(expr) ((expr) == OK)
#define DS5W_FAILED(expr) ((expr) != OK)

#define DS5W_OK 						0
#define DS5W_E_UNKNOWN 					1
#define DS5W_E_INSUFFICIENT_BUFFER 		2
#define DS5W_E_EXTERNAL_WINAPI 			3
#define DS5W_E_STACK_OVERFLOW 			4
#define DS5W_E_INVALID_ARGS 			5
#define DS5W_E_CURRENTLY_NOT_SUPPORTED  6
#define DS5W_E_DEVICE_REMOVED 			7
#define DS5W_E_BT_COM 					8


/* Enum for return values */
typedef enum 
{
    /* Operation completed without an error */
    OK = 0,

    /* Operation encountered an unknown error */
    E_UNKNOWN = 1,

    /* The user supplied buffer is to small */
    E_INSUFFICIENT_BUFFER = 2,

    /* External unexpected winapi error (please report as issue if you get this error!) */
    E_EXTERNAL_WINAPI = 3,

    /* Not enough memory on the stack */
    E_STACK_OVERFLOW = 4,

    /* Invalid arguments */
    E_INVALID_ARGS = 5,

    /* This feature is currently not supported */
    E_CURRENTLY_NOT_SUPPORTED = 6,

    /* Device was disconnected */
    E_DEVICE_REMOVED = 7,

    /* Bluetooth communication error */
    E_BT_COM = 8
} DS5W_ReturnValue;

typedef DS5W_ReturnValue DS5W_RV;


#endif /* HIDJOY_API_H */
