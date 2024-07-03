//
// Created by jamal on 27/06/2024.
//
#include <gtest/gtest.h>

#include <algorithm>
#include <random>

#include "BlockChain.h"

namespace {
    using namespace std;

    class BlockChainTestFixture : public ::testing::Test {
    protected:
        BlockChain mock_chain() {

        }
    };

    class BlockChainTestPersonalBalanceFixture :
            public BlockChainTestFixture,
            public ::testing::WithParamInterface<vector<int>> {

    public:
        static vector<BlockChainTestPersonalBalanceFixture::ParamType> test_values() {
            vector<BlockChainTestPersonalBalanceFixture::ParamType> tests = {
                    {0,  0,  0},
                    {10, 20, 30},
                    {1,  1,  1}
            };
            for (int i = 0; i < 100; i++) {
                std::random_device rd;
                std::mt19937 rng(rd());
                std::uniform_real_distribution<double> dist(1.0, 1000.0);

                int size = dist(rng);

                vector<int> vec;
                vec.reserve(size);

                for (int i = 0; i < size; i++) {
                    double v = dist(rng);
                    vec.push_back(v);
                }

                tests.push_back(vec);
            }

            return tests;
        }
    };

    TEST_F(BlockChainTestFixture, BlockChainGetSize_returns_correct_size) {
        // TODO: add your own test since the size is implementation-dependent.
    }

    TEST_F(BlockChainTestFixture, BlockChainAppendTransaction_appends_to_chain) {
        BlockChain chain{};

        ASSERT_EQ(BlockChainGetSize(chain), 0);

        Transaction trans1 = {
                1,
                "s1",
                "r1"
        };
        string stamp1 = "stamp1";

        BlockChainAppendTransaction(chain, trans1, stamp1);

        EXPECT_EQ(BlockChainGetSize(chain), 1);

        Transaction trans2 = {
                1,
                "s2",
                "r2"
        };
        string stamp2 = "stamp2";

        BlockChainAppendTransaction(chain, trans2, stamp2);

        EXPECT_EQ(BlockChainGetSize(chain), 2);
    }

    TEST_P(BlockChainTestPersonalBalanceFixture, BlockChainPersonalBalance_calculates_correct_balance) {
        BlockChain chain{};

        ASSERT_EQ(BlockChainPersonalBalance(chain, "receiver"), 0);

        auto values = GetParam();

        int expected_value = 0;

        // keep a copy of transactions to save the reference.
        vector<Transaction> transactions;
        for (const auto &item: values) {
            Transaction trans = {
                    (unsigned int) item,
                    "sender",
                    "receiver"
            };
            string timestamp = "timestamp";
            BlockChainAppendTransaction(chain, trans, timestamp);
            transactions.push_back(trans);
            expected_value += item;
        }

        EXPECT_EQ(BlockChainPersonalBalance(chain, "receiver"), expected_value);
    }


    INSTANTIATE_TEST_SUITE_P(BlockChainTestPersonalBalanceFixture,
                             BlockChainTestPersonalBalanceFixture,
                             testing::ValuesIn(
                                     BlockChainTestPersonalBalanceFixture::test_values()
                             )
    );

}