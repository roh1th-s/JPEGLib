#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

#include "bitreader.hpp"
#include "dct.hpp"
#include "decoder.hpp"
#include "huffman.hpp"
#include "markers.hpp"
#include "util.hpp"

Decoder::Decoder() {}

Decoder::~Decoder() {}

void Decoder::open(std::string fileName) {
    m_ImageFile.open(fileName, std::ios::binary | std::ios::in);

    if (m_ImageFile.fail()) {
        std::cout << "Failed to open file : " << fileName;
        return;
    }
}

Decoder::Result Decoder::decode() {
    uint8_t byte;
    while (m_ImageFile.good()) {
        m_ImageFile >> std::noskipws >> byte;

        if (byte == Markers::JFIF_BYTE_FF) {
            m_ImageFile >> std::noskipws >> byte;

            // choose decoding function based on the byte after ff
            Decoder::Result result = parseSegment(byte);

            std::cout << "\n";

            if (result == TERMINATE) {
                std::cout << "Error decoding file, terminating decoder.";
                return result;
            }
        }
    }
    return SUCCESS;
}

Decoder::Result Decoder::parseSegment(uint8_t byte) {
    if (byte == Markers::JFIF_BYTE_0 || byte == Markers::JFIF_BYTE_FF)
        return TERMINATE;

    switch (byte) {
    case Markers::JFIF_SOI:
        std::cout << "Found segment, Start of Image (FFD8)" << std::endl;
        return SUCCESS;
    case Markers::JFIF_APP0:
        std::cout << "Found segment, JPEG/JFIF Image Marker segment (APP0)" << std::endl;
        decodeAPP0Segment();
        return SUCCESS;
    case Markers::JFIF_COM:
        std::cout << "Found segment, Comment(FFFE)" << std::endl;
        decodeCOMSegment();
        return SUCCESS;
    case Markers::JFIF_DQT:
        std::cout << "Found segment, Define Quantization Table (FFDB)" << std::endl;
        decodeDQTSegment();
        return SUCCESS;
    case Markers::JFIF_SOF0:
        std::cout << "Found segment, Start of Frame 0: Baseline DCT (FFC0)" << std::endl;
        return decodeSOF0Segment();
    case Markers::JFIF_SOF1:
        std::cout << "Found segment, Start of Frame 1: Extended Sequential DCT "
                     "(FFC1), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF2:
        std::cout << "Found segment, Start of Frame 2: Progressive DCT (FFC2), Not "
                     "supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF3:
        std::cout << "Found segment, Start of Frame 3: Lossless Sequential (FFC3), "
                     "Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF5:
        std::cout << "Found segment, Start of Frame 5: Differential Sequential DCT "
                     "(FFC5), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF6:
        std::cout << "Found segment, Start of Frame 6: Differential Progressive "
                     "DCT (FFC6), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF7:
        std::cout << "Found segment, Start of Frame 7: Differential lossless "
                     "(Sequential) (FFC7), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF9:
        std::cout << "Found segment, Start of Frame 9: Extended Sequential DCT, "
                     "Arithmetic Coding (FFC9), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF10:
        std::cout << "Found segment, Start of Frame 10: Progressive DCT, "
                     "Arithmetic Coding (FFCA), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF11:
        std::cout << "Found segment, Start of Frame 11: Lossless (Sequential), "
                     "Arithmetic Coding (FFCB), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF13:
        std::cout << "Found segment, Start of Frame 13: Differentical Sequential "
                     "DCT, Arithmetic Coding (FFCD), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF14:
        std::cout << "Found segment, Start of Frame 14: Differentical Progressive "
                     "DCT, Arithmetic Coding (FFCE), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF15:
        std::cout << "Found segment, Start of Frame 15: Differentical Lossless "
                     "(Sequential), Arithmetic Coding (FFCF), Not supported"
                  << std::endl;
        return TERMINATE;
    case Markers::JFIF_DHT:
        std::cout << "Found segment, Define Huffman Table (FFC4)" << std::endl;
        decodeDHTSegment();
        return SUCCESS;
    case Markers::JFIF_SOS:
        std::cout << "Found segment, Start of Scan (FFDA)" << std::endl;
        decodeSOSSegment();
        return SUCCESS;
    }

    return SUCCESS;
}

