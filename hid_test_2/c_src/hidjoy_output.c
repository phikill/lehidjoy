/*
    DS5_Output.h is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/


#include"hidjoy_output.h"


#ifdef __WATCOMC__
    #define NEED_MAX_DEF
#elif __GNUC__
    #define NEED_MAX_DEF
#elif _MSC_VER
    #define NEED_MAX_DEF
#endif

#ifdef NEED_MAX_DEF
    #define MAX(a, b) ((a) > (b) ? (a) : (b))
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif 

void createHidOutputBuffer(unsigned char*  hidOutBuffer, 
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
    switch(ptrEffect->effectType) 
    {
        /* Continious */
        case ContinuousResitance:
            /* Mode */
            buffer[0x00] = 0x01;
            /* Parameters */
            buffer[0x01] = ptrEffect->Continuous.startPosition;
            buffer[0x02] = ptrEffect->Continuous.force;
        break;

        /* Section */
        case SectionResitance:
            /* Mode */
            buffer[0x00] = 0x02;
            /* Parameters */
            buffer[0x01] = ptrEffect->Continuous.startPosition;
            buffer[0x02] = ptrEffect->Continuous.force;
        break;

        /* EffectEx */
        case EffectEx:
            /* Mode */
            buffer[0x00] = 0x02 | 0x20 | 0x04;
            /* Parameters */
            buffer[0x01] = 0xFF - ptrEffect->EffectEx.startPosition;
            /* Keep flag */
            if(ptrEffect->EffectEx.keepEffect) 
            {
                buffer[0x02] = 0x02;
            }
            /* Forces */
            buffer[0x04] = ptrEffect->EffectEx.beginForce;
            buffer[0x05] = ptrEffect->EffectEx.middleForce;
            buffer[0x06] = ptrEffect->EffectEx.endForce;
            /* Frequency */
            buffer[0x09] = max(1, ptrEffect->EffectEx.frequency / 2);
        break;

        /* Calibrate */
        case Calibrate:
            /* Mode */
            buffer[0x00] = 0xFC;
        break;

        /* No resistance / default */
        case NoResitance:
            /* __fallthrough; */
            goto default_case;
        default:
        default_case:
            /* All zero */
            buffer[0x00] = 0x00;
            buffer[0x01] = 0x00;
            buffer[0x02] = 0x00;
        break;
    }
}
