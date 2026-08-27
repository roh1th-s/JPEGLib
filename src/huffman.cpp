#include "huffman.hpp"
#include "bitreader.hpp"
#include <cstdint>
#include <memory>

// HuffmanNode
std::shared_ptr<HuffmanNode> HuffmanNode::getNextOnLevel() const {
    if (this->parent.expired())
        return nullptr;

    auto parent = this->parent.lock();

    if (this == parent->left.get()) {
        // if this is a left child, return right child
        return parent->right;
    }

    auto nextOnParentsLevel = parent->getNextOnLevel();
    if (!nextOnParentsLevel) {
        return nullptr;
    } else {
        return nextOnParentsLevel->left;
    }
}

std::shared_ptr<HuffmanNode> HuffmanNode::insertLeftChild() {
    if (this->left)
        return this->left;

    auto child = std::make_shared<HuffmanNode>();
    child->parent = shared_from_this();

    this->left = child;

    return child;
}

std::shared_ptr<HuffmanNode> HuffmanNode::insertRightChild() {
    if (this->right)
        return this->right;

    auto child = std::make_shared<HuffmanNode>();
    child->parent = shared_from_this();

    this->right = child;

    return child;
}

// HuffmanTree
// Tree logic only for debug and testing purposes, not used in actual decoding
HuffmanTree::HuffmanTree() {
    auto root = std::make_shared<HuffmanNode>();
    this->root = root;
}

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
    auto root = tree->root;

    root->insertLeftChild();
    root->insertRightChild();

    auto leftmost = root->left;

    // track sym_idx across all symbol lengths
    int sym_idx = 0;

    for (int sym_len = 1; sym_len <= 16; sym_len++) {
        int sym_count = sym_counts[sym_len - 1];

        if (sym_count > 0) {
            int end_idx = sym_idx + sym_count;
            // assign symbols to tree nodes going from left to right
            for (; sym_idx < end_idx; sym_idx++) {
                uint8_t symbol = syms[sym_idx];

                leftmost->symbol = symbol;

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

        leftmost = current->left;

        while ((current = current->getNextOnLevel())) {
            if (!current)
                break;

            current->insertLeftChild();
            current->insertRightChild();
        }
    }

    return tree;
}

// HuffmanTable
// expects jfif data as <16 bytes of symbol count> <n bytes of symbols>
std::shared_ptr<HuffmanTable> HuffmanTable::fromJfifData(std::vector<uint8_t> dht_data) {
    int curr_idx = 0;

    auto h_table = std::make_shared<HuffmanTable>();

    uint16_t current_code = 0;
    int total_syms = 0;

    for (int sym_len = 1; sym_len <= 16; sym_len++) {
        uint8_t sym_count = dht_data[curr_idx++];

        if (sym_count > 0) {
            h_table->valptr[sym_len] = total_syms;
            h_table->mincode[sym_len] = current_code;
            h_table->maxcode[sym_len] = current_code + sym_count - 1;
            total_syms += sym_count;
        } else {
            h_table->valptr[sym_len] = -1;
            h_table->mincode[sym_len] = -1;
            h_table->maxcode[sym_len] = -1;
        }

        current_code = (current_code + sym_count) << 1;
    }

    for (int i = 0; i < total_syms; i++) {
        uint8_t sym = dht_data[curr_idx++];
        h_table->huffval.push_back(sym);
    }

    return h_table;
}

uint8_t HuffmanTable::decodeBitstream(BitReader& bitReader) {
    // try lengths in order
    for (int len = 1; len <= 16; len++) {
        // important that this is interpreted as signed int for maxcode comparison
        int32_t code = bitReader.peek_bits(len);

        // check h_table
        if (code <= maxcode[len]) {
            // consume bits
            bitReader.consume_bits(len);

            auto offset = valptr[len] + (code - mincode[len]);

            return huffval[offset];
        }
    }

    return -1;
}
