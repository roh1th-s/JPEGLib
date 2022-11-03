#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>

#include "Pixel.hpp"
#include "Util.hpp"

class Image
{
public:
    Image();
    ~Image();

    uint32_t width;
    uint32_t height;
    
    std::vector<std::vector<Pixel>> m_data;
private:

};

#endif
