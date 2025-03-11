#ifndef DEVICE_H
#define DEVICE_H

#include<stdbool.h>
#include<stdio.h>

/*
    Device.h is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/


/* Enum for device connection type */
typedef unsigned char _DeviceConnection;
typedef enum _DeviceConnection
{
    /* Controller connected via USB */
    USB = 0,

    /* Controller connected via Bluetooth */
    BT = 1,
} DeviceConnection;

/* Framework for storing device information during discovery */
typedef struct 
{
    /* Internal structure to encapsulate data and prevent external modifications */
    struct 
    {
        /* Path to discovered device */
        char path[260];

        /* Discovered device connection type */
        DeviceConnection connection;
    } _internal;
} DeviceEnumInfo;

/* Structure for device context */
typedef struct 
{
    /* Internal structure to encapsulate data and try to prevent external modifications */
    struct 
    {
        /* Path to device */
        char devicePath[260];

        /* Handle for open device */
        void* deviceHandle;

        /* Device connection type */
        DeviceConnection connection;

        /* Current connection state */
        bool connected;

        /* Input HID buffer (will be allocated by context initialization function) */
        unsigned char hidBuffer[547];
    } _internal;
} DeviceContext;


#endif /* DEVICE_H */

