#ifndef HELPERS_H
#define HELPERS_H

#include"hidjoy_state.h"

/* Function to create a color in the format R32G32B32_FLOAT */
Color color_R32G32B32_FLOAT(float r, 
                            float g, 
                            float b);

/* Function to create a color in the format R32G32B32A32_FLOAT */
Color color_R32G32B32A32_FLOAT(float r, 
                               float g, 
                               float b, 
                               float a);

/* Function to create a color in the format R8G8B8A8_UCHAR */
Color color_R8G8B8A8_UCHAR(unsigned char r, 
                           unsigned char g, 
                           unsigned char b, 
                           unsigned char a);

/* Function to create a color in the format R8G8B8_UCHAR_A32_FLOAT */
Color color_R8G8B8_UCHAR_A32_FLOAT(unsigned char r, 
                                   unsigned char g, 
                                   unsigned char b, 
                                   float a);


#endif /* HELPERS_H */
