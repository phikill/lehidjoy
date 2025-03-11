/*
    IO.cpp is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/

#include<windows.h>

#include<wchar.h>

#include<setupapi.h>
#include<devguid.h>
#include<initguid.h>
#ifndef __HIDSDI_H__
    #include<hidsdi.h>
#endif


#include"io.h"
#include"DS_CRC32.h"
#include"hidjoy_input.h"
#include"hidjoy_output.h"

#include<string.h> 

#include<stddef.h> /* For size_t */



/* Manually aligned structure to avoid error 1784 on Watcom */
#ifdef __WATCOMC__
    #include<stdbool.h>

	#pragma pack(push, 1) 
	   typedef struct 
	   {
    	   DWORD cbSize;
    	   char DevicePath[1]; /* First character of the path */
	   } SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED;
	#pragma pack(pop)

    #define EINVAL 22  /* Error code for invalid arguments */
    typedef int errno_t;

    /* strncpy_s implementation for C89 */
    errno_t strncpy_s(char *dest, 
                      size_t dest_size, 
                      const char *src, 
                      size_t count) 
    {
        size_t i;

        if(dest == NULL || src == NULL) 
        {
            return EINVAL;  /* Returns error if pointers are invalid */
        }

        if(dest_size == 0) 
        {
            return EINVAL;  /* Returns error if destination size is zero */
        }

        for(i = 0; i < count && i < dest_size - 1 && src[i] != '\0'; i++) 
        {
            dest[i] = src[i];
        }

        /* Ensures that the destination will always have '\0' at the end */
        if(i < dest_size) 
        {
            dest[i] = '\0';
        } 
        else 
        {
            dest[dest_size - 1] = '\0'; /* If the entire string does not fit, end the string with '\0' */
        }

        return 0; /* Sucess */
    }

    /* strcpy_s implementation for C89 */
    int strcpy_s(char *dest, size_t dest_size, const char *src) 
    {
        size_t i;

        /* Checks for null pointers */
        if(!dest || !src) 
        {
            return 1; /* Indicates error */
        }

        /* Check if the destination size is sufficient */
        for (i = 0; i < dest_size - 1; i++) 
        {
            if ((dest[i] = src[i]) == '\0') 
            {
                return 0; /* Sucess */
            }
        }

        /* Ensures the resulting string is null terminated */
        dest[dest_size - 1] = '\0';

        return 1; /* Indicates truncation */
    }




#endif

#ifdef _MSC_VER
    #include<stdbool.h>

    #pragma pack(push, 1) 
       typedef struct 
       {
           DWORD cbSize;
           char DevicePath[1]; /* First character of the path */
       } SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED;
    #pragma pack(pop)
#endif /* _MSC_VER */

/*
    enumDevices()

This function is responsible for enumerating the HID devices connected to the system. Here's a summary of what she does:

Buffer check: It checks whether the input buffer (ptrBuffer) is correct and whether the array size (inArrLength) is valid.

HID device enumeration: Uses the SetupDiGetClassDevs function to get the list of connected HID devices and starts iterating over the devices.

Device type check: For each device, checks whether it is a HID device with the corresponding GUID and tries to get the device path.

Connection and device identification: Attempts to open the device and check if it is a DS5 control (by seeing the VendorID and ProductID), then collects information about the connection (USB or Bluetooth).

Information storage: If a DS5 control is found, its information is stored in a buffer provided as a parameter.

Return: Returns the number of devices found or an error if the buffer is too small or another error occurs during execution.
*/

