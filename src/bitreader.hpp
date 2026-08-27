#ifndef BITREADER_HPP
#define BITREADER_HPP

#include <cstdint>
#include <vector>

class BitReader {
  private:
    const uint8_t* data; // Pointer to the start of the data
    size_t data_size;    // Total number of bytes
    size_t byte_pos = 0;

    uint64_t bit_buffer = 0; // The accumulator
    int bits_left = 0;       // How many bits in the accumulator are valid

    void fill_buffer();

  public:
    explicit BitReader(const std::vector<uint8_t>& buffer)
        : data(buffer.data()), data_size(buffer.size()) {}

    uint32_t peek_bits(int count);
    void consume_bits(int count);
    uint32_t read_bits(int count);
};
#endif // BITREADER_HPP
