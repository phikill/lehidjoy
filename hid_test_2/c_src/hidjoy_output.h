#ifndef HIDJOY_OUTPUT_H
#define HIDJOY_OUTPUT_H

#include<hidjoy_api.h>
#include<hidjoy_device.h>
#include<hidjoy_state.h>

void createHidOutputBuffer(unsigned char* hidOutBuffer, 
                           DS5OutputState* ptrOutputState);

void processTrigger(TriggerEffect* ptrEffect, 
					unsigned char* buffer);


#endif /* HIDJOY_OUTPUT_H */