DS5W_ReturnValue enumDevices(void           *ptrBuffer, 
                             unsigned int   inArrLength, 
                             unsigned int   *requiredLength, 
                             bool            pointerToArray) 
{
    HDEVINFO hidDiHandle;
    SP_DEVINFO_DATA hidDiInfo;
    DWORD devIndex = 0;
    GUID hidGuid;
    unsigned int inputArrIndex = 0;
    bool inputArrOverflow = false;
    
    /* Check for invalid non expected buffer */
    if(inArrLength && !ptrBuffer) 
    {
        inArrLength = 0;
    }

    HidD_GetHidGuid(&hidGuid);
    hidDiHandle = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if(!hidDiHandle || (hidDiHandle == INVALID_HANDLE_VALUE)) 
    {
        /* Error getting HID device list */
        printf(" \n Error getting HID device list \n"); 
        return DS5W_E_EXTERNAL_WINAPI;
    }

    /* Enumerate over hid device */
    hidDiInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    
    while(SetupDiEnumDeviceInfo(hidDiHandle, devIndex, &hidDiInfo))
    {
        DWORD ifIndex = 0;
        SP_DEVICE_INTERFACE_DATA ifDiInfo;

        ifDiInfo.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        /* Inner loop to enumerate current device interfaces */
        while (SetupDiEnumDeviceInterfaces(hidDiHandle, 
                                           &hidDiInfo, 
                                           &hidGuid, 
                                           ifIndex, 
                                           &ifDiInfo)) 
        {
            DWORD requiredSize = 0;
            SP_DEVICE_INTERFACE_DETAIL_DATA *deviceDetail;
            HANDLE deviceHandle;


            /* Gets the required size for the detail structure */
            SetupDiGetDeviceInterfaceDetail(hidDiHandle, &ifDiInfo, NULL, 0, &requiredSize, NULL);

            /* Check size */
            if(requiredSize > (260 * sizeof(wchar_t))) 
            {
                SetupDiDestroyDeviceInfoList(hidDiHandle);
                return DS5W_E_EXTERNAL_WINAPI;
            }

            /* Allocate memory for path on the stack */
            deviceDetail = (struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A *)
                           (SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED*)malloc(requiredSize + sizeof(DWORD));

            if(!deviceDetail) 
            {
                printf("Error allocating memory.\n");
                SetupDiDestroyDeviceInfoList(hidDiHandle);
                return DS5W_E_STACK_OVERFLOW;
            }

            /* Get device path */
            deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED);

            if(SetupDiGetDeviceInterfaceDetail(hidDiHandle, &ifDiInfo, deviceDetail, requiredSize, NULL, NULL)) 
            {
                /* printf("  Interface %d: %s\n", ifIndex, deviceDetail->DevicePath); */
            }
            else
            {
                DWORD error = GetLastError();
                printf(" \n [!ERROR]: SetupDiGetDeviceInterfaceDetail failed with error %lu \n", error);
            }

            /* Check if input array has space */
            /* Check if device is reachable */

            deviceHandle = CreateFileA(deviceDetail->DevicePath, 
                                       GENERIC_READ | GENERIC_WRITE, 
                                       FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                       NULL, 
                                       OPEN_EXISTING, 
                                       0, /* NULL */
                                       NULL);

            /* Check if device is reachable */
            if(deviceHandle && (deviceHandle != INVALID_HANDLE_VALUE)) 
            {
                /* Get vendor and product id */
                unsigned int vendorId = 0;
                unsigned int productId = 0;
                HIDD_ATTRIBUTES deviceAttributes;
                PHIDP_PREPARSED_DATA ppd;
                

                if(HidD_GetAttributes(deviceHandle, &deviceAttributes)) 
                {
                    vendorId = deviceAttributes.VendorID;
                    productId = deviceAttributes.ProductID;
                }

                /* Check if ids match */

                /* DS4 CUH-ZCT1x | before 2016 */
                if(vendorId == 0x054C && productId == 0x05C4) 
                {
                    printf(" \n DualShock 4 - Pre 2016 - Model CUH-ZCT1x \n");
                }

                /* DS4 CUH-ZCT2x | After 2016 */
                if(vendorId == 0x054C && productId == 0x09CC) 
                {
                    DeviceEnumInfo* ptrInfo = NULL;

                    printf(" \n DualShock 4 - After 2016 - Model CUH-ZCT2x \n");
                }

                /* DualSense5  */
                if(vendorId == 0x054C && productId == 0x0CE6) 
                {
                    DeviceEnumInfo* ptrInfo = NULL;
                
                    if(inputArrIndex < inArrLength) 
                    {
                        if (pointerToArray) 
                        {
                            ptrInfo = &(((DeviceEnumInfo*)ptrBuffer)[inputArrIndex]);
                        }
                        else
                        {
                            ptrInfo = (((DeviceEnumInfo**)ptrBuffer)[inputArrIndex]);
                        }
                    }
                
                    /* Copy path */
                    if(ptrInfo) 
                    {
                        strcpy_s(ptrInfo->_internal.path, sizeof(ptrInfo->_internal.path), (const char*)deviceDetail->DevicePath);
                        
                    }

                    /* Get preparsed data */
                    if(HidD_GetPreparsedData(deviceHandle, &ppd)) 
                    {
                        /* Get device capcbilitys */
                        HIDP_CAPS deviceCaps;
                        if(HidP_GetCaps(ppd, &deviceCaps) == HIDP_STATUS_SUCCESS) 
                        {
                            /* Check for device connection type */
                            if(ptrInfo) 
                            {
                                /* Check if controller matches USB specifications */
                                if (deviceCaps.InputReportByteLength == 64) 
                                {
                                    ptrInfo->_internal.connection = USB;

                                    /* Device found and valid -> Inrement index */
                                    inputArrIndex++;
                                }
                                /* Check if controler matches BT specifications */
                                else if(deviceCaps.InputReportByteLength == 78) 
                                {
                                    ptrInfo->_internal.connection = BT;

                                    /* Device found and valid -> Inrement index */
                                    inputArrIndex++;
                                }
                            }
                        }

                        /* Free preparsed data */
                        HidD_FreePreparsedData(ppd);
                    }
                }
                /* Close device */
                CloseHandle(deviceHandle); 
            }
            else
            {
                /* printf(" [!ERROR]: deviceHandle Error \n \n"); */
            }
            ifIndex++; /* Increment index */
            free(deviceDetail); /* Free device from mem */
        }
        devIndex++; /* Increment index */
    } 

    /* Check if you have reached the end of the list */
    if(GetLastError() != ERROR_NO_MORE_ITEMS) 
    {
        printf("Error enumerating devices.\n");
    }
    
    SetupDiDestroyDeviceInfoList(hidDiHandle);
    
    /* Set required size if exists */
    if(requiredLength) 
    {
        *requiredLength = inputArrIndex;
    }
    
    /* Check if array was suficient */
    if(inputArrIndex <= inArrLength) 
    {
        return DS5W_OK;
    }
    else 
    {
        return DS5W_E_INSUFFICIENT_BUFFER;
    }
}

