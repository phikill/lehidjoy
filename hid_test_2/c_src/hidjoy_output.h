/*
	DS5_Output.h is part of DualSenseWindows
	https://github.com/Ohjurot/DualSense-Windows

	Contributors of this file:
	11.2020 Ludwig Füchsl

	Licensed under the MIT License (To be found in repository root directory)

	MODC89: PHIKILL
*/

#ifndef HIDJOY_OUTPUT_H
#define HIDJOY_OUTPUT_H

#include<hidjoy_api.h>
#include<hidjoy_device.h>
#include<hidjoy_state.h>

void createHidOutputBuffer(unsigned char*  hidOutBuffer, 
                           DS5OutputState* ptrOutputState);

void processTrigger(TriggerEffect* ptrEffect, 
					unsigned char* buffer);


#endif /* HIDJOY_OUTPUT_H */
