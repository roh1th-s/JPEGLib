#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <cstdint>

#include "Util.hpp"

class Pixel
{
public:
    Pixel(/* args */);
    ~Pixel();

    uint8_t r;
    uint8_t g;
    uint8_t b;
};

Pixel::Pixel() {};
Pixel::~Pixel() {};

#endif


