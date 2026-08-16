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
    jpegDecoder.decode();

    return 0;
}
