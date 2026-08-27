#include "../src/dct.hpp"
#include "doctest/doctest.h"
#include <cmath>

TEST_CASE("DCT - Zero Block Stability") {
    int16_t input[64] = {0};
    int16_t coeffs[64] = {0};
    int16_t output[64] = {0};

    SUBCASE("FDCT of all zeros is all zeros") {
        fdct(input, coeffs);
        for (int i = 0; i < 64; ++i) {
            CHECK(coeffs[i] == 0);
        }
    }

    SUBCASE("IDCT of all zeros is all zeros") {
        idct(coeffs, output);
        for (int i = 0; i < 64; ++i) {
            CHECK(output[i] == 0);
        }
    }
}

TEST_CASE("DCT - Constant Block (DC Only)") {
    int16_t input[64];
    int16_t coeffs[64] = {0};
    int16_t output[64] = {0};

    // Fill block with a constant value (e.g., 100)
    for (int i = 0; i < 64; ++i) {
        input[i] = 100;
    }

    SUBCASE("FDCT generates only a DC coefficient") {
        fdct(input, coeffs);

        // For a constant block of value X, the DC coefficient is exactly X * 8.
        CHECK(coeffs[0] == 800);

        // All AC coefficients must be 0
        for (int i = 1; i < 64; ++i) {
            CHECK(coeffs[i] == 0);
        }
    }

    SUBCASE("IDCT perfectly restores a pure DC block") {
        // Manually set a DC coefficient of 800 (equivalent to constant 100)
        coeffs[0] = 800;
        for (int i = 1; i < 64; ++i)
            coeffs[i] = 0;

        idct(coeffs, output);

        for (int i = 0; i < 64; ++i) {
            CHECK(output[i] == 100);
        }
    }
}

TEST_CASE("DCT - Forward and Inverse Roundtrip Lossless Check") {
    int16_t input[64];
    int16_t coeffs[64] = {0};
    int16_t output[64] = {0};

    // Create a predictable gradient block (simulating level-shifted pixel data)
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            // Values ranging roughly from -128 to 127
            input[y * 8 + x] = (x * 15) + (y * 5) - 100;
        }
    }

    fdct(input, coeffs);
    idct(coeffs, output);

    // Verify that the reconstructed block matches the input
    // Note: Due to std::lround and floating point operations, a tolerance of +/- 1
    // is standard and acceptable in JPEG integer DCT verification.
    for (int i = 0; i < 64; ++i) {
        int diff = std::abs(input[i] - output[i]);
        CHECK(diff <= 1);
    }
}

TEST_CASE("DCT - Alternating High-Frequency Pattern") {
    int16_t input[64];
    int16_t coeffs[64] = {0};
    int16_t output[64] = {0};

    // Create a checkerboard pattern (maximum high frequency)
    for (int i = 0; i < 64; ++i) {
        input[i] = (i % 2 == 0) ? 50 : -50;
    }

    fdct(input, coeffs);
    idct(coeffs, output);

    // Verify roundtrip for extreme high-frequency changes
    for (int i = 0; i < 64; ++i) {
        int diff = std::abs(input[i] - output[i]);
        CHECK(diff <= 1);
    }
}
