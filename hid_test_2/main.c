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

*/


/*
void list_hid_devices() 
{
    HDEVINFO deviceInfoSet;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    DWORD index = 0;
    GUID hidGuid;

    HidD_GetHidGuid(&hidGuid); // Obtém o GUID do HID
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

        // Obtém o tamanho necessário para deviceDetail
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL)) 
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
            {
                printf(" - Dispositivo HID %lu: Erro ao obter tamanho necessário (código %lu)\n", index, GetLastError());
                index++; // Continua para o próximo dispositivo
                continue;
            }
        }

        // Aloca memória corretamente alinhada
        deviceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED*)malloc(requiredSize);
        if(!deviceDetail) 
        {
            printf("Erro de alocação de memoria.\n");
            break;
        }
        deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED);

        // Obtém os detalhes do dispositivo
        if(!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, (SP_DEVICE_INTERFACE_DETAIL_DATA*)deviceDetail, requiredSize, NULL, &devInfoData)) {
            printf(" - Dispositivo HID %lu: Erro ao obter detalhes do dispositivo (código %lu)\n", index, GetLastError());
            free(deviceDetail);
            index++; // Continua para o próximo dispositivo
            continue;
        }

        // Obtém o nome amigável do dispositivo
        if(SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)deviceName, sizeof(deviceName), NULL) ||
           SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)deviceName, sizeof(deviceName), NULL)) 
        {
            printf(" - Dispositivo HID %lu: %s\n", index, deviceName);
        } 
        else 
        {
            printf(" - Dispositivo HID %lu: Nome nao disponivel (codigo %lu)\n", index, GetLastError());
        }

        // Obtém o Hardware ID para extrair VID e PID
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareID, sizeof(hardwareID), NULL)) 
        {
            printf("   -> Hardware ID: %s\n", hardwareID);

            // Verifica se é o DualSense 5 (VID 054C, PID 0CE6 ou 0DF2)
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

#define MAX_DEVICES 8 /* Maximum number of devices to be stored */


int main() 
{
    //list_hid_devices();
    
    
    DeviceEnumInfo devices[MAX_DEVICES]; /* Array to store the devices */
    unsigned int requiredLength = 0;
    DeviceContext con;
    DS5W_ReturnValue resultDeviceContext;

    DS5W_ReturnValue result = enumDevices(devices, MAX_DEVICES, &requiredLength, 1);

    if(requiredLength == 0) 
    {
        printf("\n No devices found. <[^~~^]> \n");
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
        /* Color intentsity */
        float intensity = 1.0f;
        uint16_t lrmbl = 0.0;
        uint16_t rrmbl = 0.0;
        /* Force */
        TriggerEffectType rType = NoResitance;
        int btMul = con._internal.connection == BT ? 10 : 1;
        
        printf(" \n DualSense controller connected \n");

        memset(&inState, 0, sizeof(DS5InputState));
        memset(&outState, 0, sizeof(DS5OutputState));

        while(!(inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_BUMPER && inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_BUMPER))
        {
            DS5W_ReturnValue DeviceInputState;
            DeviceInputState = getDeviceInputState(&con, &inState);
            
            if(DeviceInputState == OK)
            {
				/* === Read Input ===
                  Build all universal buttons (USB and BT) as text */
				
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
                    outState.playerLeds.bitmask = DS5W_OSTATE_PLAYER_LED_MIDDLE;
                    outState.playerLeds.brightness = LED_BRIGHTNESS_HIGH;
                }
                else 
                {
                    outState.playerLeds.bitmask = 0;
                }
				
				
                /* Set force */
                if(inState.rightTrigger == 0xFF) 
                {
                    rType = ContinuousResitance;
                } 
                else if(inState.rightTrigger == 0x00) 
                {
                    rType = NoResitance;
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
				
				
                /* Left trigger is clicky / section */
                outState.leftTriggerEffect.effectType = SectionResitance;
                outState.leftTriggerEffect.Section.startPosition = 0x00;
                outState.leftTriggerEffect.Section.endPosition = 0x60;
				
                /* Right trigger is forcy */
                outState.rightTriggerEffect.effectType = rType;
                outState.rightTriggerEffect.Continuous.force = 0xFF;
                outState.rightTriggerEffect.Continuous.startPosition = 0x00;                
				
                setDeviceOutputState(&con, &outState);
				
				
				
			}
			else
			{
				/* Device disconnected show error and try to reconnect */
                printf("\nDevice removed!\n");
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





