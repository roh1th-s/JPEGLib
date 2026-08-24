#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <cstdint>
#include <memory>
#include <vector>

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
    // Stores the actual symbols found in the DHT segment
    std::vector<uint8_t> huffval;

    // The boundary arrays (always size 17 to match lengths 1 to 16)
    // We use size 17 so length 'i' matches index 'i' directly
    uint32_t mincode[17] = {0};
    uint32_t maxcode[17] = {0};

    // Pointer offsets into the huffval vector for each length
    int32_t valptr[17] = {0};

    static std::shared_ptr<HuffmanTable> fromHuffmanTree(std::shared_ptr<HuffmanTree>);
};

#endif // HUFFMAN_HPP