#define DEVICE_PATH_MAX 260

DS5W_ReturnValue initDeviceContext(DeviceEnumInfo* ptrEnumInfo, 
                                   DeviceContext*  ptrContext) 
{
    HANDLE deviceHandle;
    unsigned short reportLength = 0;


    /* Check if pointers are valid */
    if(!ptrEnumInfo || !ptrContext) 
    {
        return DS5W_E_INVALID_ARGS;
    }


    /* Checks if the device path is valid */
    if(strlen(ptrEnumInfo->_internal.path) == 0) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    /* Connect to device */
    deviceHandle = CreateFileA(ptrEnumInfo->_internal.path,
                              GENERIC_READ | GENERIC_WRITE, 
                              FILE_SHARE_READ | FILE_SHARE_WRITE, 
                              NULL, 
                              OPEN_EXISTING, 
                              0, /* NULL */
                              NULL);

    if(deviceHandle == INVALID_HANDLE_VALUE || deviceHandle == NULL) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    /* Write in context */
    ptrContext->_internal.connected = 1;
    ptrContext->_internal.connection = ptrEnumInfo->_internal.connection;
    ptrContext->_internal.deviceHandle = deviceHandle;

    strncpy_s(ptrContext->_internal.devicePath, 
              sizeof(ptrContext->_internal.devicePath), 
              ptrEnumInfo->_internal.path, 260 - 1);
    ptrContext->_internal.devicePath[260 - 1] = '\0'; /* Guarantees termination */

    /* Get input report length */
    if(ptrContext->_internal.connection == BT) 
    { 
        /* Start BT by reading feature report 5 */
        unsigned char fBuffer[64];
        fBuffer[0] = 0x05;
        if(!HidD_GetFeature(deviceHandle, fBuffer, 64)) 
        {
            return DS5W_E_BT_COM;
        }

        /* The Bluetooth input report is 78 bytes long */
        reportLength = 547;
    } 
    else 
    {
        /* USB report is 64 bytes long */
        reportLength = 64;
    }
    return DS5W_OK;
}

 
void freeDeviceContext(DeviceContext* ptrContext) 
{
    DS5OutputState os;

    /* Check if the device is present */
    if(ptrContext->_internal.deviceHandle) 
    {
        /* Send zero output report to disable all active effects */
        memset(&os, 0, sizeof(DS5OutputState));  /* Clear the state structure */

        /* Set triggers to "No resistance" type */
        os.leftTriggerEffect.effectType = NoResitance;
        os.rightTriggerEffect.effectType = NoResitance;
        os.disableLeds = 1;

        /* Update device status */
        setDeviceOutputState(ptrContext, &os);

        /* Close device handle */
        CloseHandle(ptrContext->_internal.deviceHandle);
        ptrContext->_internal.deviceHandle = NULL;
    }

    /* Disconnect */
    ptrContext->_internal.connected = 0;

    /* Clear device path string */
    ptrContext->_internal.devicePath[0] = '\0';  /* Set to empty string */
}

