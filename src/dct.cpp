#include "dct.hpp"

#include <cmath>

// TODO: Optimize dct routines

void fdct(const int16_t mcu[64], int16_t dct_out[64]) {
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {

            double sum = 0;
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    double val = mcu[y * 8 + x];
                    double c1 = cos((2 * x + 1) * u * M_PI / 16);
                    double c2 = cos((2 * y + 1) * v * M_PI / 16);

                    sum += val * c1 * c2;
                }
            }

            double c_u = u == 0 ? ONE_OVER_SQRT2 : 1.0;
            double c_v = v == 0 ? ONE_OVER_SQRT2 : 1.0;

            double val = 0.25 * c_u * c_v * sum;

            dct_out[v * 8 + u] = static_cast<int16_t>(std::lround(val));
        }
    }
}

void idct(const int16_t dct_coeffs[64], int16_t mcu_out[64]) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {

            double sum = 0;
            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    double coeff = dct_coeffs[v * 8 + u];
                    double c1 = cos((2 * x + 1) * u * M_PI / 16);
                    double c2 = cos((2 * y + 1) * v * M_PI / 16);

                    double c_u = u == 0 ? ONE_OVER_SQRT2 : 1.0;
                    double c_v = v == 0 ? ONE_OVER_SQRT2 : 1.0;
                    sum += c_u * c_v * coeff * c1 * c2;
                }
            }

            double val = 0.25 * sum;

            mcu_out[y * 8 + x] = static_cast<int16_t>(std::lround(val));
        }
    }
}
