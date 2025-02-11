#include<windows.h>
#include<setupapi.h>
#include<devguid.h>
#include<initguid.h>
#include<hidsdi.h>

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>


/* Info DualShock 5 

Vendor ID (VID) e Product ID (PID):

O DualSense 5 geralmente tem:
VID: 0x054C (Sony)
PID: 0x0CE6 (DualSense USB)
PID: 0x0DF2 (DualSense Bluetooth)


*/

#pragma library("setupapi.lib")
//#pragma library("hid.lib")

// Estrutura alinhada manualmente para evitar erro 1784 no Watcom
#pragma pack(push, 1) 
typedef struct 
{
    DWORD cbSize;
    char DevicePath[1]; // Primeiro caractere do caminho
} SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED;
#pragma pack(pop)


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
}

/*
    enumDevices()

Essa função é responsável por enumerar os dispositivos HID conectados ao sistema. Aqui está um resumo do que ela faz:

Verificação de buffer: Ela verifica se o buffer de entrada (ptrBuffer) está correto e se o tamanho do array (inArrLength) é válido.

Enumeração de dispositivos HID: Utiliza a função SetupDiGetClassDevs para obter a lista de dispositivos HID conectados e começa a iterar sobre os dispositivos.

Verificação do tipo de dispositivo: Para cada dispositivo, verifica se é um dispositivo HID com o GUID correspondente e tenta obter o caminho do dispositivo.

Conexão e identificação do dispositivo: Tenta abrir o dispositivo e verificar se ele é um controle DS5 (vendo o VendorID e ProductID), então coleta informações sobre a conexão (USB ou Bluetooth).

Armazenamento das informações: Caso um controle DS5 seja encontrado, suas informações são armazenadas em um buffer fornecido como parâmetro.

Retorno: Retorna o número de dispositivos encontrados ou um erro caso o buffer seja pequeno demais ou ocorra outro erro durante a execução.
*/

#include"lehidjoy/device.h"
#include"lehidjoy/joyState.h"
#include"lehidjoy/hidjoy_api.h"
#include"lehidjoy/DS_CRC32.h"
#include"lehidjoy/hidjoy_input.h"
#include"lehidjoy/Helpers.h"


// DS5_OUTPUT

void createHidOutputBuffer(unsigned char* hidOutBuffer, 
                           DS5OutputState* ptrOutputState) 
{
    // Feature mask
    hidOutBuffer[0x00] = 0xFF;
    hidOutBuffer[0x01] = 0xF7;

    // Rumble motors
    hidOutBuffer[0x02] = ptrOutputState->rightRumble;
    hidOutBuffer[0x03] = ptrOutputState->leftRumble;

    // Mic led
    hidOutBuffer[0x08] = (unsigned char)ptrOutputState->microphoneLed;

    // Player led
    hidOutBuffer[0x2B] = ptrOutputState->playerLeds.bitmask;
    if (ptrOutputState->playerLeds.playerLedFade) 
    {
        hidOutBuffer[0x2B] &= ~(0x20);
    }
    else 
    {
        hidOutBuffer[0x2B] |= 0x20;
    }

    // Player led brightness
    hidOutBuffer[0x26] = 0x03;
    hidOutBuffer[0x29] = ptrOutputState->disableLeds ? 0x01 : 0x2;
    hidOutBuffer[0x2A] = ptrOutputState->playerLeds.brightness;

    // Lightbar
    hidOutBuffer[0x2C] = ptrOutputState->lightbar.r;
    hidOutBuffer[0x2D] = ptrOutputState->lightbar.g;
    hidOutBuffer[0x2E] = ptrOutputState->lightbar.b;

    // Adaptive Triggers
    processTrigger(&ptrOutputState->leftTriggerEffect, &hidOutBuffer[0x15]);
    processTrigger(&ptrOutputState->rightTriggerEffect, &hidOutBuffer[0x0A]);
}