/* The function that re-connects the device */
DS5W_ReturnValue reconnectDevice(DeviceContext* ptrContext)
{
    HANDLE deviceHandle;

    /* Verify that the device path is not empty */
    if(strlen(ptrContext->_internal.devicePath) == 0) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    /* Connect to device */
    deviceHandle = CreateFileA(ptrContext->_internal.devicePath, 
                               GENERIC_READ | GENERIC_WRITE, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE, 
                               NULL, 
                               OPEN_EXISTING, 
                               0, /* NULL */
                               NULL);

    /* Check if the device is connected correctly */
    if(!deviceHandle || deviceHandle == INVALID_HANDLE_VALUE) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    /* Update device state in context */
    ptrContext->_internal.connected = 1;  /* Connected */
    ptrContext->_internal.deviceHandle = deviceHandle;  /* Update device handle */

    return DS5W_OK;
}

/* Main function to obtain device input state */
DS5W_ReturnValue getDeviceInputState(DeviceContext* ptrContext, 
									 DS5InputState* ptrInputState)
{
    unsigned short inputReportLength = 0;
	DWORD bytesRead = 0;
	bool resultRF;


    /* Checks whether the input context or state pointer is null */
	if(!ptrContext || !ptrInputState) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    /* Checks if the device is connected */
	if(!ptrContext->_internal.connected) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    /* Clears the input queue (using the HidD_FlushQueue API) */
    HidD_FlushQueue(ptrContext->_internal.deviceHandle);

	if(ptrContext->_internal.deviceHandle == NULL ||
	   ptrContext->_internal.deviceHandle == INVALID_HANDLE_VALUE)
	{
		printf(" \n [!ERROR]: Handle do dispositivo é invalido \n ");
		return DS5W_E_DEVICE_REMOVED;
	}

    /* Determines the length of the input report depending on the connection */
    if(ptrContext->_internal.connection == BT) 
    {
        inputReportLength = 78;
        ptrContext->_internal.hidBuffer[0] = 0x31;  /* Bluetooth command example */
    } 
    else 
    {
        inputReportLength = 64;
        ptrContext->_internal.hidBuffer[0] = 0x01;  /* USB command example */
    }

    /* Attempts to read the device input report */
	resultRF = ReadFile(ptrContext->_internal.deviceHandle, 
						ptrContext->_internal.hidBuffer,
						inputReportLength,
						&bytesRead,
						NULL);
						
	if(!resultRF)
	{
		printf("\n ReadFile Failed! Error code: %d \n", GetLastError());
		CloseHandle(ptrContext->_internal.deviceHandle);
		ptrContext->_internal.deviceHandle = NULL;
		ptrContext->_internal.connected = 0;  /* Mark as disconnected */
		
		return DS5W_E_DEVICE_REMOVED;	
	}

    /* Evaluates the contents of the HID buffer according to the connection type */
    if(ptrContext->_internal.connection == BT) 
    {
        /* Starts from index 2 for Bluetooth */
        evaluateHidInputBuffer(&ptrContext->_internal.hidBuffer[2], ptrInputState); 
    } 
    else 
    {
        /* Starts from index 1 for USB */
        evaluateHidInputBuffer(&ptrContext->_internal.hidBuffer[1], ptrInputState); 
    }

    return DS5W_OK;
}

