#include "bitreader.hpp"

#include <cstdint>

// Refills the accumulator to ensure we have enough bits
void BitReader::fill_buffer() {
    // Keep loading bytes until our 64-bit buffer is mostly full.
    // We stop at 56 so we can safely shift an 8-bit byte in without overflow.
    while (bits_left <= 56 && byte_pos < data_size) {
        // Note: JPEG is MSB-first (Most Significant Bit first).
        // We shift the new byte into the bottom of the buffer.
        bit_buffer = (bit_buffer << 8) | data[byte_pos++];
        bits_left += 8;
    }
}

// Look at the next N bits WITHOUT consuming them (crucial for Huffman decoding)
uint32_t BitReader::peek_bits(int count) {
    if (bits_left < count)
        fill_buffer();
    if (bits_left < count)
        return 0; // Or throw

    // Shift the requested bits to the far right, mask off the rest
    return (bit_buffer >> (bits_left - count)) & ((1ULL << count) - 1);
}

// Actually consume the bits we just peeked at
void BitReader::consume_bits(int count) {
    bits_left -= count;
    // Optional: clear the consumed bits from the buffer to keep it clean
    // bit_buffer &= (1ULL << bits_left) - 1;
}

// Read and consume in one step
uint32_t BitReader::read_bits(int count) {
    uint32_t val = peek_bits(count);
    consume_bits(count);
    return val;
};
