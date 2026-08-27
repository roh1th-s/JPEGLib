#ifndef DCT_HPP
#define DCT_HPP

#include <cstdint>

#define ONE_OVER_SQRT2 0.7071067811865476 // 1/sqrt(2)
#define M_PI 3.14159265358979323846

void fdct(const int16_t mcu[64], int16_t dct_out[64]);
void idct(const int16_t dct_coeffs[64], int16_t mcu_out[64]);

#endif // DCT_HPP
