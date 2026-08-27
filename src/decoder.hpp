#ifndef DECODER_HPP
#define DECODER_HPP

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "huffman.hpp"
#include "image.hpp"

struct ComponentSpec {
    uint8_t dc_ht;
    uint8_t ac_ht;
    uint8_t quant_table_no;
};

class Decoder {
  public:
    Decoder(/* args */);
    ~Decoder();

    // Open image file of specified file name
    void open(std::string fileName);

    enum Result { SUCCESS, TERMINATE };

    // Decode image file
    Result decode();

    Image m_image;

  private:
    // Handle to the image data being decoded
    std::ifstream m_ImageFile;

    enum DHT_Type { DC = 0, AC = 1 };

    // idx 1 : 0 = DC, 1 = AC
    // idx 2 : 0-3 = table number
    std::shared_ptr<HuffmanTable> m_huffmanTables[2][4] = {};

    uint8_t m_quantizationTables[4][64] = {};

    std::vector<uint8_t> m_scanData;

    std::vector<ComponentSpec> m_componentSpec;

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
