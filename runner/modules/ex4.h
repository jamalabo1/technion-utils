//
// Created by jamal on 18/05/2025.
//

#ifndef Q4_H
#define Q4_H
#include <iostream>
#include <TestCase.h>


namespace Q4_MODULE {
    // define extern
    void FUNC();

    /// this method is for mapping test case data to appropriate memory
    void prepare_data(TestCase tCase);

    /// verify if the test was correct
    void verify_results();
}


#endif //Q4_H