void processTrigger(TriggerEffect* ptrEffect, unsigned char* buffer) 
{
    // Switch on effect
    switch (ptrEffect->EffectEx.startPosition) { // Change condition to fit C89
        // Continious
        case 1:
            buffer[0x00] = 0x01;
            buffer[0x01] = ptrEffect->Continuous.startPosition;
            buffer[0x02] = ptrEffect->Continuous.force;
            break;

        // Section
        case 2:
            buffer[0x00] = 0x02;
            buffer[0x01] = ptrEffect->Continuous.startPosition;
            buffer[0x02] = ptrEffect->Continuous.force;
            break;

        // EffectEx
        case 3:
            buffer[0x00] = 0x02 | 0x20 | 0x04;
            buffer[0x01] = 0xFF - ptrEffect->EffectEx.startPosition;
            if (ptrEffect->EffectEx.keepEffect) {
                buffer[0x02] = 0x02;
            }
            buffer[0x04] = ptrEffect->EffectEx.beginForce;
            buffer[0x05] = ptrEffect->EffectEx.middleForce;
            buffer[0x06] = ptrEffect->EffectEx.endForce;
            buffer[0x09] = max(1, ptrEffect->EffectEx.frequency / 2);
            break;

        // Calibrate
        case 4:
            buffer[0x00] = 0xFC;
            break;

        // No resistance / default
        default:
            buffer[0x00] = 0x00;
            buffer[0x01] = 0x00;
            buffer[0x02] = 0x00;
            break;
    }
}

// DS5_OUTPUT


