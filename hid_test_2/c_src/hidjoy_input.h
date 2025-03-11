/*
    DS5_Input.h is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL

*/

#ifndef HIDJOY_INPUT_H
#define HIDJOY_INPUT_H

#include<windows.h>

#include<hidjoy_state.h>

void evaluateHidInputBuffer(unsigned char* hidInBuffer, 
                            DS5InputState* ptrInputState);


#endif /* HIDJOY_INPUT_H */
