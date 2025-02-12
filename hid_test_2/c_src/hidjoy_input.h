#ifndef HIDJOY_INPUT_H
#define HIDJOY_INPUT_H

#include<windows.h>

#include<hidjoy_state.h>

void evaluateHidInputBuffer(unsigned char* hidInBuffer, 
                            DS5InputState* ptrInputState);


#endif /* HIDJOY_INPUT_H */
