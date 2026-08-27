#include "../src/huffman.hpp"
#include "doctest/doctest.h"
#include <vector>

TEST_CASE("Huffman table construction from JFIF data") {
    // Code-length counts:
    //   1-bit: 0
    //   2-bit: 1  -> 'a'
    //   3-bit: 5  -> 'b'..'f'
    //   4-bit: 1  -> 'g'
    //   5-bit: 1  -> 'h'
    //   6-bit: 1  -> 'i'
    //   7-bit: 1  -> 'j'
    //   8-bit: 1  -> 'k'
    std::vector<uint8_t> dht_data = {0, 1, 5,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,  0,
                                     0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};

    auto huffmanTable = HuffmanTable::fromJfifData(dht_data);

    REQUIRE(huffmanTable != nullptr);

    CHECK(huffmanTable->huffval.size() == 11);

    CHECK(huffmanTable->mincode[2] == 0);
    CHECK(huffmanTable->maxcode[2] == 0);
    CHECK(huffmanTable->valptr[2] == 0);

    CHECK(huffmanTable->mincode[3] == 2);
    CHECK(huffmanTable->maxcode[3] == 6);
    CHECK(huffmanTable->valptr[3] == 1);

    CHECK(huffmanTable->huffval[0] == 'a');
    CHECK(huffmanTable->huffval[1] == 'b');
    CHECK(huffmanTable->huffval[5] == 'f');
}

TEST_CASE("HuffmanNode - Child Insertion & Parent Linkage") {
    auto root = std::make_shared<HuffmanNode>();

    SUBCASE("insertLeftChild assigns left node and parent link") {
        auto left = root->insertLeftChild();
        REQUIRE(left != nullptr);
        CHECK(root->left == left);
        CHECK(left->parent.lock() == root);

        // Sub-call idempotency check
        CHECK(root->insertLeftChild() == left);
    }

    SUBCASE("insertRightChild assigns right node and parent link") {
        auto right = root->insertRightChild();
        REQUIRE(right != nullptr);
        CHECK(root->right == right);
        CHECK(right->parent.lock() == root);

        // Sub-call idempotency check
        CHECK(root->insertRightChild() == right);
    }
}

TEST_CASE("HuffmanNode - Level Traversal with getNextOnLevel") {
    auto root = std::make_shared<HuffmanNode>();
    auto left = root->insertLeftChild();
    auto right = root->insertRightChild();

    CHECK(root->getNextOnLevel() == nullptr);
    CHECK(left->getNextOnLevel() == right);
    CHECK(right->getNextOnLevel() == nullptr);

    SUBCASE("Level 2 subtree cross-traversal") {
        auto l_left = left->insertLeftChild();
        auto l_right = left->insertRightChild();
        auto r_left = right->insertLeftChild();
        auto r_right = right->insertRightChild();

        CHECK(l_left->getNextOnLevel() == l_right);
        CHECK(l_right->getNextOnLevel() == r_left);
        CHECK(r_left->getNextOnLevel() == r_right);
        CHECK(r_right->getNextOnLevel() == nullptr);
    }
}

TEST_CASE("HuffmanTree - Complete Tree Mapping Verification") {
    // 16 length counts (Sum = 11 symbols):
    // Length 1: 0, Length 2: 1 ('a'), Length 3: 5 ('b','c','d','e','f')
    // Length 4: 1 ('g'), Length 5: 1 ('h'), Length 6: 1 ('i')
    // Length 7: 1 ('j'), Length 8: 1 ('k'), Lengths 9-16: 0
    std::vector<uint8_t> dht_data = {0, 1, 5,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,  0,
                                     0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};

    auto tree = HuffmanTree::fromJfifData(dht_data);

    REQUIRE(tree != nullptr);
    REQUIRE(tree->root != nullptr);

    // Track nodes level-by-level
    // Level 1: Root children
    auto L1_left = tree->root->left;
    auto L1_right = tree->root->right;

    // --- Level 2 (1 Symbol: 'a') ---
    // Left branch gets the symbol 'a'; Right branch continues down
    auto L2_left = L1_left->left;
    auto L2_right = L1_left->right;

    CHECK(L2_left->symbol == 'a');

    // --- Level 3 (5 Symbols: 'b', 'c', 'd', 'e', 'f') ---
    // L2_right spawns (b, c)
    auto L3_node1 = L2_right->left;  // 'b'
    auto L3_node2 = L2_right->right; // 'c'
    // L1_right spawns (d, e) and (f, branch_node)
    auto L3_node3 = L1_right->left->left;  // 'd'
    auto L3_node4 = L1_right->left->right; // 'e'
    auto L3_node5 = L1_right->right->left; // 'f'
    auto L3_next = L1_right->right->right; // Internal node extending to L4

    CHECK(L3_node1->symbol == 'b');
    CHECK(L3_node2->symbol == 'c');
    CHECK(L3_node3->symbol == 'd');
    CHECK(L3_node4->symbol == 'e');
    CHECK(L3_node5->symbol == 'f');

    // --- Level 4 (1 Symbol: 'g') ---
    auto L4_node1 = L3_next->left; // 'g'
    auto L4_next = L3_next->right; // Internal node extending to L5

    CHECK(L4_node1->symbol == 'g');

    // --- Level 5 (1 Symbol: 'h') ---
    auto L5_node1 = L4_next->left; // 'h'
    auto L5_next = L4_next->right; // Internal node extending to L6

    CHECK(L5_node1->symbol == 'h');

    // --- Level 6 (1 Symbol: 'i') ---
    auto L6_node1 = L5_next->left; // 'i'
    auto L6_next = L5_next->right; // Internal node extending to L7

    CHECK(L6_node1->symbol == 'i');

    // --- Level 7 (1 Symbol: 'j') ---
    auto L7_node1 = L6_next->left; // 'j'
    auto L7_next = L6_next->right; // Internal node extending to L8

    CHECK(L7_node1->symbol == 'j');

    // --- Level 8 (1 Symbol: 'k') ---
    auto L8_node1 = L7_next->left; // 'k'

    CHECK(L8_node1->symbol == 'k');
}