void Decoder::decodeAPP0Segment() {
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing APP-0 segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char*>(&length), 2);
    length = swapBytes(length); // bytes are stored as big endian, convert to little

    // skip JFIF\0 string
    m_ImageFile.seekg(5, std::ios_base::cur);

    uint16_t jfifVersion;
    m_ImageFile.read(reinterpret_cast<char*>(&jfifVersion), 2);

    std::cout << "JFIF Version : " << (int)(jfifVersion >> 8) << "."
              << (int)((jfifVersion & 0x00F0) >> 4) << (int)(jfifVersion & 0x000F) << std::endl;

    uint8_t densityUnitByte;
    m_ImageFile >> std::noskipws >> densityUnitByte;

    std::string densityUnit = "";
    switch (densityUnitByte) {
    case 0x00:
        densityUnit = "Pixel Aspect Ratio";
        break;
    case 0x01:
        densityUnit = "Pixels per inch (DPI)";
        break;
    case 0x02:
        densityUnit = "Pixels per centimeter";
        break;
    }

    std::cout << "Image density unit: " << densityUnit << std::endl;

    uint16_t horizontalPixelDensity;
    m_ImageFile.read(reinterpret_cast<char*>(&horizontalPixelDensity), 2);
    horizontalPixelDensity = swapBytes(horizontalPixelDensity);

    uint16_t verticalPixelDensity;
    m_ImageFile.read(reinterpret_cast<char*>(&verticalPixelDensity), 2);
    verticalPixelDensity = swapBytes(verticalPixelDensity);

    std::cout << "Pixel density: " << horizontalPixelDensity << "x" << verticalPixelDensity
              << std::endl;
    uint8_t horizontalPixelCount, verticalPixelCount;
    m_ImageFile >> std::noskipws >> horizontalPixelCount >> verticalPixelCount;

    // thumbnail data : packed - 24 bit for each pixel (3 channels)
    uint16_t thumbnailDataSize = 3 * horizontalPixelCount * verticalPixelCount;

    if (thumbnailDataSize > 0) {
        uint8_t* thumbnailData = new uint8_t[thumbnailDataSize];
        m_ImageFile.read(reinterpret_cast<char*>(thumbnailData),
                         3 * horizontalPixelCount * verticalPixelCount);
    }

    std::cout << "APP-0 segment parsed successfully." << std::endl;
}

void Decoder::decodeCOMSegment() {
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing comment segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char*>(&length), 2);
    length = swapBytes(length);

    std::cout << "Comment segment length: " << length << std::endl;

    length -= 2; // length includes markers

    uint8_t* commentData = new uint8_t[length];
    m_ImageFile.read(reinterpret_cast<char*>(commentData), length);

    std::string comment(reinterpret_cast<char const*>(commentData), length);

    std::cout << "Comments: " << comment << std::endl;
    std::cout << "COM segment parsed successfully." << std::endl;

    delete[] commentData;
}

void Decoder::decodeDQTSegment() {
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing DQT segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char*>(&length), 2);
    length = swapBytes(length);

    std::cout << "DQT Segment length: " << length << std::endl;

    length -= 2; // marker bytes included in length

    while (length > 0) {
        uint8_t qt_info;
        m_ImageFile >> std::noskipws >> qt_info;

        uint8_t precision = qt_info >> 4;
        uint8_t qt_n = qt_info & 0xF; // Quantization table number

        std::cout << "QT no: " << (int)qt_n
                  << ", Precision: " << (precision == 0 ? "8-bit" : "16-bit") << std::endl;

        m_ImageFile.read(reinterpret_cast<char*>(m_quantizationTables[qt_n]), 64);

        length -= 65;
    }

    std::cout << "Successfully parsed DQT segment." << std::endl;
}

void Decoder::decodeDHTSegment() {
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char*>(&length), 2);
    length = swapBytes(length);

    std::cout << "DHT Segment length: " << length << std::endl;

    length -= 2; // marker bytes included in length

    int endOfSegment = (int)m_ImageFile.tellg() + length;

    while (m_ImageFile.tellg() < endOfSegment) {
        uint8_t ht_info;

        m_ImageFile >> std::noskipws >> ht_info;

        uint8_t ht_type = ht_info >> 4;
        uint8_t ht_no = ht_info & 0x0F;

        if (ht_type > 1 || ht_no > 3) {
            std::cerr << "Malformed DHT data" << std::endl;
            return;
        }

        std::vector<uint8_t> dht_payload(16);
        m_ImageFile.read(reinterpret_cast<char*>(dht_payload.data()), 16);

        // count total symbols
        int total_symbols = 0;
        for (uint8_t count : dht_payload) {
            total_symbols += count;
        }

        // resize buffer to fit everything
        size_t old_size = dht_payload.size();
        dht_payload.resize(old_size + total_symbols);

        m_ImageFile.read(reinterpret_cast<char*>(dht_payload.data() + old_size), total_symbols);

        auto h_table = HuffmanTable::fromJfifData(dht_payload);
        m_huffmanTables[ht_type][ht_no] = h_table;

        std::cout << "Parsed HT, ht_type: " << (ht_type == DHT_Type::DC ? "DC" : "AC")
                  << ", ht_no: " << (int)ht_no << std::endl;
    }

    std::cout << "Successfully parsed Huffman table segment." << std::endl;
}

