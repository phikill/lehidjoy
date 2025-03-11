#include<windows.h>
#include<setupapi.h>
#include<devguid.h>
#include<initguid.h>
#include<hidsdi.h>

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>


/* Info Sony Joys

Vendor ID (VID) e Product ID (PID):

DualShock 4: VID: 0x054C (Sony) PID: 0x05C4 (CUH-ZCT1x) Before 2016
DualShock 4: VID: 0x054C (Sony) PID: 0x09CC (CUH-ZCT2x) After  2016
DualSense 5: VID: 0x054C (Sony) PID: 0x0CE6 (Basic DS5)
DualSense 5: VID: 0x054C (Sony) PID: 0x0DF2 (DualSense Edge)

*/


/*
void list_hid_devices() 
{
    HDEVINFO deviceInfoSet;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    DWORD index = 0;
    GUID hidGuid;

    HidD_GetHidGuid(&hidGuid); /* Obtém o GUID do HID *
    deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    
    if (deviceInfoSet == INVALID_HANDLE_VALUE) 
    {
        printf("Erro ao obter lista de dispositivos HID.\n");
        return;
    }

    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    printf("Dispositivos HID detectados:\n");

    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &hidGuid, index, &deviceInterfaceData)) 
    {
        DWORD requiredSize = 0;
        SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED *deviceDetail;
        SP_DEVINFO_DATA devInfoData;
        TCHAR deviceName[256];
        TCHAR hardwareID[256];

        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        /* Obtém o tamanho necessário para deviceDetail *
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL)) 
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
            {
                printf(" - Dispositivo HID %lu: Erro ao obter tamanho necessário (código %lu)\n", index, GetLastError());
                index++; /* Continua para o próximo dispositivo *
                continue;
            }
        }

        /* Aloca memória corretamente alinhada *
        deviceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED*)malloc(requiredSize);
        if(!deviceDetail) 
        {
            printf("Erro de alocação de memoria.\n");
            break;
        }
        deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED);

        /* Obtém os detalhes do dispositivo *
        if(!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, (SP_DEVICE_INTERFACE_DETAIL_DATA*)deviceDetail, requiredSize, NULL, &devInfoData)) {
            printf(" - Dispositivo HID %lu: Erro ao obter detalhes do dispositivo (código %lu)\n", index, GetLastError());
            free(deviceDetail);
            index++; /* Continua para o próximo dispositivo *
            continue;
        }

        /* Obtém o nome amigável do dispositivo *
        if(SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)deviceName, sizeof(deviceName), NULL) ||
           SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)deviceName, sizeof(deviceName), NULL)) 
        {
            printf(" - Dispositivo HID %lu: %s\n", index, deviceName);
        } 
        else 
        {
            printf(" - Dispositivo HID %lu: Nome nao disponivel (codigo %lu)\n", index, GetLastError());
        }

        /* Obtém o Hardware ID para extrair VID e PID *
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareID, sizeof(hardwareID), NULL)) 
        {
            printf("   -> Hardware ID: %s\n", hardwareID);

            /* Verifica se é o DualSense 5 (VID 054C, PID 0CE6 ou 0DF2) *
            if (strstr(hardwareID, "VID_054C") && (strstr(hardwareID, "PID_0CE6") || strstr(hardwareID, "PID_0DF2"))) 
            {
                printf(" \n ============================================ \n");
                printf("   ** Este dispositivo e um DualSense 5! **");
                printf(" \n ============================================ \n");
            }
        }

        free(deviceDetail);
        index++;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}*/




#include"lehidjoy/lehidjoy.h"


/* Function to display the bytes of the outState structure */
void printOutStateHex(const DS5OutputState *outState) 
{
    size_t i = 0;
    unsigned char *rawData = (unsigned char *)outState;
    size_t dataSize = sizeof(DS5OutputState);

    printf("Dump de outState (%zu bytes):\n", dataSize);
    for(i = 0; i < dataSize; i++) 
    {
        printf("%02X ", rawData[i]);  /* Print each byte in hexadecimal */
        if ((i + 1) % 8 == 0) printf("\n"); /* Line break every 8 bytes */
    }
    printf("\n");
}


