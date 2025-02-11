#ifndef DEVICE_H
#define DEVICE_H

#include<stdbool.h>
#include<stdio.h>

/*

*/


// Enum para o tipo de conexão do dispositivo
typedef enum 
{
    // Controlador conectado via USB
    USB = 0,

    // Controlador conectado via Bluetooth
    BT = 1
} DeviceConnection;

// Estrutura para armazenar informações do dispositivo durante a descoberta
typedef struct 
{
    // Estrutura interna para encapsular dados e impedir modificações externas
    struct 
    {
        // Caminho para o dispositivo descoberto
        char path[260];

        // Tipo de conexão do dispositivo descoberto
        DeviceConnection connection;
    } _internal;
} DeviceEnumInfo;

// Estrutura para o contexto do dispositivo
typedef struct 
{
    // Estrutura interna para encapsular dados e tentar prevenir modificações externas
    struct 
    {
        // Caminho para o dispositivo
        char devicePath[260];

        // Handle para o dispositivo aberto
        void* deviceHandle;

        // Tipo de conexão do dispositivo
        DeviceConnection connection;

        // Estado atual da conexão
        bool connected;

        // Buffer HID de entrada (será alocado pela função de inicialização do contexto)
        unsigned char hidBuffer[547];
    } _internal;
} DeviceContext;







#endif /* DEVICE_H */









