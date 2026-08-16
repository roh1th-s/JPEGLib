#ifndef DECODER_HPP
#define DECODER_HPP

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "huffman.hpp"

class Decoder {
  public:
    Decoder(/* args */);
    ~Decoder();

    // Open image file of specified file name
    void open(std::string fileName);

    // Decode image file
    void decode();

    enum Result { SUCCESS, TERMINATE };

  private:
    // Handle to the image data being decoded
    std::ifstream m_ImageFile;
    std::shared_ptr<HuffmanTree> m_huffmanTables[2][4] = {};

    // Parse appropriate segment from byte
    Result parseSegment(uint8_t byte);

    // Parse APP-0 segment
    void decodeAPP0Segment();

    // Parse comment segment
    void decodeCOMSegment();

    // Parse quantization tables
    void decodeDQTSegment();

    // Parse Huffman tables
    void decodeDHTSegment();

    // Parse frame segment
    Result decodeSOF0Segment();

    // Parse scan segment
    void decodeSOSSegment();

    // Parse the actual image data
    void decodeImageData();
};

#endif // DECODER_HPP