DS5W_ReturnValue enumDevices(void *ptrBuffer, 
                             unsigned int inArrLength, 
                             unsigned int *requiredLength, 
                             int pointerToArray) 
{
    HDEVINFO hidDiHandle;
    //SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    SP_DEVINFO_DATA hidDiInfo;
    DWORD devIndex = 0;
    GUID hidGuid;
    unsigned int inputArrIndex = 0;
    bool inputArrOverflow = false;
    
    // Check for invalid non expected buffer
    if(inArrLength && !ptrBuffer) 
    {
        inArrLength = 0;
    }

    HidD_GetHidGuid(&hidGuid);
    hidDiHandle = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if(!hidDiHandle || (hidDiHandle == INVALID_HANDLE_VALUE)) 
    {
        // Erro ao obter lista de dispositivos HID

        printf(" \n Erro ao obter a lista de dispositivos HID\n"); 
        //return -1;
        return DS5W_E_EXTERNAL_WINAPI; // = 3
    }

    hidDiInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    
    printf(" \n [= DEBUG =] Comecando a enumeracao de dispositivos...\n\n");

    while(SetupDiEnumDeviceInfo(hidDiHandle, devIndex, &hidDiInfo))
    {
        DWORD ifIndex = 0;
        SP_DEVICE_INTERFACE_DATA ifDiInfo;

        //printf("Dispositivo %d encontrado.\n", devIndex);
        
        ifDiInfo.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        // Loop interno para enumerar as interfaces do dispositivo atual
        while (SetupDiEnumDeviceInterfaces(hidDiHandle, 
                                           &hidDiInfo, 
                                           &hidGuid, 
                                           ifIndex, 
                                           &ifDiInfo)) 
        {
            DWORD requiredSize = 0;
            SP_DEVICE_INTERFACE_DETAIL_DATA *deviceDetail;
            SP_DEVINFO_DATA devInfoData;

            HANDLE deviceHandle;


            memset(&devInfoData, 0, sizeof(SP_DEVINFO_DATA));
            devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);


            // Obtém o tamanho necessário para a estrutura de detalhes
            SetupDiGetDeviceInterfaceDetail(hidDiHandle, &ifDiInfo, NULL, 0, &requiredSize, NULL);

            // Check size
            if (requiredSize > (260 * sizeof(wchar_t))) 
            {
                SetupDiDestroyDeviceInfoList(hidDiHandle);
                return DS5W_E_EXTERNAL_WINAPI;
            }

            // Allocate memory for path on the stack
            // Aloca memória para os detalhes da interface
            //deviceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
            //deviceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize + sizeof(DWORD));
            deviceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED*)malloc(requiredSize + sizeof(DWORD));
            if(!deviceDetail) 
            {
                printf("Erro ao alocar memória.\n");
                //SetupDiDestroyDeviceInfoList(hidDiHandle);
                return DS5W_E_STACK_OVERFLOW;
                //break;
            }

            // Get device path
            // Obtém os detalhes da interface
            
            //deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_ALIGNED);

            //deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256; 

            //printf("Tamanho necessario para deviceDetail: %lu bytes\n", requiredSize);

            if(SetupDiGetDeviceInterfaceDetail(hidDiHandle, &ifDiInfo, deviceDetail, requiredSize, NULL, NULL)) 
            {
                //printf("  Interface %d: %s\n", ifIndex, deviceDetail->DevicePath);
            }
            else
            {
                DWORD error = GetLastError();
                printf(" \n [!ERROR]: SetupDiGetDeviceInterfaceDetail falhou com erro %lu \n", error);
            }

            //SetupDiGetDeviceInterfaceDetail(hidDiHandle, &ifDiInfo, deviceDetail, requiredSize, NULL, NULL);

            // Check if input array has space
            // Check if device is reachable

            deviceHandle = CreateFileA(deviceDetail->DevicePath, 
                                       GENERIC_READ | GENERIC_WRITE, 
                                       FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                       NULL, 
                                       OPEN_EXISTING, 
                                       0, // NULL
                                       NULL);

            // Check if device is reachable
            if(deviceHandle && (deviceHandle != INVALID_HANDLE_VALUE)) 
            {
                // Get vendor and product id
                unsigned int vendorId = 0;
                unsigned int productId = 0;
                HIDD_ATTRIBUTES deviceAttributes;
                PHIDP_PREPARSED_DATA ppd;
                

                if(HidD_GetAttributes(deviceHandle, &deviceAttributes)) 
                {
                    vendorId = deviceAttributes.VendorID;
                    productId = deviceAttributes.ProductID;
                }

                // Check if ids match
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
                
                    // Copy path
                    if(ptrInfo) 
                    {
                        wcscpy_s(ptrInfo->_internal.path, 260, (const wchar_t*)deviceDetail->DevicePath);
                    }

                    // Get preparsed data
                    if(HidD_GetPreparsedData(deviceHandle, &ppd)) 
                    {
                        // Get device capcbilitys
                        HIDP_CAPS deviceCaps;
                        if(HidP_GetCaps(ppd, &deviceCaps) == HIDP_STATUS_SUCCESS) 
                        {
                            // Check for device connection type
                            if(ptrInfo) 
                            {
                                // Check if controller matches USB specifications
                                if (deviceCaps.InputReportByteLength == 64) 
                                {
                                    ptrInfo->_internal.connection = USB;

                                    // Device found and valid -> Inrement index
                                    inputArrIndex++;
                                }
                                // Check if controler matches BT specifications
                                else if(deviceCaps.InputReportByteLength == 78) 
                                {
                                    ptrInfo->_internal.connection = BT;

                                    // Device found and valid -> Inrement index
                                    inputArrIndex++;
                                }
                            }
                        }

                        // Free preparsed data
                        HidD_FreePreparsedData(ppd);
                    }
                }

                // Close device
                CloseHandle(deviceHandle); 
            }
            else
            {
                //printf(" [!ERROR]: deviceHandle Error \n \n");
            }

            ifIndex++; // Increment index

            free(deviceDetail); // Free device from mem
        }

        if(GetLastError() != ERROR_NO_MORE_ITEMS) 
        {
            printf("Erro ao enumerar interfaces.\n");
        }

        devIndex++; // Increment index
    } 

    // Verifica se chegou ao final da lista
    if (GetLastError() != ERROR_NO_MORE_ITEMS) 
    {
        printf("Erro ao enumerar dispositivos.\n");
    }
    
    SetupDiDestroyDeviceInfoList(hidDiHandle);
    
    // Set required size if exists
    if(requiredLength) 
    {
        *requiredLength = inputArrIndex;
    }
    
    // Check if array was suficient
    if(inputArrIndex <= inArrLength) 
    {
        return DS5W_OK; // DS5W_OK
    }
    else 
    {
        return DS5W_E_INSUFFICIENT_BUFFER; // DS5W_E_INSUFFICIENT_BUFFER
    }
}

