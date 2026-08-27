#include <fstream>
#include <iostream>
#include <string>

#include "decoder.hpp"

int main(int argc, char const* argv[]) {
    if (argc <= 1) {
        std::cout << "Please enter a file name!";
        return 1;
    }

    std::string fileName = argv[1];

    Decoder jpegDecoder;

    jpegDecoder.open(fileName);
    Decoder::Result res = jpegDecoder.decode();

    if (res == Decoder::TERMINATE) {
        return 1;
    }

    // write jpegDecoder.m_image to a file
    std::ofstream outFile;
    outFile.open(fileName + ".ppm", std::ios::binary | std::ios::out);

    if (outFile.fail()) {
        std::cout << "Failed to open output file : " << fileName + ".ppm";
        return 1;
    }

    outFile << "P6\n"
            << jpegDecoder.m_image.width << " " << jpegDecoder.m_image.height << "\n255\n";

    for (int y = 0; y < jpegDecoder.m_image.height; y++) {
        for (int x = 0; x < jpegDecoder.m_image.width; x++) {
            outFile << jpegDecoder.m_image.at(x, y, 0) << jpegDecoder.m_image.at(x, y, 1)
                    << jpegDecoder.m_image.at(x, y, 2);
        }
    }

    outFile.close();

    return 0;
}
