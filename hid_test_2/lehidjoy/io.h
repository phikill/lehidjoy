#ifndef IO_H
#define IO_H

/*
    DualSenseWindows API
    https://github.com/Ohjurot/DualSense-Windows

    MIT License

    Copyright (c) 2020 Ludwig Füchsl

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#include<windows.h>
#include<setupapi.h>
#include<devguid.h>
#include<initguid.h>
#include<hidsdi.h>

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

#include"hidjoy_api.h"
#include"hidjoy_device.h"
#include"hidjoy_state.h"


/* function to enumerate and detect HID devices */
DS5W_ReturnValue enumDevices(void           *ptrBuffer, 
                             unsigned int   inArrLength, 
                             unsigned int   *requiredLength, 
                             int            pointerToArray);

/* function to start the device according to enumDevices */
DS5W_ReturnValue initDeviceContext(DeviceEnumInfo*  ptrEnumInfo, 
                                   DeviceContext*   ptrContext);

/* function to release device data at the end of the program */
void freeDeviceContext(DeviceContext* ptrContext);

/* The function that re-connects the device */
DS5W_ReturnValue reconnectDevice(DeviceContext* ptrContext);

/* Main function to obtain device input state */
DS5W_ReturnValue getDeviceInputState(DeviceContext* ptrContext, 
									 DS5InputState* ptrInputState);

/* Main function to set the device output state */
DS5W_ReturnValue setDeviceOutputState(DeviceContext*  ptrContext, 
                                      DS5OutputState* ptrOutputState);


#endif /* IO_H */