#define DEVICE_PATH_MAX 260

DS5W_ReturnValue initDeviceContext(DeviceEnumInfo* ptrEnumInfo, 
                                   DeviceContext* ptrContext) 
{
    HANDLE deviceHandle;
    unsigned short reportLength = 0;
    //unsigned char fBuffer[64];


    // Verificar se os ponteiros são válidos
    if(!ptrEnumInfo || !ptrContext) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    /*
    // Verificar o comprimento de len (em C89, wcscmp e wcslen são usados para strings wide)
    if(wcslen((wchar_t)ptrEnumInfo->_internal.path) == 0) 
    {
        return DS5W_E_INVALID_ARGS;
    }*/

    /* Verifica se o caminho do dispositivo é válido */
    if(strlen(ptrEnumInfo->_internal.path) == 0) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    // Conectar ao dispositivo
    deviceHandle = CreateFileA(ptrEnumInfo->_internal.path,  // Usando `CreateFileA` para ANSI
                              GENERIC_READ | GENERIC_WRITE, 
                              FILE_SHARE_READ | FILE_SHARE_WRITE, 
                              NULL, 
                              OPEN_EXISTING, 
                              0,  // Em C89, NULL pode ser 0
                              NULL);

    if(deviceHandle == INVALID_HANDLE_VALUE || deviceHandle == NULL) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    // Escrever no contexto
    ptrContext->_internal.connected = 1;
    ptrContext->_internal.connection = ptrEnumInfo->_internal.connection;
    ptrContext->_internal.deviceHandle = deviceHandle;
    //wcscpy((wchar_t)ptrContext->_internal.devicePath, (wchar_t)ptrEnumInfo->_internal.path);  // Substitui wcscpy_s
    strncpy(ptrContext->_internal.devicePath, ptrEnumInfo->_internal.path, DEVICE_PATH_MAX - 1);
    ptrContext->_internal.devicePath[DEVICE_PATH_MAX - 1] = '\0'; /* Garante terminação */

    // Obter o comprimento do relatório de entrada
    if(ptrContext->_internal.connection == BT) {  // Se `2` corresponde a BT (ajuste conforme necessário)
        // Começar o BT lendo o relatório de características 5
        unsigned char fBuffer[64];
        fBuffer[0] = 0x05;
        if(!HidD_GetFeature(deviceHandle, fBuffer, 64)) 
        {
            return DS5W_E_BT_COM;
        }

        // O relatório de entrada Bluetooth tem 78 bytes de comprimento
        reportLength = 547;
    } 
    else 
    {
        // O relatório USB tem 64 bytes de comprimento
        reportLength = 64;
    }

    // Retornar OK
    return DS5W_OK;
}

DS5W_ReturnValue setDeviceOutputState(DeviceContext* ptrContext, 
                                      DS5OutputState* ptrOutputState);
 
void freeDeviceContext(DeviceContext* ptrContext) 
{
    DS5OutputState os;

    // Verificar se o dispositivo está presente
    if(ptrContext->_internal.deviceHandle) 
    {
        // Enviar relatório de saída zero para desabilitar todos os efeitos ativos
        memset(&os, 0, sizeof(DS5OutputState));  // Zera a estrutura de estado

        // Ajustar os gatilhos para o tipo "Sem resistência"
        os.leftTriggerEffect.effectType = NoResitance;
        os.rightTriggerEffect.effectType = NoResitance;
        os.disableLeds = 1;  // Definir para TRUE em C89, que é 1

        // Atualizar estado do dispositivo
        setDeviceOutputState(ptrContext, &os);

        // Fechar o handle do dispositivo
        CloseHandle(ptrContext->_internal.deviceHandle);
        ptrContext->_internal.deviceHandle = NULL;  // Ajuste em C89 (NULL é normalmente 0)
    }

    // Desconectar
    ptrContext->_internal.connected = 0;  // False é representado por 0 no C89

    // Zerar a string de caminho do dispositivo
    ptrContext->_internal.devicePath[0] = '\0';  // Configurar como string vazia
}