Decoder::Result Decoder::decodeSOF0Segment() {
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return TERMINATE;
    }

    std::cout << "Parsing Frame-0 segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char*>(&length), 2);
    length = swapBytes(length);

    std::cout << "SOF-0 Segment length: " << length << std::endl;

    length -= 2; // marker bytes included in length

    uint8_t precision;
    m_ImageFile >> std::noskipws >> precision;

    std::cout << "SOF-0 segment data precision: " << (int)precision << std::endl;

    uint16_t image_height, image_width;
    m_ImageFile.read(reinterpret_cast<char*>(&image_height), 2);
    m_ImageFile.read(reinterpret_cast<char*>(&image_width), 2);
    image_height = swapBytes(image_height);
    image_width = swapBytes(image_width);

    m_image.width = image_width;
    m_image.height = image_height;

    std::cout << "Image height: " << (int)image_height << " Image width: " << (int)image_width
              << std::endl;

    uint8_t n_components;
    m_ImageFile >> std::noskipws >> n_components;

    m_image.n_components = n_components;

    std::cout << "No. of components: " << (int)n_components << std::endl;

    if (m_componentSpec.size() < n_components) {
        m_componentSpec.resize(n_components);
    }

    uint8_t comp, sampling_factor, qt_n;

    bool chroma_ss = false; // flag for presence of chroma subsampling

    for (auto i = 0; i < n_components; i++) {
        m_ImageFile >> std::noskipws >> comp >> sampling_factor >> qt_n;

        int sf_horizontal = (int)(sampling_factor >> 4);
        int sf_vertical = (int)(sampling_factor & 0x0F);

        std::cout << "Component: " << (int)comp << std::endl;
        std::cout << "Sampling Factor, Horizontal: " << sf_horizontal
                  << ", Vertical: " << sf_vertical << std::endl;
        std::cout << "Quantization table no.: " << (int)qt_n << std::endl;

        m_componentSpec[comp - 1].quant_table_no = qt_n;

        if (!(sf_horizontal == 1 && sf_vertical == 1)) {
            chroma_ss = true;
        }
    }

    if (chroma_ss) {
        std::cout << "Chroma subsampling is not supported." << std::endl;
        return TERMINATE;
    }

    std::cout << "Successfully parsed SOF-0 segment" << std::endl;

    return SUCCESS;
}

void Decoder::decodeSOSSegment() {
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing SOS segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char*>(&length), 2);
    length = swapBytes(length);

    std::cout << "SOS Segment length: " << length << std::endl;

    length -= 2; // marker bytes included in length

    uint8_t n_components;
    m_ImageFile >> std::noskipws >> n_components;

    if (m_componentSpec.size() < n_components) {
        m_componentSpec.resize(n_components);
    }

    uint8_t c_id, c_ht; // Component ID, Huffman table for component

    for (auto i = 0; i < n_components; i++) {
        m_ImageFile >> std::noskipws >> c_id >> c_ht;

        uint8_t dc_ht = (c_ht >> 4);
        uint8_t ac_ht = (c_ht & 0x0F);

        m_componentSpec[c_id - 1].ac_ht = ac_ht;
        m_componentSpec[c_id - 1].dc_ht = dc_ht;

        std::cout << "Component ID: " << (int)c_id << std::endl;
        std::cout << "Huffman table no.: DC-" << (int)dc_ht << " AC-" << (int)ac_ht << std::endl;
    }

    // skip 3 bytes mandatorily
    m_ImageFile.seekg(3, std::ios_base::cur);

    std::cout << "Successfully parsed SOS segment." << std::endl;

    // Image data immediately follows the SOS segment
    decodeImageData();
}

