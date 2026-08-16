#include "huffman.hpp"
#include <cstdint>
#include <memory>

// HuffmanNode
std::shared_ptr<HuffmanNode> HuffmanNode::getNextOnLevel() const {
    if (this->m_parent.expired())
        return nullptr;

    auto parent = this->m_parent.lock();

    if (this == parent->m_left.get()) {
        // if this is a left child, return right child
        return parent->m_right;
    }

    auto nextOnParentsLevel = parent->getNextOnLevel();
    if (!nextOnParentsLevel) {
        return nullptr;
    } else {
        return nextOnParentsLevel->m_left;
    }
}

std::shared_ptr<HuffmanNode> HuffmanNode::insertLeftChild() {
    if (this->m_left)
        return this->m_left;

    auto child = std::make_shared<HuffmanNode>();
    child->m_parent = shared_from_this();

    this->m_left = child;

    return child;
}

std::shared_ptr<HuffmanNode> HuffmanNode::insertRightChild() {
    if (this->m_right)
        return this->m_right;

    auto child = std::make_shared<HuffmanNode>();
    child->m_parent = shared_from_this();

    this->m_right = child;

    return child;
}

// HuffmanTree

HuffmanTree::HuffmanTree() {
    auto root = std::make_shared<HuffmanNode>();
    this->m_root = root;
}

// expects jfif data as <16 bytes of symbol count> <n bytes of symbols>
std::shared_ptr<HuffmanTree> HuffmanTree::fromJfifData(std::vector<uint8_t> dht_data) {
    int curr_idx = 0;

    int total_sym_count = 0;
    uint8_t sym_counts[16] = {0};

    for (int i = 0; i < 16; i++) {
        uint8_t sym_count = dht_data[curr_idx++];
        sym_counts[i] = sym_count;
        total_sym_count += (int)sym_count;
    }

    std::vector<uint8_t> syms(total_sym_count);

    for (auto i = 0; i < total_sym_count; i++) {
        uint8_t sym = dht_data[curr_idx++];
        syms[i] = sym;
    }

    // build huffman tree

    auto tree = std::make_shared<HuffmanTree>();
    auto root = tree->m_root;

    root->insertLeftChild();
    root->insertRightChild();

    auto leftmost = root->m_left;

    // track sym_idx across all symbol lengths
    int sym_idx = 0;

    for (int sym_len = 1; sym_len <= 16; sym_len++) {
        int sym_count = sym_counts[sym_len - 1];

        if (sym_count > 0) {
            int end_idx = sym_idx + sym_count;
            // assign symbols to tree nodes going from left to right
            for (; sym_idx < end_idx; sym_idx++) {
                uint8_t symbol = syms[sym_idx];

                leftmost->m_symbol = symbol;

                leftmost = leftmost->getNextOnLevel();
            }
        }

        if (!leftmost) {
            // we've reached end of this level, exit loop
            break;
        }
        // create children of remaining nodes on current level
        auto current = leftmost;

        current->insertLeftChild();
        current->insertRightChild();

        leftmost = current->m_left;

        while ((current = current->getNextOnLevel())) {
            if (!current)
                break;

            current->insertLeftChild();
            current->insertRightChild();
        }
    }

    return tree;
}

std::vector<uint8_t> HuffmanTree::decodeData() {
    // compiler should prevent copying when returning a vector
}
