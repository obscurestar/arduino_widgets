//pixels.h
//Middleware to make lighting algorithms independent of underlying code.

#ifndef PIXELS_H //Don't dupe header files.
#define PIXELS_H

#include "color.h"

class Pixels
{
public:
    Pixels(uint8_t *pix,
           const unsigned short &num,
           const unsigned short &bpp,
           const unsigned long &ord);

    const unsigned short getNumPixels() { return numPixels; }
    void setPixelColor(const unsigned short &id,const unsigned long &color);
    const unsigned long getPixelColor(const unsigned short &id);
private:
    unsigned short numPixels;
    unsigned short bytesPerPixel;
    COLOR order;
    uint8_t *pixels;
};

Pixels::Pixels(uint8_t *pix,
               const unsigned short &num,
               const unsigned short &bpp,
               const unsigned long &ord)
{
    pixels = pix;
    numPixels = num;
    bytesPerPixel = bpp;
    order.l = ord;
}

void Pixels::setPixelColor(const unsigned short &id,const unsigned long &c)
{
    //Values in color are assumed to be RGBW ordered.
    COLOR color;
    color.l = c;

    if (id >= numPixels) return; //Don't overrun memory.

    unsigned long offset = id * bytesPerPixel;

    for (int i=0;i<bytesPerPixel;++i)
    {
       //pixels[offset + i] = color.c[i];
       pixels[offset + order.c[i]] = color.c[i]; 
    }
}

const unsigned long Pixels::getPixelColor(const unsigned short &id)
{
    COLOR color;
    color.l = 0;

    if (id >= numPixels) return 0; //Don't overrun memory. return black.

    unsigned long offset = id * bytesPerPixel;

    for (int i=0;i<bytesPerPixel;++i)
    {
       //color.c[i] = pixels[offset + i];
       color.c[i] = pixels[offset + order.c[i]];
    }
    return color.l;
}

#endif //PIXELS_H
