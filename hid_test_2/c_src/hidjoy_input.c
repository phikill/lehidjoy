
#include"hidjoy_input.h"


void evaluateHidInputBuffer(unsigned char* hidInBuffer, DS5InputState* ptrInputState) 
{
	UINT32 touchpad1Raw;
	UINT32 touchpad2Raw;

    /* Convert sticks to signed range */
    ptrInputState->leftStick.x = (char)(((short)(hidInBuffer[0x00] - 128)));
    ptrInputState->leftStick.y = (char)(((short)(hidInBuffer[0x01] - 127)) * -1);
    ptrInputState->rightStick.x = (char)(((short)(hidInBuffer[0x02] - 128)));
    ptrInputState->rightStick.y = (char)(((short)(hidInBuffer[0x03] - 127)) * -1);

    /* Convert trigger to unsigned range */
    ptrInputState->leftTrigger = hidInBuffer[0x04];
    ptrInputState->rightTrigger = hidInBuffer[0x05];

    /* Buttons */
    ptrInputState->buttonsAndDpad = hidInBuffer[0x07] & 0xF0;
    ptrInputState->buttonsA = hidInBuffer[0x08];
    ptrInputState->buttonsB = hidInBuffer[0x09];

    /* Dpad */
    switch (hidInBuffer[0x07] & 0x0F) 
    {
        /* UP */
        case 0x0:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_UP;
            break;
        /* Low */
        case 0x4:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_DOWN;
            break;
        /* Left */
        case 0x6:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_LEFT;
            break;
        /* Right */
        case 0x2:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_RIGHT;
            break;
        /* Left and Down */
        case 0x5:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_LEFT | DS5W_ISTATE_DPAD_DOWN;
            break;
        /* Left and Up */
        case 0x7:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_LEFT | DS5W_ISTATE_DPAD_UP;
            break;
        /* Right and Up */
        case 0x1:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_RIGHT | DS5W_ISTATE_DPAD_UP;
            break;
        /* Right and Down */
        case 0x3:
            ptrInputState->buttonsAndDpad |= DS5W_ISTATE_DPAD_RIGHT | DS5W_ISTATE_DPAD_DOWN;
            break;
    }

    /* Copy accelerometer readings */
    memcpy(&ptrInputState->accelerometer, &hidInBuffer[0x0F], 2 * 3);

    /* TEMP: Copy gyroscope data (no processing is done at this time!) */
    memcpy(&ptrInputState->gyroscope, &hidInBuffer[0x15], 2 * 3);

    /* Assess the health of the touchpad 1 */
    touchpad1Raw = *(UINT32*)(&hidInBuffer[0x20]);
    ptrInputState->touchPoint1.y = (touchpad1Raw & 0xFFF00000) >> 20;
    ptrInputState->touchPoint1.x = (touchpad1Raw & 0x000FFF00) >> 8;
    ptrInputState->touchPoint1.down = (touchpad1Raw & (1 << 7)) == 0;
    ptrInputState->touchPoint1.id = (touchpad1Raw & 127);

    /* Assess the health of the touchpad 2 */
    touchpad2Raw = *(UINT32*)(&hidInBuffer[0x24]);
    ptrInputState->touchPoint2.y = (touchpad2Raw & 0xFFF00000) >> 20;
    ptrInputState->touchPoint2.x = (touchpad2Raw & 0x000FFF00) >> 8;
    ptrInputState->touchPoint2.down = (touchpad2Raw & (1 << 7)) == 0;
    ptrInputState->touchPoint2.id = (touchpad2Raw & 127);

    /* Evaluate the headphone jack */
    ptrInputState->headPhoneConnected = hidInBuffer[0x35] & 0x01;

    /* Trigger feedback */
    ptrInputState->leftTriggerFeedback = hidInBuffer[0x2A];
    ptrInputState->rightTriggerFeedback = hidInBuffer[0x29];

    /* Battery */
    ptrInputState->battery.chargin = (hidInBuffer[0x35] & 0x08);
    ptrInputState->battery.fullyCharged = (hidInBuffer[0x36] & 0x20);
    ptrInputState->battery.level = (hidInBuffer[0x36] & 0x0F);
}


