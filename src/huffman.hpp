
#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <cstdint>
#include <memory>
#include <vector>

class HuffmanNode : public std::enable_shared_from_this<HuffmanNode> {
  public:
    HuffmanNode() = default;

    uint8_t m_symbol;
    std::shared_ptr<HuffmanNode> m_left;
    std::shared_ptr<HuffmanNode> m_right;
    std::weak_ptr<HuffmanNode> m_parent;

    std::shared_ptr<HuffmanNode> getNextOnLevel() const;
    std::shared_ptr<HuffmanNode> insertLeftChild();
    std::shared_ptr<HuffmanNode> insertRightChild();
};

class HuffmanTree {
  public:
    HuffmanTree();

    std::vector<uint8_t> decodeData();
    static std::shared_ptr<HuffmanTree> fromJfifData(std::vector<uint8_t>);

    std::shared_ptr<HuffmanNode> m_root;
};

#endif // HUFFMAN_HPP
