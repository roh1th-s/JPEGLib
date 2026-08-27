#include "image.hpp"

Image::Image(/* args */) {}
Image::~Image() {}

std::vector<uint8_t>& Image::component(uint8_t c) { return m_data.at(c); }

uint8_t Image::at(int x, int y, uint8_t c) { return m_data[c][x + y * width]; }

void Image::ingestFromMCU(const std::vector<std::array<std::array<int16_t, 64>, 3>>& blocks) {
    m_data.resize(n_components);

    for (int c = 0; c < n_components; c++) {
        m_data[c].resize(width * height);
    }

    int mcus_per_row = (width + 7) / 8;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int mcu_x = x / 8;
            int mcu_y = y / 8;
            int mcu_idx = mcu_y * mcus_per_row + mcu_x;

            int block_x = x % 8;
            int block_y = y % 8;
            int block_idx = block_y * 8 + block_x;

            for (int c = 0; c < n_components; c++) {
                m_data[c][x + y * width] = blocks[mcu_idx][c][block_idx];
            }
        }
    }
}