void Decoder::decodeImageData() {

    if (!(m_ImageFile.is_open() && m_ImageFile.good())) {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Scanning image data" << std::endl;

    uint8_t byte;

    // Read all the image data into memory
    while (m_ImageFile.good()) {
        m_ImageFile >> std::noskipws >> byte;

        if (byte == Markers::JFIF_BYTE_FF) {
            m_ImageFile >> std::noskipws >> byte;
            if (byte == Markers::JFIF_EOI) {
                // Reached end of image
                break;
            } else if (byte == Markers::JFIF_BYTE_0) {
                // FF 00 - byte stuffed FF
                m_scanData.push_back(Markers::JFIF_BYTE_FF);
            }
        } else {
            m_scanData.push_back(byte);
        }
    }

    // start decoding
    auto n_components = m_image.n_components;

    int mcu_count = ((m_image.height + 7) / 8) * ((m_image.width + 7) / 8);

    BitReader bit_reader(m_scanData);

    // vector of MCUs with 3 components each
    std::vector<std::array<std::array<int16_t, 64>, 3>> blocks(mcu_count);

    // track it seperately per component, assuming 3 components here
    std::array<int16_t, 3> current_dc = {0, 0, 0};

    for (int mcu_idx = 0; mcu_idx < mcu_count; mcu_idx++) {
        std::array<std::array<int16_t, 64>, 3>& current_mcu = blocks[mcu_idx];
        // for each mcu, go through each component
        for (int comp_idx = 0; comp_idx < n_components; comp_idx++) {
            uint8_t dc_ht = m_componentSpec[comp_idx].dc_ht;
            uint8_t ac_ht = m_componentSpec[comp_idx].ac_ht;

            // DC Coefficient
            uint8_t huff_code = m_huffmanTables[0][dc_ht]->decodeBitstream(bit_reader);

            if (huff_code == 255) {
                std::cerr << "ERROR: No valid huffman code found in bitstream";
                return;
            }

            uint8_t val_categ = huff_code;
            uint16_t bit_repr = bit_reader.read_bits(val_categ);
            int16_t dc_diff = convertBitReprIntoValue(val_categ, bit_repr);

            current_dc[comp_idx] += dc_diff;
            current_mcu[comp_idx][0] = current_dc[comp_idx];

            // AC coefficients
            int ac_coeff_idx = 0;
            while (ac_coeff_idx < 63) {
                uint8_t huff_code = m_huffmanTables[1][ac_ht]->decodeBitstream(bit_reader);

                if (huff_code == 255) {
                    std::cerr << "ERROR: No valid huffman code found in bitstream";
                    return;
                }

                if (huff_code == 0) {
                    // EOB (End of block) encountered
                    break;
                }

                uint8_t no_prev_zeroes = huff_code >> 4;
                uint8_t val_categ = huff_code & 0xF;

                if (no_prev_zeroes == 15 && val_categ == 0) {
                    // handle 0xF0, (ZRL) case explicitly
                    ac_coeff_idx += 16;
                    continue;
                }

                if (no_prev_zeroes > 0) {
                    ac_coeff_idx += no_prev_zeroes;
                    // blocks array is already filled with zeroes by default
                }

                if (ac_coeff_idx >= 63 || val_categ == 0) {
                    std::cerr << "ERROR: Invalid AC coefficient run";
                    return;
                }

                uint16_t bit_repr = bit_reader.read_bits(val_categ);
                int16_t ac_coeff = convertBitReprIntoValue(val_categ, bit_repr);

                current_mcu[comp_idx][ac_coeff_idx + 1] = ac_coeff;
                ac_coeff_idx++;
            }

            // dequantize
            uint8_t quant_table_no = m_componentSpec[comp_idx].quant_table_no;
            for (int i = 0; i < 64; i++) {
                int coeff = current_mcu[comp_idx][i];
                current_mcu[comp_idx][i] = coeff * m_quantizationTables[quant_table_no][i];
            }

            // undo zig zag transform
            undoZigzagTransform(current_mcu[comp_idx]);

            // inverse dct
            std::array<int16_t, 64> mcu_out = {};
            idct(current_mcu[comp_idx].data(), mcu_out.data());
            current_mcu[comp_idx] = mcu_out;

            // level shift
            for (int i = 0; i < 64; i++) {
                int value = current_mcu[comp_idx][i] + 128;
                current_mcu[comp_idx][i] = std::clamp(value, 0, 255);
            }
        }

        // assuming 3 components and that they are y, cb, cr
        // convert to rgb

        for (int i = 0; i < 64; i++) {
            uint8_t y = current_mcu[0][i];
            uint8_t cb = current_mcu[1][i];
            uint8_t cr = current_mcu[2][i];

            uint8_t r = std::clamp((int)(y + 1.402 * (cr - 128)), 0, 255);
            uint8_t g =
                std::clamp((int)(y - 0.344136 * (cb - 128) - 0.714136 * (cr - 128)), 0, 255);
            uint8_t b = std::clamp((int)(y + 1.772 * (cb - 128)), 0, 255);

            current_mcu[0][i] = r;
            current_mcu[1][i] = g;
            current_mcu[2][i] = b;
        }
    }

    m_image.ingestFromMCU(blocks);
}
