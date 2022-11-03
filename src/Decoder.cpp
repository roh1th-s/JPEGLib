#include "decoder.hpp"

Decoder::Decoder()
{
}

Decoder::~Decoder()
{
}

void Decoder::open(std::string fileName) {
    m_ImageFile.open(fileName, std::ios::binary | std::ios::in);

    while(!m_ImageFile.eof()) {
        //uint8_t byte = m_ImageFile.read();
    }
}