// A função que re-conecta o dispositivo
DS5W_ReturnValue reconnectDevice(DeviceContext* ptrContext)
{
    HANDLE deviceHandle;

    // Verificar se o caminho do dispositivo não está vazio
    if (strlen(ptrContext->_internal.devicePath) == 0) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    // Conectar ao dispositivo
    deviceHandle = CreateFileA(ptrContext->_internal.devicePath, 
                               GENERIC_READ | GENERIC_WRITE, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE, 
                               NULL, 
                               OPEN_EXISTING, 
                               0,  // Flags
                               NULL);

    // Verificar se o dispositivo foi conectado corretamente
    if (!deviceHandle || deviceHandle == INVALID_HANDLE_VALUE) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    // Atualizar o estado do dispositivo no contexto
    ptrContext->_internal.connected = 1;  // Conectado
    ptrContext->_internal.deviceHandle = deviceHandle;  // Atualizar handle do dispositivo

    // Retornar sucesso
    return DS5W_OK;
}

// Função principal para obter o estado de entrada do dispositivo
DS5W_ReturnValue getDeviceInputState(DeviceContext* ptrContext, DS5InputState* ptrInputState)
{
    unsigned short inputReportLength = 0;

    // Verifica se o ponteiro do contexto ou do estado de entrada são nulos
    if (!ptrContext || !ptrInputState) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    // Verifica se o dispositivo está conectado
    if (!ptrContext->_internal.connected) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    // Limpa a fila de entrada (usando a API HidD_FlushQueue)
    HidD_FlushQueue(ptrContext->_internal.deviceHandle);

    // Determina o comprimento do relatório de entrada dependendo da conexão
    if (ptrContext->_internal.connection == BT) 
    {
        inputReportLength = 78;
        ptrContext->_internal.hidBuffer[0] = 0x31;  // Exemplo de comando para Bluetooth
    } 
    else 
    {
        inputReportLength = 64;
        ptrContext->_internal.hidBuffer[0] = 0x01;  // Exemplo de comando para USB
    }

    // Tenta ler o relatório de entrada do dispositivo
    if (!ReadFile(ptrContext->_internal.deviceHandle, ptrContext->_internal.hidBuffer, inputReportLength, NULL, NULL)) 
    {
        // Caso falhe, fecha o handle e marca o dispositivo como desconectado
        CloseHandle(ptrContext->_internal.deviceHandle);
        ptrContext->_internal.deviceHandle = NULL;
        ptrContext->_internal.connected = 0;  // Marca como desconectado

        // Retorna erro de dispositivo removido
        return DS5W_E_DEVICE_REMOVED;
    }

    // Avalia o conteúdo do buffer HID de acordo com o tipo de conexão
    if (ptrContext->_internal.connection == BT) 
    {
        evaluateHidInputBuffer(&ptrContext->_internal.hidBuffer[2], ptrInputState);  // Começa do índice 2 para Bluetooth
    } 
    else 
    {
        evaluateHidInputBuffer(&ptrContext->_internal.hidBuffer[1], ptrInputState);  // Começa do índice 1 para USB
    }

    // Retorna sucesso
    return DS5W_OK;
}

