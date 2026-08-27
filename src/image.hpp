#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <array>
#include <cstdint>
#include <vector>

class Image {
  public:
    Image();
    ~Image();

    uint32_t width;
    uint32_t height;
    uint8_t n_components;

    // one vector of bytes per component
    std::vector<std::vector<uint8_t>> m_data;

    std::vector<uint8_t>& component(uint8_t c);
    uint8_t at(int x, int y, uint8_t c);

    void ingestFromMCU(const std::vector<std::array<std::array<int16_t, 64>, 3>>& blocks);

  private:
};

#endif
