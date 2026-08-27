#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include "bitreader.hpp"

struct HuffmanNode : public std::enable_shared_from_this<HuffmanNode> {
  public:
    HuffmanNode() = default;

    uint8_t symbol;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;
    std::weak_ptr<HuffmanNode> parent;

    std::shared_ptr<HuffmanNode> getNextOnLevel() const;
    std::shared_ptr<HuffmanNode> insertLeftChild();
    std::shared_ptr<HuffmanNode> insertRightChild();
};

struct HuffmanTree {
  public:
    HuffmanTree();
    static std::shared_ptr<HuffmanTree> fromJfifData(std::vector<uint8_t>);

    std::shared_ptr<HuffmanNode> root;
};

struct HuffmanTable {
    // stores symbols
    std::vector<uint8_t> huffval;

    // these arrays define the range of codes for each length, mincode to maxcode
    int32_t mincode[17] = {0};
    int32_t maxcode[17] = {0};

    // valptr[i] is the index of the first symbol of length i in huffval
    int32_t valptr[17] = {0};

    static std::shared_ptr<HuffmanTable> fromJfifData(std::vector<uint8_t>);

    uint8_t decodeBitstream(BitReader& bitReader);
};

#endif // HUFFMAN_HPP
