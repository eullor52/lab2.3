#include <gtest/gtest.h>
#include <iostream>

class QuietListener : public testing::EmptyTestEventListener {
public:
    void OnTestPartResult(const testing::TestPartResult&) override {
    }
};

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::TestEventListeners& listeners =
        testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    
    listeners.Append(new QuietListener);

    return RUN_ALL_TESTS();
}