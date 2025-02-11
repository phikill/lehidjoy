#ifndef HELPERS_H
#define HELPERS_H

// Função para criar uma cor no formato R32G32B32_FLOAT
Color color_R32G32B32_FLOAT(float r, float g, float b) 
{
    Color color;
    color.r = (unsigned char)(255.0F * r);
    color.g = (unsigned char)(255.0F * g);
    color.b = (unsigned char)(255.0F * b);
    color.a = 255;  // Default to full opacity if not provided
    return color;
}

// Função para criar uma cor no formato R32G32B32A32_FLOAT
Color color_R32G32B32A32_FLOAT(float r, float g, float b, float a) 
{
    Color color;
    color.r = (unsigned char)(255.0F * r * a);
    color.g = (unsigned char)(255.0F * g * a);
    color.b = (unsigned char)(255.0F * b * a);
    color.a = (unsigned char)(255.0F * a);  // Apply opacity scaling
    return color;
}

// Função para criar uma cor no formato R8G8B8A8_UCHAR
Color color_R8G8B8A8_UCHAR(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
{
    Color color;
    color.r = (unsigned char)(r * (a / 255.0f));
    color.g = (unsigned char)(g * (a / 255.0f));
    color.b = (unsigned char)(b * (a / 255.0f));
    color.a = a;
    return color;
}

// Função para criar uma cor no formato R8G8B8_UCHAR_A32_FLOAT
Color color_R8G8B8_UCHAR_A32_FLOAT(unsigned char r, unsigned char g, unsigned char b, float a) 
{
    Color color;
    color.r = (unsigned char)(r * a);
    color.g = (unsigned char)(g * a);
    color.b = (unsigned char)(b * a);
    color.a = (unsigned char)(a * 255); // Assume alpha as the factor applied to each component
    return color;
}


















#endif /* HELPERS_H */