/* Main function to set the device output state */
DS5W_ReturnValue setDeviceOutputState(DeviceContext*  ptrContext, 
                                      DS5OutputState* ptrOutputState)
{
    unsigned short outputReportLength = 0;
    unsigned int crcChecksum;
	DWORD bytesWritten = 0;
	BOOL resultWF;

    /* Checks if pointers are valid */
    if(!ptrContext || !ptrOutputState) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    /* Checks if the device is connected */
    if(!ptrContext->_internal.connected) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    /* Determines the length of the output report depending on the connection type */
    if(ptrContext->_internal.connection == BT) 
    {
        /* Bluetooth output report is 547 bytes */
        outputReportLength = 547;
    } 
    else 
    {
        /* USB output report is 48 bytes */
        outputReportLength = 48;
    }

    /* Clears the data in the buffer */
    memset(ptrContext->_internal.hidBuffer, 0, outputReportLength);

    /* Fills the output buffer depending on the connection type */
    if(ptrContext->_internal.connection == BT) 
    {
        /* Report Type for Bluetooth */
        ptrContext->_internal.hidBuffer[0x00] = 0x31;  /* Type identifier for BT */
        ptrContext->_internal.hidBuffer[0x01] = 0x02;  /* Sub-type identifier */
        createHidOutputBuffer(&ptrContext->_internal.hidBuffer[2], ptrOutputState);

        /* Calculates buffer CRC for Bluetooth */
        crcChecksum = computeCRC32(ptrContext->_internal.hidBuffer, 74);

        /* Fills the 4 bytes of CRC checksum in the buffer */
        ptrContext->_internal.hidBuffer[0x4A] = (unsigned char)((crcChecksum & 0x000000FF) >> 0);
        ptrContext->_internal.hidBuffer[0x4B] = (unsigned char)((crcChecksum & 0x0000FF00) >> 8);
        ptrContext->_internal.hidBuffer[0x4C] = (unsigned char)((crcChecksum & 0x00FF0000) >> 16);
        ptrContext->_internal.hidBuffer[0x4D] = (unsigned char)((crcChecksum & 0xFF000000) >> 24);
    } 
    else 
    {
        /* Report Type for USB */
        ptrContext->_internal.hidBuffer[0x00] = 0x02;  /* Type identifier for USB */
        createHidOutputBuffer(&ptrContext->_internal.hidBuffer[1], ptrOutputState);  /* Fill buffer from [1] for USB */
    }
	
	if(ptrContext->_internal.deviceHandle == NULL || 
	   ptrContext->_internal.deviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("\n [!ERROR]: Device Handle is invalid \n ");
		return DS5W_E_DEVICE_REMOVED;
	}

	resultWF = WriteFile(ptrContext->_internal.deviceHandle, 
						 ptrContext->_internal.hidBuffer, 
						 outputReportLength, 
						 &bytesWritten, 
						 NULL);

	if(!resultWF)
	{
		printf("\n [!ERROR]: WriteFile Failed !, Error code: %d\n", GetLastError());
		CloseHandle(ptrContext->_internal.deviceHandle);
		ptrContext->_internal.deviceHandle = NULL;
		ptrContext->_internal.connected = 0;  /* Mark as disconnected */
	
		/* Returns device removed error */
		return DS5W_E_DEVICE_REMOVED; 
	}

    /* If everything goes well, success returns */
    return DS5W_OK;
}

