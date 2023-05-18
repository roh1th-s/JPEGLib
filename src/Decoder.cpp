#include "Decoder.hpp"
#include "Util.hpp"
#include "Markers.hpp"

Decoder::Decoder()
{
}

Decoder::~Decoder()
{
}

void Decoder::open(std::string fileName)
{
    m_ImageFile.open(fileName, std::ios::binary | std::ios::in);

    if (m_ImageFile.fail())
    {
        std::cout << "Failed to open file : " << fileName;
        return;
    }
}

void Decoder::decode()
{
    uint8_t byte;
    while (m_ImageFile.good())
    {
        m_ImageFile >> std::noskipws >> byte;

        if (byte == Markers::JFIF_BYTE_FF)
        {
            m_ImageFile >> std::noskipws >> byte;

            // choose decoding function based on the byte after ff
            Decoder::Result result = parseSegment(byte);

            std::cout << "\n";

            if (result == TERMINATE)
            {
                std::cout << "Error decoding file, terminating decoder.";
                return;
            }
        }
    }
}

Decoder::Result Decoder::parseSegment(uint8_t byte)
{
    if (byte == Markers::JFIF_BYTE_0 || byte == Markers::JFIF_BYTE_FF)
        return TERMINATE;

    switch (byte)
    {
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
        std::cout << "Found segment, Start of Frame 1: Extended Sequential DCT (FFC1), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF2:
        std::cout << "Found segment, Start of Frame 2: Progressive DCT (FFC2), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF3:
        std::cout << "Found segment, Start of Frame 3: Lossless Sequential (FFC3), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF5:
        std::cout << "Found segment, Start of Frame 5: Differential Sequential DCT (FFC5), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF6:
        std::cout << "Found segment, Start of Frame 6: Differential Progressive DCT (FFC6), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF7:
        std::cout << "Found segment, Start of Frame 7: Differential lossless (Sequential) (FFC7), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF9:
        std::cout << "Found segment, Start of Frame 9: Extended Sequential DCT, Arithmetic Coding (FFC9), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF10:
        std::cout << "Found segment, Start of Frame 10: Progressive DCT, Arithmetic Coding (FFCA), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF11:
        std::cout << "Found segment, Start of Frame 11: Lossless (Sequential), Arithmetic Coding (FFCB), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF13:
        std::cout << "Found segment, Start of Frame 13: Differentical Sequential DCT, Arithmetic Coding (FFCD), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF14:
        std::cout << "Found segment, Start of Frame 14: Differentical Progressive DCT, Arithmetic Coding (FFCE), Not supported" << std::endl;
        return TERMINATE;
    case Markers::JFIF_SOF15:
        std::cout << "Found segment, Start of Frame 15: Differentical Lossless (Sequential), Arithmetic Coding (FFCF), Not supported" << std::endl;
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

void Decoder::decodeAPP0Segment()
{
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good()))
    {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing APP-0 segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char *>(&length), 2);
    length = swap_bytes(length); // bytes are stored as big endian, convert to little 

    // skip JFIF\0 string
    m_ImageFile.seekg(5, std::ios_base::cur);

    uint16_t jfifVersion;
    m_ImageFile.read(reinterpret_cast<char *>(&jfifVersion), 2);

    std::cout << "JFIF Version : " << (int)(jfifVersion >> 8) << "."
              << (int)((jfifVersion & 0x00F0) >> 4) << (int)(jfifVersion & 0x000F) << std::endl;

    uint8_t densityUnitByte;
    m_ImageFile >> std::noskipws >> densityUnitByte;

    std::string densityUnit = "";
    switch (densityUnitByte)
    {
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
    m_ImageFile.read(reinterpret_cast<char *>(&horizontalPixelDensity), 2);
    horizontalPixelDensity = swap_bytes(horizontalPixelDensity);

    uint16_t verticalPixelDensity;
    m_ImageFile.read(reinterpret_cast<char *>(&verticalPixelDensity), 2);
    verticalPixelDensity = swap_bytes(verticalPixelDensity);

    std::cout << "Pixel density: " << horizontalPixelDensity << "x" << verticalPixelDensity << std::endl;
    uint8_t horizontalPixelCount, verticalPixelCount;
    m_ImageFile >> std::noskipws >> horizontalPixelCount >> verticalPixelCount;

    // thumbnail data : packed - 24 bit for each pixel (3 channels)
    uint16_t thumbnailDataSize = 3 * horizontalPixelCount * verticalPixelCount;

    if (thumbnailDataSize > 0)
    {
        uint8_t *thumbnailData = new uint8_t(thumbnailDataSize);
        m_ImageFile.read(reinterpret_cast<char *>(thumbnailData), 3 * horizontalPixelCount * verticalPixelCount);
    }

    std::cout << "APP-0 segment parsed successfully." << std::endl;
}

void Decoder::decodeCOMSegment()
{
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good()))
    {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing comment segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char *>(&length), 2);
    length = swap_bytes(length);

    std::cout << "Comment segment length: " << length;

    length -= 2; // length includes markers

    uint8_t *commentData = new uint8_t(length);
    m_ImageFile.read(reinterpret_cast<char *>(commentData), length);

    std::string comment(reinterpret_cast<char const *>(commentData), length);

    std::cout << "Comments: " << comment << std::endl;
    std::cout << "COM segment parsed successfully." << std::endl;
}

