#include "util.hpp"

#ifdef _WIN32
#include <winsock2.h> // htons
#else
#include <arpa/inet.h>
#endif

uint16_t swapBytes(uint16_t bytes) { return (uint16_t)htons(bytes); }

// Convert a zig-zag order index to matrix indices (row, column)
const std::pair<const int, const int> zzOrderToMatIndices(const int zzindex) {
    static const std::pair<const int, const int> zzorder[64] = {
        {0, 0}, {0, 1}, {1, 0}, {2, 0}, {1, 1}, {0, 2}, {0, 3}, {1, 2}, {2, 1}, {3, 0}, {4, 0},
        {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0}, {6, 0},
        {5, 1}, {4, 2}, {3, 3}, {2, 4}, {1, 5}, {0, 6}, {0, 7}, {1, 6}, {2, 5}, {3, 4}, {4, 3},
        {5, 2}, {6, 1}, {7, 0}, {7, 1}, {6, 2}, {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7}, {2, 7},
        {3, 6}, {4, 5}, {5, 4}, {6, 3}, {7, 2}, {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7}, {4, 7},
        {5, 6}, {6, 5}, {7, 4}, {7, 5}, {6, 6}, {5, 7}, {6, 7}, {7, 6}, {7, 7}};

    return zzorder[zzindex];
}

// Convert matrix indices (row, column) to zig-zag order index
const int matIndicesToZZOrder(const int row, const int column) {
    static const int matOrder[8][8] = {
        {0, 1, 5, 6, 14, 15, 27, 28},     {2, 4, 7, 13, 16, 26, 29, 42},
        {3, 8, 12, 17, 25, 30, 41, 43},   {9, 11, 18, 24, 31, 40, 44, 53},
        {10, 19, 23, 32, 39, 45, 52, 54}, {20, 22, 33, 38, 46, 51, 55, 60},
        {21, 34, 37, 47, 50, 56, 59, 61}, {35, 36, 48, 49, 57, 58, 62, 63}};

    return matOrder[row][column];
}

void undoZigzagTransform(std::array<int16_t, 64>& block) {
    constexpr uint8_t ZIGZAG_ORDER[64] = {
        0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33, 40, 48,
        41, 34, 27, 20, 13, 6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23,
        30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};

    std::array<int16_t, 64> temp;

    for (int i = 0; i < 64; ++i) {
        temp[ZIGZAG_ORDER[i]] = block[i];
    }
    block = temp;
}

int16_t convertBitReprIntoValue(uint8_t value_categ, uint16_t bit_repr) {
    if (value_categ == 0)
        return 0; // Optional, but explicitly handles the 0 case early

    // 2 ^ value_categ
    int max = 1 << value_categ;

    if (bit_repr >= (max / 2)) {
        return bit_repr;
    } else {
        // Mathematically identical to your code, just slightly easier to read
        return bit_repr - (max - 1);
    }
}
