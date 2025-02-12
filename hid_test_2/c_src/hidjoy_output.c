
#include"hidjoy_output.h"


#ifdef __WATCOMC__
    #define MAX(a, b) ((a) > (b) ? (a) : (b))
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif

void createHidOutputBuffer(unsigned char* hidOutBuffer, 
                           DS5OutputState* ptrOutputState) 
{
    /* Feature mask */
    hidOutBuffer[0x00] = 0xFF;
    hidOutBuffer[0x01] = 0xF7;

    /* Rumble motors */
    hidOutBuffer[0x02] = ptrOutputState->rightRumble;
    hidOutBuffer[0x03] = ptrOutputState->leftRumble;

    /* Mic led */
    hidOutBuffer[0x08] = (unsigned char)ptrOutputState->microphoneLed;

    /* Player led */
    hidOutBuffer[0x2B] = ptrOutputState->playerLeds.bitmask;
    if (ptrOutputState->playerLeds.playerLedFade) 
    {
        hidOutBuffer[0x2B] &= ~(0x20);
    }
    else 
    {
        hidOutBuffer[0x2B] |= 0x20;
    }

    /* Player led brightness */
    hidOutBuffer[0x26] = 0x03;
    hidOutBuffer[0x29] = ptrOutputState->disableLeds ? 0x01 : 0x2;
    hidOutBuffer[0x2A] = ptrOutputState->playerLeds.brightness;

    /* Lightbar */
    hidOutBuffer[0x2C] = ptrOutputState->lightbar.r;
    hidOutBuffer[0x2D] = ptrOutputState->lightbar.g;
    hidOutBuffer[0x2E] = ptrOutputState->lightbar.b;

    /* Adaptive Triggers */
    processTrigger(&ptrOutputState->leftTriggerEffect, &hidOutBuffer[0x15]);
    processTrigger(&ptrOutputState->rightTriggerEffect, &hidOutBuffer[0x0A]);
}

void processTrigger(TriggerEffect* ptrEffect, unsigned char* buffer) 
{
    /* Switch on effect */
    switch (ptrEffect->EffectEx.startPosition) 
    {
        /* Continious */
        case 1:
            buffer[0x00] = 0x01;
            buffer[0x01] = ptrEffect->Continuous.startPosition;
            buffer[0x02] = ptrEffect->Continuous.force;
            break;

        /* Section */
        case 2:
            buffer[0x00] = 0x02;
            buffer[0x01] = ptrEffect->Continuous.startPosition;
            buffer[0x02] = ptrEffect->Continuous.force;
            break;

        /* EffectEx */
        case 3:
            buffer[0x00] = 0x02 | 0x20 | 0x04;
            buffer[0x01] = 0xFF - ptrEffect->EffectEx.startPosition;
            if (ptrEffect->EffectEx.keepEffect) 
            {
                buffer[0x02] = 0x02;
            }
            buffer[0x04] = ptrEffect->EffectEx.beginForce;
            buffer[0x05] = ptrEffect->EffectEx.middleForce;
            buffer[0x06] = ptrEffect->EffectEx.endForce;
            buffer[0x09] = max(1, ptrEffect->EffectEx.frequency / 2);
            break;

        /* Calibrate */
        case 4:
            buffer[0x00] = 0xFC;
            break;

        /* No resistance / default */
        default:
            buffer[0x00] = 0x00;
            buffer[0x01] = 0x00;
            buffer[0x02] = 0x00;
            break;
    }
}






