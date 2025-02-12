
#include"DS_CRC32.h"

#include<stdio.h>
#include<stdint.h>

#include<windows.h>

/* Function to calculate CRC32 */
UINT32 computeCRC32(unsigned char* buffer, size_t len) 
{
    size_t i;
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





