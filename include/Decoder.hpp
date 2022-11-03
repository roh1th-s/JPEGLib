#ifndef DECODER_HPP
#define DECODER_HPP

#include <fstream>
#include <string>
#include <iostream>

class Decoder
{
public:
    Decoder(/* args */);
    ~Decoder();

    void open(std::string fileName);

    
private:
    std::ifstream m_ImageFile;

};

#endif // DECODER_HPP