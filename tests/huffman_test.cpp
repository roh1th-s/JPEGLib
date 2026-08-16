#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/huffman.hpp"
#include "doctest/doctest.h"
#include <vector>

TEST_CASE("HuffmanNode - Child Insertion & Parent Linkage") {
    auto root = std::make_shared<HuffmanNode>();

    SUBCASE("insertLeftChild assigns left node and parent link") {
        auto left = root->insertLeftChild();
        REQUIRE(left != nullptr);
        CHECK(root->m_left == left);
        CHECK(left->m_parent.lock() == root);

        // Sub-call idempotency check
        CHECK(root->insertLeftChild() == left);
    }

    SUBCASE("insertRightChild assigns right node and parent link") {
        auto right = root->insertRightChild();
        REQUIRE(right != nullptr);
        CHECK(root->m_right == right);
        CHECK(right->m_parent.lock() == root);

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
    REQUIRE(tree->m_root != nullptr);

    // Track nodes level-by-level
    // Level 1: Root children
    auto L1_left = tree->m_root->m_left;
    auto L1_right = tree->m_root->m_right;

    // --- Level 2 (1 Symbol: 'a') ---
    // Left branch gets the symbol 'a'; Right branch continues down
    auto L2_left = L1_left->m_left;
    auto L2_right = L1_left->m_right;

    CHECK(L2_left->m_symbol == 'a');

    // --- Level 3 (5 Symbols: 'b', 'c', 'd', 'e', 'f') ---
    // L2_right spawns (b, c)
    auto L3_node1 = L2_right->m_left;  // 'b'
    auto L3_node2 = L2_right->m_right; // 'c'
    // L1_right spawns (d, e) and (f, branch_node)
    auto L3_node3 = L1_right->m_left->m_left;  // 'd'
    auto L3_node4 = L1_right->m_left->m_right; // 'e'
    auto L3_node5 = L1_right->m_right->m_left; // 'f'
    auto L3_next = L1_right->m_right->m_right; // Internal node extending to L4

    CHECK(L3_node1->m_symbol == 'b');
    CHECK(L3_node2->m_symbol == 'c');
    CHECK(L3_node3->m_symbol == 'd');
    CHECK(L3_node4->m_symbol == 'e');
    CHECK(L3_node5->m_symbol == 'f');

    // --- Level 4 (1 Symbol: 'g') ---
    auto L4_node1 = L3_next->m_left; // 'g'
    auto L4_next = L3_next->m_right; // Internal node extending to L5

    CHECK(L4_node1->m_symbol == 'g');

    // --- Level 5 (1 Symbol: 'h') ---
    auto L5_node1 = L4_next->m_left; // 'h'
    auto L5_next = L4_next->m_right; // Internal node extending to L6

    CHECK(L5_node1->m_symbol == 'h');

    // --- Level 6 (1 Symbol: 'i') ---
    auto L6_node1 = L5_next->m_left; // 'i'
    auto L6_next = L5_next->m_right; // Internal node extending to L7

    CHECK(L6_node1->m_symbol == 'i');

    // --- Level 7 (1 Symbol: 'j') ---
    auto L7_node1 = L6_next->m_left; // 'j'
    auto L7_next = L6_next->m_right; // Internal node extending to L8

    CHECK(L7_node1->m_symbol == 'j');

    // --- Level 8 (1 Symbol: 'k') ---
    auto L8_node1 = L7_next->m_left; // 'k'

    CHECK(L8_node1->m_symbol == 'k');
}