// Função principal para definir o estado da saída do dispositivo
DS5W_ReturnValue setDeviceOutputState(DeviceContext* ptrContext, 
                                      DS5OutputState* ptrOutputState)
{
    unsigned short outputReportLength = 0;
    unsigned int crcChecksum;

    // Verifica se os ponteiros são válidos
    if (!ptrContext || !ptrOutputState) 
    {
        return DS5W_E_INVALID_ARGS;
    }

    // Verifica se o dispositivo está conectado
    if (!ptrContext->_internal.connected) 
    {
        return DS5W_E_DEVICE_REMOVED;
    }

    // Determina o comprimento do relatório de saída conforme o tipo de conexão
    if (ptrContext->_internal.connection == BT) 
    {
        // O relatório de saída Bluetooth tem 547 bytes
        outputReportLength = 547;
    } 
    else 
    {
        // O relatório de saída USB tem 48 bytes
        outputReportLength = 48;
    }

    // Limpa os dados no buffer
    ZeroMemory(ptrContext->_internal.hidBuffer, outputReportLength);

    // Preenche o buffer de saída dependendo do tipo de conexão
    if (ptrContext->_internal.connection == BT) 
    {
        // Tipo de relatório para Bluetooth
        ptrContext->_internal.hidBuffer[0x00] = 0x31;  // Identificador de tipo para BT
        ptrContext->_internal.hidBuffer[0x01] = 0x02;  // Identificador de sub-tipo
        createHidOutputBuffer(&ptrContext->_internal.hidBuffer[2], ptrOutputState);

        // Calcula o CRC do buffer para o Bluetooth
        crcChecksum = computeCRC32(ptrContext->_internal.hidBuffer, 74);

        // Preenche os 4 bytes de checksum CRC no buffer
        ptrContext->_internal.hidBuffer[0x4A] = (unsigned char)((crcChecksum & 0x000000FF) >> 0);
        ptrContext->_internal.hidBuffer[0x4B] = (unsigned char)((crcChecksum & 0x0000FF00) >> 8);
        ptrContext->_internal.hidBuffer[0x4C] = (unsigned char)((crcChecksum & 0x00FF0000) >> 16);
        ptrContext->_internal.hidBuffer[0x4D] = (unsigned char)((crcChecksum & 0xFF000000) >> 24);
    } 
    else 
    {
        // Tipo de relatório para USB
        ptrContext->_internal.hidBuffer[0x00] = 0x02;  // Identificador de tipo para USB
        createHidOutputBuffer(&ptrContext->_internal.hidBuffer[1], ptrOutputState);  // Preenche o buffer a partir de [1] para USB
    }

    // Tenta escrever no controlador
    if (!WriteFile(ptrContext->_internal.deviceHandle, 
                   ptrContext->_internal.hidBuffer, 
                   outputReportLength, 
                   NULL, 
                   NULL)) 
    {
        // Caso falhe, fecha o handle e marca o dispositivo como desconectado
        CloseHandle(ptrContext->_internal.deviceHandle);
        ptrContext->_internal.deviceHandle = NULL;
        ptrContext->_internal.connected = 0;  // Marca como desconectado

        // Retorna erro de dispositivo removido
        return DS5W_E_DEVICE_REMOVED;
    }

    // Se tudo der certo, retorna sucesso
    return DS5W_OK;
}

#define MAX_DEVICE_PATH 260

typedef enum 
{
    DS5W_CONN_USB,
    DS5W_CONN_BT
} DS5W_DeviceConnection;

typedef struct 
{
    char path[MAX_DEVICE_PATH]; // Caminho do dispositivo
    DS5W_DeviceConnection connection; // Tipo de conexão (USB ou Bluetooth)
} DS5W_DeviceEnumInfo;

#define MAX_DEVICES 8 // Número máximo de dispositivos a serem armazenados


