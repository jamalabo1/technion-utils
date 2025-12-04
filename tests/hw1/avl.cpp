//
// Created by jamal on 02/12/2025.
//
#include "avl.h"

// test_avl.cpp
#include <catch2/catch_test_macros.hpp>

using IntTree = Avl<int, int>;

TEST_CASE("Empty AVL tree basic behavior", "[avl]") {
    IntTree tree;

    SECTION("find on empty tree returns nullptr") {
        auto v = tree.find(42);
        CHECK(v == nullptr);
    }

    SECTION("inorder on empty tree produces no values") {
        std::vector<int> values;
        tree.inorder([&](std::shared_ptr<int> v) {
            values.push_back(*v);
        });
        CHECK(values.empty());
    }

    SECTION("remove on empty tree throws") {
        CHECK_THROWS_AS(tree.remove(10), std::invalid_argument);
    }
}

TEST_CASE("Single insertion and lookup", "[avl]") {
    IntTree tree;

    auto ptr = tree.insert(5, 50);

    SECTION("insert returns non-null pointer with correct value") {
        REQUIRE(ptr != nullptr);
        CHECK(*ptr == 50);
    }

    SECTION("find returns same value for inserted key") {
        auto found = tree.find(5);
        REQUIRE(found != nullptr);
        CHECK(*found == 50);
    }

    SECTION("inorder visits exactly one value") {
        std::vector<int> values;
        tree.inorder([&](std::shared_ptr<int> v) {
            values.push_back(*v);
        });
        REQUIRE(values.size() == 1);
        CHECK(values[0] == 50);
    }
}

TEST_CASE("Multiple inserts: inorder traversal is sorted by key", "[avl]") {
    IntTree tree;

    // key == value for simplicity
    std::vector<int> keys = {10, 20, 5, 15, 25, 2, 8};
    for (int k : keys) {
        tree.insert(k, k);
    }

    std::vector<int> values;
    tree.inorder([&](std::shared_ptr<int> v) {
        values.push_back(*v);
    });

    // Inorder over a BST should yield sorted keys
    std::vector<int> expected = {2, 5, 8, 10, 15, 20, 25};
    REQUIRE(values.size() == expected.size());
    CHECK(values == expected);
}

TEST_CASE("find returns nullptr for non-existing keys", "[avl]") {
    IntTree tree;

    tree.insert(10, 100);
    tree.insert(5, 50);
    tree.insert(15, 150);

    CHECK(tree.find(7) == nullptr);
    CHECK(tree.find(999) == nullptr);
}

TEST_CASE("Removing a leaf node", "[avl]") {
    IntTree tree;

    // Build a small tree
    tree.insert(10, 10);
    tree.insert(5, 5);
    tree.insert(15, 15);
    tree.insert(2, 2);   // leaf (very likely)
    tree.insert(7, 7);

    // Remove leaf 2
    tree.remove(2);

    SECTION("removed key cannot be found") {
        CHECK(tree.find(2) == nullptr);
    }

    SECTION("other keys still present") {
        CHECK(tree.find(5)  != nullptr);
        CHECK(tree.find(7)  != nullptr);
        CHECK(tree.find(10) != nullptr);
        CHECK(tree.find(15) != nullptr);
    }

    SECTION("inorder still sorted and missing removed key") {
        std::vector<int> values;
        tree.inorder([&](std::shared_ptr<int> v) {
            values.push_back(*v);
        });

        std::vector<int> expected = {5, 7, 10, 15};
        CHECK(values == expected);
    }
}

TEST_CASE("Removing a node with one child", "[avl]") {
    IntTree tree;

    // 10 with a single left child 5 (plus maybe further balancing)
    tree.insert(10, 10);
    tree.insert(5, 5);
    tree.insert(2, 2);   // makes 5 likely to have one child at some point

    // Remove 5 (which at some point will have a single child in the underlying BST)
    tree.remove(5);

    SECTION("removed key cannot be found") {
        CHECK(tree.find(5) == nullptr);
    }

    SECTION("remaining keys are correct") {
        std::vector<int> values;
        tree.inorder([&](std::shared_ptr<int> v) {
            values.push_back(*v);
        });

        // Just check sorted keys present
        std::vector<int> expected = {2, 10};
        CHECK(values == expected);
    }
}

TEST_CASE("Removing a node with two children", "[avl]") {
    IntTree tree;

    // Construct a tree where node '10' has two children (5 and 15)
    tree.insert(20, 20);
    tree.insert(10, 10);
    tree.insert(30, 30);
    tree.insert(5, 5);
    tree.insert(15, 15);
    tree.insert(25, 25);
    tree.insert(35, 35);

    // Remove 10 (two children)
    tree.remove(10);

    SECTION("removed key cannot be found") {
        CHECK(tree.find(10) == nullptr);
    }

    SECTION("other keys still present") {
        for (int k : {5, 15, 20, 25, 30, 35}) {
            INFO("Key " << k << " should still be present");
            CHECK(tree.find(k) != nullptr);
        }
    }

    SECTION("inorder is still sorted and contains all remaining keys") {
        std::vector<int> values;
        tree.inorder([&](std::shared_ptr<int> v) {
            values.push_back(*v);
        });

        std::vector<int> expected = {5, 15, 20, 25, 30, 35};
        CHECK(values == expected);
    }
}

TEST_CASE("Removing multiple times until tree is empty", "[avl]") {
    IntTree tree;

    std::vector<int> keys = {10, 5, 15};
    for (int k : keys) {
        tree.insert(k, k);
    }

    // Remove all keys
    tree.remove(10);
    tree.remove(5);
    tree.remove(15);

    SECTION("tree is logically empty") {
        CHECK(tree.find(10) == nullptr);
        CHECK(tree.find(5)  == nullptr);
        CHECK(tree.find(15) == nullptr);
    }

    SECTION("inorder on empty tree after deletions yields nothing") {
        std::vector<int> values;
        tree.inorder([&](std::shared_ptr<int> v) {
            values.push_back(*v);
        });
        CHECK(values.empty());
    }

    SECTION("removing again from empty tree throws") {
        CHECK_THROWS_AS(tree.remove(42), std::invalid_argument);
    }
}
