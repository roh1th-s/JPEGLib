#ifndef UTIL_HPP
#define UTIL_HPP

#include <array>
#include <cstdint>
#include <utility>

uint16_t swapBytes(uint16_t bytes);

const std::pair<const int, const int> zzOrderToMatIndices(const int zzindex);

const int matIndicesToZZOrder(const int row, const int column);

void undoZigzagTransform(std::array<int16_t, 64>& block);
void zigzagTransform(std::array<int16_t, 64>& block);

int16_t convertBitReprIntoValue(uint8_t value_categ, uint16_t bit_repr);

#endif // UTIL_HPP
