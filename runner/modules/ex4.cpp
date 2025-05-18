//
// Created by jamal on 19/05/2025.
//
#include "ex4.h"

namespace Q4_MODULE {
    // Define the global variables
    extern "C" {
        char Lower[1000];
        char Upper[1000];

        void ASM_START_METHOD();
    }


    void FUNC() {
        ASM_START_METHOD();
    }

    void prepare_data(TestCase tCase) {
        std::string input = "abc1!@#XYZ";

        std::memset(Lower, 0, 100);
        std::memset(Upper, 0, 100);

        std::strncpy(Lower, input.c_str(), 99); // avoid overflow
    }

    void verify_results() {
        std::string result;
        for (int i = 0; i < 100 && Upper[i] != '\0'; ++i) {
            result += Upper[i];
        }

        std::cout << "Q4_Output: " << result << "\"\n";
    }
}