void check_sizes() 
{
    printf("Size of TriggerEffect: %lu bytes\n", (unsigned long)sizeof(TriggerEffect));
    printf("Size of AnalogStick: %zu bytes\n", sizeof(AnalogStick));
    printf("Size of Vec3: %zu bytes\n", sizeof(Vec3));
    printf("Size of Color: %zu bytes\n", sizeof(Color));
    printf("Size of Touch: %zu bytes\n", sizeof(Touch));
    printf("Size of Battery: %zu bytes\n", sizeof(Battery));
    printf("Size of MicLed: %zu bytes\n", sizeof(MicLed));
    printf("Size of TriggerEffectType: %zu bytes\n", sizeof(TriggerEffectType));
    printf("Size of TriggerEffect: %zu bytes\n", sizeof(TriggerEffect));
    printf("Size of LedBrightness: %zu bytes\n", sizeof(LedBrightness));
    printf("Size of PlayerLeds: %zu bytes\n", sizeof(PlayerLeds));
    printf("Size of DS5InputState: %zu bytes\n", sizeof(DS5InputState));
    printf("Size of DS5OutputState: %zu bytes\n", sizeof(DS5OutputState));
}

#define MAX_DEVICES 5 /* Maximum number of devices to be stored */


int main(int argc, char *argv[]) 
{
    /* list_hid_devices(); */
    
    
    DeviceEnumInfo devices[MAX_DEVICES]; /* Array to store the devices */
    unsigned int requiredLength = 0;
    DeviceContext con;
    DS5W_ReturnValue resultDeviceContext;

    DS5W_ReturnValue result = enumDevices(devices, MAX_DEVICES, &requiredLength, 1);

    if(requiredLength == 0) 
    {
        printf("\n No devices found. <[^~~^]> \n");
        system("pause");
        return -15;
    }

    /* Check return */
    if(result == DS5W_OK)
    {
        unsigned int i = 0;

        printf("Devices detected (%d found):\n", requiredLength);
        for(i = 0; i < requiredLength; i++) 
        {
            printf(" \n =============== \n");
            printf("Device %d:\n", i + 1);
            printf("  Path: %s\n", devices[i]._internal.path);
            if(devices[i]._internal.connection == BT)
            {
                printf(" \n Connection Bluetooth \n");
            }
            else if(devices[i]._internal.connection == (BT | USB))
            {
                printf(" \n Connection Bluetooth & chargin \n");
            }
            else
            {
                printf(" \n Connection USB \n");
            }

        }
    } 
    else if(result == DS5W_E_INSUFFICIENT_BUFFER)
    {
        printf("\n Insufficient buffer. At least %d entries are required.\n ", requiredLength);
    } 
    else 
    {
        printf("Error listing devices (Error Code: %d).\n", result);
    }

    resultDeviceContext = initDeviceContext(&devices[0], &con);

    if(resultDeviceContext == OK)
    {
        DS5InputState inState;
        DS5OutputState outState;
        /* Color intensity */
        float intensity = 1.0f;
        uint16_t lrmbl = 0.0;
        uint16_t rrmbl = 0.0;
        /* Force */
        TriggerEffectType rType = NoResitance;
        int btMul = con._internal.connection == BT ? 10 : 1;
        
        char title[256] = "DS5 ("; /* Start With "DS5 (" */

        printf(" \n DualSense controller connected \n");
        system("pause");

        /* Add "BT" or "USB" */
        if (con._internal.connection == BT)
        {
            strcat(title, "BT");
        } 
        else 
        {
            strcat(title, "USB");
        }

        strcat(title, ") Press L1 and R1 to exit");

        /* Sets the console title */
        SetConsoleTitleA(title);


        memset(&inState, 0, sizeof(DS5InputState));
        memset(&outState, 0, sizeof(DS5OutputState));

        while(!(inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_BUMPER && 
                inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_BUMPER))
        {
            DS5W_ReturnValue DeviceInputState;
            DeviceInputState = getDeviceInputState(&con, &inState);
            
            if(DeviceInputState == OK)
            {
				/* === Read Input ===
                  Build all universal buttons (USB and BT) as text */
				
                printf("\n \n \n \n \n \n \n \n \n \n \n \n \n \n \n");

				printf("Left Stick\tX: %d\tY: %d%s\n", (int)inState.leftStick.x, 
                                                       (int)inState.leftStick.y, 
                (inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_STICK ? "\tPUSH" : ""));

                printf("Right Stick\tX: %d\tY: %d%s\n\n", (int)inState.rightStick.x, 
                                                          (int)inState.rightStick.y, 
                (inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_STICK ? "\tPUSH" : ""));
    
                printf("Left Trigger:  %d\tBinary active: %s%s\n", (int)inState.leftTrigger, 
                    (inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_TRIGGER ? "Yes" : "No"), 
                    (inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_BUMPER ? "\tBUMPER" : ""));

                printf("Right Trigger: %d\tBinary active: %s%s\n\n", (int)inState.rightTrigger, 
                    (inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_TRIGGER ? "Yes" : "No"), 
                    (inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_BUMPER ? "\tBUMPER" : ""));

                printf("DPAD: %s%s%s%s\tButtons: %s%s%s%s\n",
                    (inState.buttonsAndDpad & DS5W_ISTATE_DPAD_LEFT ? "L " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_DPAD_UP ? "U " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_DPAD_DOWN ? "D " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_DPAD_RIGHT ? "R " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_BTX_SQUARE ? "S " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_BTX_CROSS ? "X " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_BTX_CIRCLE ? "O " : "  "),
                    (inState.buttonsAndDpad & DS5W_ISTATE_BTX_TRIANGLE ? "T " : "  "));

                printf("%s%s\n",
                    (inState.buttonsA & DS5W_ISTATE_BTN_A_MENU ? "MENU" : ""),
                    (inState.buttonsA & DS5W_ISTATE_BTN_A_SELECT ? "\tSELECT" : ""));

                printf("Trigger Feedback:\tLeft: %d\tRight: %d\n\n",
                    (int)inState.leftTriggerFeedback, 
                    (int)inState.rightTriggerFeedback);

                printf("Touchpad%s:\n",
                    (inState.buttonsB & DS5W_ISTATE_BTN_B_PAD_BUTTON ? " (pushed)" : ""));

                printf("Finger 1\tX: %d\tY: %d\n",
                    inState.touchPoint1.x, inState.touchPoint1.y);
    
                printf("Finger 2\tX: %d\tY: %d\n\n",
                    inState.touchPoint2.x, inState.touchPoint2.y);

                printf("Battery: %d%s%s\n\n", inState.battery.level,
                    (inState.battery.chargin ? " Charging" : ""),
                    (inState.battery.fullyCharged ? "  Fully charged" : ""));

                printf("%s%s\n",
                    (inState.buttonsB & DS5W_ISTATE_BTN_B_PLAYSTATION_LOGO ? "PLAYSTATION" : ""),
                    (inState.buttonsB & DS5W_ISTATE_BTN_B_MIC_BUTTON ? "\tMIC" : ""));
				
				/* === Write Output ===
                   Rumbel 
                */
                lrmbl = max(lrmbl - 0x200 / btMul, 0);
                rrmbl = max(rrmbl - 0x100 / btMul, 0);
				
				
                outState.leftRumble = (lrmbl & 0xFF00) >> 8UL;
                outState.rightRumble = (rrmbl & 0xFF00) >> 8UL;

				
                /* Lightbar */
                outState.lightbar = color_R8G8B8_UCHAR_A32_FLOAT(0, 255, 0, intensity);
                intensity -= 0.0025f / btMul;
                if(intensity <= 0.0f) 
                {
                    intensity = 1.0f;

                    lrmbl = 0xFF00;
                    rrmbl = 0xFF00;
                }
				
                /* Player led */
                if(outState.rightRumble) 
                {
                    outState.playerLeds.playerLedFade = true;
                    outState.playerLeds.bitmask = HIDJOY_LED_PLAYER_1;
                    outState.playerLeds.brightness = 0x00;
                }
                else 
                {
                    outState.playerLeds.bitmask = 0;
                }
				
                /* Mic led */
                if(inState.buttonsB & DS5W_ISTATE_BTN_B_MIC_BUTTON) 
                {
                    outState.microphoneLed = ON;
                }
                else if(inState.buttonsB & DS5W_ISTATE_BTN_B_PLAYSTATION_LOGO) 
                {
                    outState.microphoneLed = OFF;
                }
				
                /* Set force */
                if(inState.rightTrigger == 0xFF) 
                {
                    printf("\n rightTrigger MAX ! =============================== \n");
                    rType = ContinuousResitance; /* = ContinuousResitance */
                } 
                else if(inState.rightTrigger == 0x00) 
                {
                    rType = NoResitance; /* = NoResitance */
                }
				
                /* Left trigger is clicky / section */
                outState.leftTriggerEffect.effectType = SectionResitance;
                outState.leftTriggerEffect.Section.startPosition = 0x00;
                outState.leftTriggerEffect.Section.endPosition = 0x60;
				
                /* Right trigger is forcy */
                outState.rightTriggerEffect.effectType = rType;
                outState.rightTriggerEffect.Continuous.force = 0xFF;
                outState.rightTriggerEffect.Continuous.startPosition = 0x00;                
				
                
                /* 
                printOutStateHex(&outState);
                check_sizes();
                */


                setDeviceOutputState(&con, &outState); /* Send Data to Device */
				
				
				
			}
			else
			{
				/* Device disconnected show error and try to reconnect */
                printf("\nDevice removed! \n Reconnect Device Again !\n");
                reconnectDevice(&con);
			}
        }
		/* Free state */
        freeDeviceContext(&con);
    }
    else
    {
        printf(" \n [!ERROR]: Function error initDeviceContext() \n");
        printf("\n  Failed to connect to controller! \n");
        system("pause");
        return -1;
    }
    
    system("pause");
    return 0;
}

