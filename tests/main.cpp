// #include <gtest/gtest.h>
// #include <iostream>

// class MinimalTestPrinter : public ::testing::EmptyTestEventListener {
// public:
//     void OnTestStart(const ::testing::TestInfo& test_info) override {
//         test_failed_ = false;
//     }

//     void OnTestPartResult(const ::testing::TestPartResult& result) override {
//         if (result.failed()) {
//             test_failed_ = true;
//         }
//     }

//     void OnTestEnd(const ::testing::TestInfo& test_info) override {
//         if (test_failed_) {
//             std::cout << "[  FAILED  ] " << test_info.test_suite_name() 
//                       << "." << test_info.name() << "\n";
//         } else {
//             std::cout << "[       OK ] " << test_info.test_suite_name() 
//                       << "." << test_info.name() << "\n";
//         }
//     }

//     void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override {
//         std::cout << "\n========== Summary ==========\n";
//         std::cout << "Total tests: " << unit_test.total_test_count() << "\n";
//         std::cout << "Passed: " << unit_test.successful_test_count() << "\n";
//         std::cout << "Failed: " << unit_test.failed_test_count() << "\n";
//         std::cout << "=============================\n";
//     }

// private:
//     bool test_failed_ = false;
// };

// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     std::cerr << "========== TESTS ==========\n";
//     auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
//     delete listeners.Release(listeners.default_result_printer());
    
//     listeners.Append(new MinimalTestPrinter);

//     return RUN_ALL_TESTS();
// }