int main() 
{
    //list_hid_devices();
    
    
    DeviceEnumInfo devices[MAX_DEVICES]; // Array para armazenar os dispositivos
    unsigned int requiredLength = 0;
    DeviceContext con;
    DS5W_ReturnValue resultDeviceContext;

    DS5W_ReturnValue result = enumDevices(devices, MAX_DEVICES, &requiredLength, 1);

    if(requiredLength == 0) 
    {
        printf("\n Nenhum dispositivo encontrado.\n");
        return -15;
    }

    // Verificar retorno
    if(result == DS5W_OK)  // DS5W_OK
    {
        unsigned int i = 0;

        printf("Dispositivos detectados (%d encontrados):\n", requiredLength);
        for(i = 0; i < requiredLength; i++) 
        {
            printf(" \n =============== \n");
            printf("Dispositivo %d:\n", i + 1);
            printf("  Caminho: %s\n", devices[i]._internal.path);
            //printf("  Conexao: %s\n\n", devices[i]._internal.connection == DS5W_CONN_USB ? "USB" : "Bluetooth");
            //printf(" =============== \n");
            if(devices[i]._internal.connection == BT)
            {
                printf(" \n Conexao Bluetooth \n");
            }
            else if(devices[i]._internal.connection == (BT | USB))
            {
                printf(" \n Conexao Bluetooth & chargin \n");
            }
            else
            {
                printf(" \n Conexao USB \n");
            }

        }
    } 
    else if(result == DS5W_E_INSUFFICIENT_BUFFER) // DS5W_E_INSUFFICIENT_BUFFER
    {
        printf("Buffer insuficiente. São necessários pelo menos %d entradas.\n", requiredLength);
    } 
    else 
    {
        printf("Erro ao listar dispositivos (codigo: %d).\n", result);
    }

    resultDeviceContext = initDeviceContext(&devices[0], &con);


    if(resultDeviceContext == OK)
    {
        DS5InputState inState;
        DS5OutputState outState;
        // Color intentsity
        float intensity = 1.0f;
        uint16_t lrmbl = 0.0;
        uint16_t rrmbl = 0.0;
        // Force
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
                // === Read Input ===
                // Build all universal buttons (USB and BT) as text

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


                // === Write Output ===
                // Rumbel
                lrmbl = max(lrmbl - 0x200 / btMul, 0);
                rrmbl = max(rrmbl - 0x100 / btMul, 0);

                outState.leftRumble = (lrmbl & 0xFF00) >> 8UL;
                outState.rightRumble = (rrmbl & 0xFF00) >> 8UL;

                // Lightbar
                outState.lightbar = color_R8G8B8_UCHAR_A32_FLOAT(0, 255, 0, intensity);
                intensity -= 0.0025f / btMul;
                if(intensity <= 0.0f) 
                {
                    intensity = 1.0f;

                    lrmbl = 0xFF00;
                    rrmbl = 0xFF00;
                }

                // Player led
                if (outState.rightRumble) 
                {
                    outState.playerLeds.playerLedFade = true;
                    outState.playerLeds.bitmask = DS5W_OSTATE_PLAYER_LED_MIDDLE;
                    outState.playerLeds.brightness = LED_BRIGHTNESS_HIGH;
                }
                else 
                {
                    outState.playerLeds.bitmask = 0;
                }

                // Set force
                if(inState.rightTrigger == 0xFF) 
                {
                    rType = ContinuousResitance;
                } 
                else if(inState.rightTrigger == 0x00) 
                {
                    rType = NoResitance;
                }

                // Mic led
                if(inState.buttonsB & DS5W_ISTATE_BTN_B_MIC_BUTTON) 
                {
                    outState.microphoneLed = ON;
                }
                else if(inState.buttonsB & DS5W_ISTATE_BTN_B_PLAYSTATION_LOGO) 
                {
                    outState.microphoneLed = OFF;
                }

                // Left trigger is clicky / section
                outState.leftTriggerEffect.effectType = SectionResitance;
                outState.leftTriggerEffect.Section.startPosition = 0x00;
                outState.leftTriggerEffect.Section.endPosition = 0x60;

                // Right trigger is forcy
                outState.rightTriggerEffect.effectType = rType;
                outState.rightTriggerEffect.Continuous.force = 0xFF;
                outState.rightTriggerEffect.Continuous.startPosition = 0x00;                

                setDeviceOutputState(&con, &outState);

            }
            else
            {
                // Device disconnected show error and try to reconnect
                printf("\nDevice removed!\n");
                reconnectDevice(&con);
            }
        }
        // Free state
        freeDeviceContext(&con);
    }
    else
    {
        printf(" \n Erro na funcao initDeviceContext \n");
        printf("\n  Failed to connect to controller! \n");
        system("pause");
        return -1;
    }
    return 0;
}