void Decoder::decodeDQTSegment()
{
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good()))
    {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing DQT segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char *>(&length), 2);
    length = swap_bytes(length);

    std::cout << "DQT Segment length: " << length << std::endl;

    length -= 2; // marker bytes included in length

    while (length > 0)
    {
        uint8_t qt_info;
        m_ImageFile >> std::noskipws >> qt_info;

        int precision = qt_info >> 4;
        int qt_n = qt_info & 0x0F; // Quantization table number

        std::cout << "QT no: " << qt_n << ", Precision: " << (precision == 0 ? "8-bit" : "16-bit") << std::endl;

        uint8_t *qt_data = new uint8_t[64];
        m_ImageFile.read(reinterpret_cast<char *>(qt_data), 64);

        // TODO: Keep track of these quantization tables somewhere

        length -= 65;
    }

    std::cout << "Successfully parsed DQT segment." << std::endl;
}

void Decoder::decodeDHTSegment()
{
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good()))
    {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing DHT segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char *>(&length), 2);
    length = swap_bytes(length);

    std::cout << "DHT Segment length: " << length;

    length -= 2; // marker bytes included in length

    int endOfSegment = (int)m_ImageFile.tellg() + length;

    while (m_ImageFile.tellg() < endOfSegment)
    {
        uint8_t ht_info;

        m_ImageFile >> std::noskipws >> ht_info;

        int ht_type = ht_info >> 4;
        int ht_no = ht_info & 0x0F;

        int total_sym_count = 0;

        for (int i = 1; i <= 16; i++)
        {
            uint8_t sym_count;
            m_ImageFile >> std::noskipws >> sym_count;

            // TODO: Keep track of this count somewhere
            total_sym_count += (int)sym_count;
        }

        int syms = 0;
        for (auto i = 0; syms < total_sym_count;)
        {
            uint8_t code;
            m_ImageFile >> std::noskipws >> code;

            // TODO: Put this into the huffman table
            syms++;

            // TODO: If all symbols for current symbol length i are read, i++
        }

        // TODO: construct huffman tree and store it
    }

    std::cout << "Successfully parsed Huffman table segment." << std::endl;
}

Decoder::Result Decoder::decodeSOF0Segment()
{
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good()))
    {
        std::cout << "Error parsing file" << std::endl;
        return TERMINATE;
    }

    std::cout << "Parsing Frame-0 segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char *>(&length), 2);
    length = swap_bytes(length);

    std::cout << "SOF-0 Segment length: " << length << std::endl;

    length -= 2; // marker bytes included in length

    uint8_t precision;
    m_ImageFile >> std::noskipws >> precision;

    std::cout << "SOF-0 segment data precision: " << (int)precision << std::endl;

    uint16_t image_height, image_width;
    m_ImageFile.read(reinterpret_cast<char *>(&image_height), 2);
    m_ImageFile.read(reinterpret_cast<char *>(&image_width), 2);
    image_height = swap_bytes(image_height);
    image_width = swap_bytes(image_width);

    std::cout << "Image height: " << (int)image_height
              << " Image width: " << (int)image_width << std::endl;

    uint8_t n_components;
    m_ImageFile >> std::noskipws >> n_components;

    std::cout << "No. of components: " << (int)n_components << std::endl;

    uint8_t comp, sampling_factor, qt_n;

    bool chroma_ss = false; // flag for presence of chroma subsampling

    for (auto i = 0; i < n_components; i++)
    {
        m_ImageFile >> std::noskipws >> comp >> sampling_factor >> qt_n;

        int sf_horizontal = (int)(sampling_factor >> 4);
        int sf_vertical = (int)(sampling_factor & 0x0F);

        std::cout << "Component: " << (int)comp << std::endl;
        std::cout << "Sampling Factor, Horizontal: " << sf_horizontal << ", Vertical: " << sf_vertical << std::endl;
        std::cout << "Quantization table no.: " << (int)qt_n << std::endl;

        if (!(sf_horizontal == 1 && sf_vertical == 1))
        {
            chroma_ss = true;
        }
    }

    if (chroma_ss)
    {
        std::cout << "Chroma subsampling is not supported." << std::endl;
        return TERMINATE;
    }

    std::cout << "Successfully parsed SOF-0 segment" << std::endl;

    return SUCCESS;
}

void Decoder::decodeSOSSegment()
{
    // make sure file is still good to read
    if (!(m_ImageFile.is_open() && m_ImageFile.good()))
    {
        std::cout << "Error parsing file" << std::endl;
        return;
    }

    std::cout << "Parsing SOS segment." << std::endl;

    uint16_t length;
    m_ImageFile.read(reinterpret_cast<char *>(&length), 2);
    length = swap_bytes(length);

    std::cout << "SOS Segment length: " << length;

    length -= 2; // marker bytes included in length

    uint8_t n_component;
    m_ImageFile >> std::noskipws >> n_component;

    uint8_t c_id, c_ht; // Component ID, Huffman table for component

    for (auto i = 0; i < n_component; i++)
    {
        m_ImageFile >> std::noskipws >> c_id >> c_ht;
        // TODO: Check if these bytes are the other way around

        int dc_ht = (int)(c_ht >> 4);
        int ac_ht = (int)(c_ht & 0x0F);

        std::cout << "Component ID: " << (int)c_id << std::endl;
        std::cout << "Huffman table no.: AC-" << ac_ht << " DC-" << dc_ht << std::endl;
    }

    m_ImageFile.seekg(3, std::ios_base::cur);

    std::cout << "Successfully parsed SOS segment." << std::endl;

    // Image data immediately follows the SOS segment
    decodeImageData();
}

void Decoder::decodeImageData()
{
}