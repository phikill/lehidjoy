/*
    Helpers.c is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/

#include<helpers.h>

/* Function to create a color in the format R32G32B32_FLOAT */
Color color_R32G32B32_FLOAT(float r, float g, float b) 
{
    Color color;
    color.r = (unsigned char)(255.0F * r);
    color.g = (unsigned char)(255.0F * g);
    color.b = (unsigned char)(255.0F * b);
    return color;
}

/* Function to create a color in the format R32G32B32A32_FLOAT */
Color color_R32G32B32A32_FLOAT(float r, float g, float b, float a) 
{
    Color color;
    color.r = (unsigned char)(255.0F * r * a);
    color.g = (unsigned char)(255.0F * g * a);
    color.b = (unsigned char)(255.0F * b * a);
    return color;
}

/* Function to create a color in the format R8G8B8A8_UCHAR */
Color color_R8G8B8A8_UCHAR(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
{
    Color color;
    color.r = (unsigned char)(r * (a / 255.0f));
    color.g = (unsigned char)(g * (a / 255.0f));
    color.b = (unsigned char)(b * (a / 255.0f));
    return color;
}

/* Function to create a color in the format R8G8B8_UCHAR_A32_FLOAT */
Color color_R8G8B8_UCHAR_A32_FLOAT(unsigned char r, unsigned char g, unsigned char b, float a) 
{
    Color color;
    color.r = (unsigned char)(r * a);
    color.g = (unsigned char)(g * a);
    color.b = (unsigned char)(b * a);
    return color;
}

