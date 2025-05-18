//
// Created by jamal on 19/05/2025.
//
#include "ex3.h"

#include <iostream>

namespace Q3_MODULE {
    extern "C" {
    struct Node {
        Node *next;
        int32_t data;
    };

    extern Node First;
    extern uint8_t Result;

    void ASM_START_METHOD();
    }

    uint8_t Result = 0;
    Node First = {nullptr};

    void FUNC() {
        ASM_START_METHOD();
    }

    void prepare_data(TestCase tCase) {
        static Node nodes[10];

        std::vector<int> seq = {10, 20, 30, 40};

        int count = seq.size();
        // Create nodes in forward order (first to last)
        for (size_t i = 0; i < count; i++) {
            nodes[i+1].data = seq[i];
            nodes[i+1].next = (i + 1 < count) ? &nodes[i + 2] : nullptr;
        }

        nodes[0].next = &nodes[1];

        extern Node First;
        First = nodes[0];

        extern uint8_t Result;
        Result = 3;  // Number of nodes in the list
    }

    void verify_results() {
        std::cout << "Q3_Result: " << static_cast<int>(Q3_MODULE::Result) << "\n";
    }
}
