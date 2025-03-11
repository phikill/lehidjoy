/*
    DS_CRC32.c is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/

#include"DS_CRC32.h"

/* Function to calculate CRC32 */
UINT32 computeCRC32(unsigned char* buffer, size_t len) 
{
    size_t i = 0;
    /* Start point */
    UINT32 result = crcSeed;
    
    /* Foreach element in arrray */
    for(i = 0; i < len; i++) 
    {
        /* Compute crc */
        result = hashTable[((unsigned char)result) ^ ((unsigned char)buffer[i])] ^ (result >> 8);
    }

    /* Return result */
    return result;
}
