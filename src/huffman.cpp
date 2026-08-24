#include "huffman.hpp"
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

HuffmanTree::HuffmanTree() {
    auto root = std::make_shared<HuffmanNode>();
    this->root = root;
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
// TODO: This double work isnt necessary, you can create the huffman table directly
// from the jfif dht data without constructing a tree first
std::shared_ptr<HuffmanTable> HuffmanTable::fromHuffmanTree(std::shared_ptr<HuffmanTree> h_tree) {
    auto h_table = std::make_shared<HuffmanTable>();

    auto root = h_tree->root;

    auto leftmost = root->left;

    int sym_len = 1;
    uint32_t current_code = 0; // tracks huffman code

    while (leftmost != nullptr) {
        auto current = leftmost;

        // pointer to start of symbols of current length
        int start_ptr = h_table->huffval.size();

        int n_syms = 0;

        while (current) {
            bool has_symbol = !current->left && !current->right;

            if (!has_symbol)
                break;

            h_table->huffval.push_back(current->symbol);

            n_syms++;

            if (n_syms == 1) {
                // if its first symbol
                h_table->mincode[sym_len] = current_code;
            }
            current = current->getNextOnLevel();
            current_code++;
        }

        if (n_syms > 0) {
            // if there were any symbols of this length
            h_table->maxcode[sym_len] = current_code;
            h_table->valptr[sym_len] = start_ptr;
        } else {
            h_table->mincode[sym_len] = -1;
            h_table->maxcode[sym_len] = -1;
            h_table->valptr[sym_len] = -1;
        }

        // append a 0 to the end of the current code
        current_code <<= 1;

        if (!current) {
            // we've reached the end of a level in a tree without any more children
            break;
        }

        leftmost = current->left;
    }

    return h_table;
}
